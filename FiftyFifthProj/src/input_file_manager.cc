// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "input_file_manager.h"

#include <memory>
#include <utility>

#include "stl_util.h"
#include "filesystem_utils.h"
#include "parser.h"
#include "scheduler.h"
#include "scope_per_file_provider.h"
#include "tokenizer.h"
#include "trace.h"
#include "vector_utils.h"

namespace {

// The opposite of std::lock_guard.
struct ScopedUnlock {
  ScopedUnlock(std::unique_lock<std::mutex>& lock) : lock_(lock) {
    lock_.unlock();
  }
  ~ScopedUnlock() { lock_.lock(); }

 private:
  std::unique_lock<std::mutex>& lock_;
};

void InvokeFileLoadCallback(const InputFileManager::FileLoadCallback& cb,
                            const ParseNode* node) {
  cb(node);
}

bool DoLoadFile(const LocationRange& origin,
                const BuildSettings* build_settings,
                const SourceFile& name,
                InputFileManager::SyncLoadFileCallback load_file_callback,
                InputFile* file,
                std::vector<Token>* tokens,
                std::unique_ptr<ParseNode>* root,
                Err* err) {
  // Do all of this stuff outside the lock. We should not give out file
  // pointers until the read is complete.
  if (g_scheduler->verbose_logging()) {
    std::string logmsg = name.value();
    if (origin.begin().file())
      logmsg += " (referenced from " + origin.begin().Describe(false) + ")";
    g_scheduler->Log("Loading", logmsg);
  }

  // Read.
  base::FilePath primary_path = build_settings->GetFullPath(name);
  ScopedTrace load_trace(TraceItem::TRACE_FILE_LOAD, name.value());
  if (load_file_callback) {
    if (!load_file_callback(name, file)) {
      *err = Err(origin, "Can't load input file.",
                 "File not mocked by load_file_callback:\n  " + name.value());
      return false;
    }
  } else if (!file->Load(primary_path)) {
    if (!build_settings->secondary_source_path().empty()) {
      // Fall back to secondary source tree.
      base::FilePath secondary_path =
          build_settings->GetFullPathSecondary(name);
      if (!file->Load(secondary_path)) {
        *err = Err(origin, "Can't load input file.",
                   "Unable to load:\n  " + FilePathToUTF8(primary_path) +
                       "\n"
                       "I also checked in the secondary tree for:\n  " +
                       FilePathToUTF8(secondary_path));
        return false;
      }
    } else {
      *err = Err(origin,
                 "Unable to load \"" + FilePathToUTF8(primary_path) + "\".");
      return false;
    }
  }
  load_trace.Done();

  ScopedTrace exec_trace(TraceItem::TRACE_FILE_PARSE, name.value());

  // Tokenize.
  *tokens = Tokenizer::Tokenize(file, err);
  if (err->has_error())
    return false;

  // Parse.
  *root = Parser::Parse(*tokens, err);
  if (err->has_error())
    return false;

  exec_trace.Done();
  return true;
}

}  // namespace

InputFileManager::InputFileData::InputFileData(const SourceFile& file_name)
    : file(file_name), loaded(false), sync_invocation(false) {}

InputFileManager::InputFileData::~InputFileData() = default;

InputFileManager::InputFileManager() = default;

InputFileManager::~InputFileManager() {
  // Should be single-threaded by now.
}

bool InputFileManager::AsyncLoadFile(const LocationRange& origin,
                                     const BuildSettings* build_settings,
                                     const SourceFile& file_name,
                                     const FileLoadCallback& callback,
                                     Err* err) {
  // Try not to schedule callbacks while holding the lock. All cases that don't
  // want to schedule should return early. Otherwise, this will be scheduled
  // after we leave the lock.
  std::function<void()> schedule_this;
  {
    std::lock_guard<std::mutex> lock(lock_);

    InputFileMap::const_iterator found = input_files_.find(file_name);
    if (found == input_files_.end()) {
      // New file, schedule load.
      std::unique_ptr<InputFileData> data =
          std::make_unique<InputFileData>(file_name);
      data->scheduled_callbacks.push_back(callback);
      schedule_this = [this, origin, build_settings, file_name,
                       file = &data->file]() {
        BackgroundLoadFile(origin, build_settings, file_name, file);
      };
      input_files_[file_name] = std::move(data);

    } else {
      InputFileData* data = found->second.get();

      // Prevent mixing async and sync loads. See SyncLoadFile for discussion.
      if (data->sync_invocation) {
        g_scheduler->FailWithError(Err(
            origin, "Load type mismatch.",
            "The file \"" + file_name.value() +
                "\" was previously loaded\n"
                "synchronously (via an import) and now you're trying to load "
                "it "
                "asynchronously\n(via a deps rule). This is a class 2 "
                "misdemeanor: "
                "a single input file must\nbe loaded the same way each time to "
                "avoid blowing my tiny, tiny mind."));
        return false;
      }

      if (data->loaded) {
        // Can just directly issue the callback on the background thread.
        schedule_this = [callback, root = data->parsed_root.get()]() {
          InvokeFileLoadCallback(callback, root);
        };
      } else {
        // Load is pending on this file, schedule the invoke.
        data->scheduled_callbacks.push_back(callback);
        return true;
      }
    }
  }
  g_scheduler->ScheduleWork(std::move(schedule_this));
  return true;
}

const ParseNode* InputFileManager::SyncLoadFile(
    const LocationRange& origin,
    const BuildSettings* build_settings,
    const SourceFile& file_name,
    Err* err) {
  std::unique_lock<std::mutex> lock(lock_);

  InputFileData* data = nullptr;
  InputFileMap::iterator found = input_files_.find(file_name);
  if (found == input_files_.end()) {
    // Haven't seen this file yet, start loading right now.
    std::unique_ptr<InputFileData> new_data =
        std::make_unique<InputFileData>(file_name);
    data = new_data.get();
    data->sync_invocation = true;
    input_files_[file_name] = std::move(new_data);

    ScopedUnlock unlock(lock);
    if (!LoadFile(origin, build_settings, file_name, &data->file, err))
      return nullptr;
  } else {
    // This file has either been loaded or is pending loading.
    data = found->second.get();

    if (!data->sync_invocation) {
      // Don't allow mixing of sync and async loads. If an async load is
      // scheduled and then a bunch of threads need to load it synchronously
      // and block on it loading, it could deadlock or at least cause a lot
      // of wasted CPU while those threads wait for the load to complete (which
      // may be far back in the input queue).
      //
      // We could work around this by promoting the load to a sync load. This
      // requires a bunch of extra code to either check flags and likely do
      // extra locking (bad) or to just do both types of load on the file and
      // deal with the race condition.
      //
      // I have no practical way to test this, and generally we should have
      // all include files processed synchronously and all build files
      // processed asynchronously, so it doesn't happen in practice.
      *err = Err(origin, "Load type mismatch.",
                 "The file \"" + file_name.value() +
                     "\" was previously loaded\n"
                     "asynchronously (via a deps rule) and now you're trying "
                     "to load it "
                     "synchronously.\nThis is a class 2 misdemeanor: a single "
                     "input file "
                     "must be loaded the same way\neach time to avoid blowing "
                     "my tiny, "
                     "tiny mind.");
      return nullptr;
    }

    if (!data->loaded) {
      // Wait for the already-pending sync load to complete.
      if (!data->completion_event) {
        data->completion_event = std::make_unique<AutoResetEvent>();
      }
      {
        ScopedUnlock unlock(lock);
        data->completion_event->Wait();
      }
      // If there were multiple waiters on the same event, we now need to wake
      // up the next one.
      data->completion_event->Signal();
    }
  }

  // The other load could have failed. It is possible that this thread's error
  // will be reported to the scheduler before the other thread's (and the first
  // error reported "wins"). Forward the parse error from the other load for
  // this thread so that the error message is useful.
  if (!data->parsed_root)
    *err = data->parse_error;
  return data->parsed_root.get();
}

void InputFileManager::AddDynamicInput(
    const SourceFile& name,
    InputFile** file,
    std::vector<Token>** tokens,
    std::unique_ptr<ParseNode>** parse_root) {
  std::unique_ptr<InputFileData> data = std::make_unique<InputFileData>(name);
  *file = &data->file;
  *tokens = &data->tokens;
  *parse_root = &data->parsed_root;
  {
    std::lock_guard<std::mutex> lock(lock_);
    dynamic_inputs_.push_back(std::move(data));
  }
}

int InputFileManager::GetInputFileCount() const {
  std::lock_guard<std::mutex> lock(lock_);
  return static_cast<int>(input_files_.size());
}

void InputFileManager::AddAllPhysicalInputFileNamesToVectorSetSorter(
    VectorSetSorter<base::FilePath>* sorter) const {
  std::lock_guard<std::mutex> lock(lock_);

  for (const auto& file : input_files_) {
    if (!file.second->file.physical_name().empty())
      sorter->Add(file.second->file.physical_name());
  }
}

void InputFileManager::BackgroundLoadFile(const LocationRange& origin,
                                          const BuildSettings* build_settings,
                                          const SourceFile& name,
                                          InputFile* file) {
  Err err;
  if (!LoadFile(origin, build_settings, name, file, &err))
    g_scheduler->FailWithError(err);
}

bool InputFileManager::LoadFile(const LocationRange& origin,
                                const BuildSettings* build_settings,
                                const SourceFile& name,
                                InputFile* file,
                                Err* err) {
  std::vector<Token> tokens;
  std::unique_ptr<ParseNode> root;
  bool success = DoLoadFile(origin, build_settings, name, load_file_callback_,
                            file, &tokens, &root, err);
  // Can't return early. We have to ensure that the completion event is
  // signaled in all cases because another thread could be blocked on this one.

  // Save this pointer for running the callbacks below, which happens after the
  // scoped ptr ownership is taken away inside the lock.
  ParseNode* unowned_root = root.get();

  std::vector<FileLoadCallback> callbacks;
  {
    std::lock_guard<std::mutex> lock(lock_);
    DCHECK(input_files_.find(name) != input_files_.end());

    InputFileData* data = input_files_[name].get();
    data->loaded = true;
    if (success) {
      data->tokens = std::move(tokens);
      data->parsed_root = std::move(root);
    } else {
      data->parse_error = *err;
    }

    // Unblock waiters on this event.
    //
    // It's somewhat bad to signal this inside the lock. When it's used, it's
    // lazily created inside the lock. So we need to do the check and signal
    // inside the lock to avoid race conditions on the lazy creation of the
    // lock.
    //
    // We could avoid this by creating the lock every time, but the lock is
    // very seldom used and will generally be NULL, so my current theory is that
    // several signals of a completion event inside a lock is better than
    // creating about 1000 extra locks (one for each file).
    if (data->completion_event)
      data->completion_event->Signal();

    callbacks = std::move(data->scheduled_callbacks);
  }

  // Run pending invocations. Theoretically we could schedule each of these
  // separately to get some parallelism. But normally there will only be one
  // item in the list, so that's extra overhead and complexity for no gain.
  if (success) {
    for (const auto& cb : callbacks)
      cb(unowned_root);
  }
  return success;
}
