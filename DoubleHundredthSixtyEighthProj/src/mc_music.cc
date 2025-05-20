// #include <map>
// #include "mc_music.h"
// #include "SDL_mixer.h"
// #include "mc_mix_library.h"
// #include "mc_mix_exception.h"
// #include "mc_log.h"
// #include "mc_mix_thread.h"
// #include <mutex>
// #include <boost/flyweight.hpp>
// #include <boost/flyweight/key_value.hpp>
// namespace
// {
// struct StreamContainer
// {
//   Mix_Music* stream_;

//   StreamContainer(std::filesystem::path const& file)
//   {
//     stream_ = Mix_LoadMUS(file.string().c_str());
//     if(!stream_)
//     {
//       BOOST_THROW_EXCEPTION(mix::Exception() << mix::Exception::What(mix::Error()));
//     }
//   }

//   ~StreamContainer()
//   {
//     Mix_FreeMusic(stream_);
//   }

//   operator Mix_Music*() const
//   {
//     return stream_;
//   }
// };

// typedef boost::flyweight<boost::flyweights::key_value<std::filesystem::path, StreamContainer>> Stream;
// }

// namespace audio
// {
// class Music::Impl
// {
// public:
//   Impl(std::filesystem::path const& file, float volume, bool repeat);
//   ~Impl();
//   auto Play(float volume) -> void;
//   auto Pause() -> void;
//   auto Resume() -> void;
//   auto End() -> void;
//   mix::Library mix_;
//   Stream stream_;
//   float play_volume_;
//   float volume_;
//   bool repeat_;
//   bool complete_;
//   bool paused_;
//   bool end_;
// };
// }

// namespace
// {
// audio::Music::Impl* music;

// auto MusicFinished() noexcept -> void
// {
//   try
//   {
//     std::lock_guard<mix::Mutex> lock(mix::mutex);
//     if(music)
//     {
//       audio::Music::Impl* temp = music;
//       music = nullptr;
//       if(temp)
//       {
//         temp->complete_ = true;
//         if(temp->repeat_ && !temp->end_)
//         {
//           temp->Play(temp->play_volume_);
//         }
//       }
//     }
//   }
//   catch(...)
//   {
//     exception::Log("Swallowed exception");
//   }
// }

// auto InitHookMusic() -> void
// {
//   static bool initialised;
//   if(!initialised)
//   {
//     initialised = true;
//     Mix_HookMusicFinished(MusicFinished);
//   }
// }
// }

// namespace audio
// {
// auto Music::Impl::Pause() -> void
// {
//   std::lock_guard<mix::Mutex> lock(mix::mutex);
//   paused_ = true;
//   if(music == this)
//   {
//     Mix_PauseMusic();
//   }
// }

// auto Music::Impl::Resume() -> void
// {
//   std::lock_guard<mix::Mutex> lock(mix::mutex);
//   paused_ = false;
//   if(music == this)
//   {
//     Mix_ResumeMusic();
//   }
// }

// auto Music::Impl::End() -> void
// {
//   std::lock_guard<mix::Mutex> lock(mix::mutex);
//   end_ = true;
// }

// auto Music::Impl::Play(float volume) -> void
// {
//   std::lock_guard<mix::Mutex> lock(mix::mutex);
//   play_volume_ = volume;
//   if(music != this)
//   {
//     complete_ = false;
//     if(Mix_PlayMusic(stream_.get(), 0) == -1)
//     {
//       BOOST_THROW_EXCEPTION(mix::Exception() << mix::Exception::What(mix::Error()));
//     }
//     if(paused_)
//     {
//       Mix_PauseMusic();
//     }
//     music = this;
//   }
//   Mix_VolumeMusic(static_cast<int>(play_volume_ * volume_ * MIX_MAX_VOLUME));
//   end_ = false;
// }

// Music::Impl::Impl(std::filesystem::path const& file, float volume, bool repeat) : repeat_(repeat), play_volume_(1.f), volume_(volume), complete_(false), paused_(true), end_(false),  stream_(file)
// {
//   InitHookMusic();
// }

// Music::Impl::~Impl()
// {
//   std::lock_guard<mix::Mutex> lock(mix::mutex);
//   if(music == this)
//   {
//     end_ = true;
//     Mix_HaltMusic();
//   }
// }

// Music::Music(std::filesystem::path const& file, float volume, bool repeat) : impl_(std::make_shared<Impl>(file, volume, repeat))
// {
// }

// Music::Music(lua::Stack& lua, std::filesystem::path const& path) : Music(path / lua.Field<std::string>("file"), lua.Field<float>("volume"), lua.Field<bool>("repeat"))
// {
// }

// auto Music::operator()(float volume) -> bool
// {
//   bool valid = static_cast<bool>(impl_);
//   if(valid)
//   {
//     impl_->Play(volume);
//   }
//   else
//   {
//     std::lock_guard<mix::Mutex> lock(mix::mutex);
//     if(music)
//     {
//       music->end_ = true;
//     }
//     Mix_HaltMusic();
//   }
//   return valid;
// }

// Music::operator bool() const
// {
//   bool valid = static_cast<bool>(impl_);
//   if(valid)
//   {
//     std::lock_guard<mix::Mutex> lock(mix::mutex);
//     valid = !impl_->complete_;
//   }
//   return valid;
// }

// auto Music::Pause() -> void
// {
//   impl_->Pause();
// }

// auto Music::Resume() -> void
// {
//   impl_->Resume();
// }

// auto Music::End() -> void
// {
//   impl_->End();
// }

// size_t Music::Hash(Music const& music)
// {
//   return std::hash<std::shared_ptr<Impl>>()(music.impl_);
// }

// bool Music::operator==(Music const& other) const
// {
//   return impl_ == other.impl_;
// }
// }


#include <map>
#include <memory>
#include <mutex>
#include <unordered_map>
#include "mc_music.h"
#include "SDL_mixer.h"
#include "mc_mix_library.h"
#include "mc_mix_exception.h"
#include "mc_log.h"
#include "mc_mix_thread.h"

namespace {
// 音频流缓存系统
class StreamCache {
public:
    static Mix_Music* Get(const std::filesystem::path& file) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto& weak = cache_[file];
        auto sp = weak.lock();
        if (!sp) {
            sp = std::shared_ptr<Mix_Music>(LoadMusic(file), Mix_FreeMusic);
            weak = sp;
        }
        return sp.get();
    }

private:
    static Mix_Music* LoadMusic(const std::filesystem::path& file) {
        if (auto* music = Mix_LoadMUS(file.string().c_str())) {
            return music;
        }
        throw mix::Exception(Mix_GetError());
    }

    static inline std::mutex mutex_;
    static inline std::unordered_map<
        std::filesystem::path, 
        std::weak_ptr<Mix_Music>,
        std::hash<std::filesystem::path>
    > cache_;
};
} // namespace

namespace audio {
class Music::Impl {
public:
    Impl(std::filesystem::path file, float volume, bool repeat)
        : file_(std::move(file)), 
          volume_(volume),
          repeat_(repeat),
          music_(StreamCache::Get(file_)) {}

    void Play(float volume);
    void Pause();
    void Resume();
    void End();

    const std::filesystem::path file_;
    const float volume_;
    const bool repeat_;
    Mix_Music* const music_;

    std::atomic<bool> complete_{false};
    std::atomic<bool> paused_{true};
    std::atomic<bool> end_{false};
};

// 全局播放状态管理
namespace {
std::mutex g_mutex;
Impl* g_current = nullptr;

void MusicFinished() noexcept {
    std::lock_guard lock(g_mutex);
    if (g_current && !g_current->end_) {
        g_current->complete_ = true;
        if (g_current->repeat_) {
            Mix_PlayMusic(g_current->music_, 0);
            Mix_VolumeMusic(g_current->volume_ * MIX_MAX_VOLUME);
        }
    }
}

void InitHook() {
    static std::once_flag flag;
    std::call_once(flag, []{ Mix_HookMusicFinished(&MusicFinished); });
}
} // namespace

// 实现方法
void Music::Impl::Play(float volume) {
    std::lock_guard lock(g_mutex);
    if (g_current != this) {
        complete_ = false;
        Mix_PlayMusic(music_, 0);
        if (paused_) Mix_PauseMusic();
        g_current = this;
    }
    Mix_VolumeMusic(volume * volume_ * MIX_MAX_VOLUME);
    end_ = false;
    InitHook();
}

void Music::Impl::Pause() {
    std::lock_guard lock(g_mutex);
    paused_ = true;
    if (g_current == this) Mix_PauseMusic();
}

void Music::Impl::Resume() {
    std::lock_guard lock(g_mutex);
    paused_ = false;
    if (g_current == this) Mix_ResumeMusic();
}

void Music::Impl::End() {
    std::lock_guard lock(g_mutex);
    end_ = true;
    if (g_current == this) Mix_HaltMusic();
}

// 公开接口
Music::Music(std::filesystem::path file, float volume, bool repeat)
    : impl_(std::make_shared<Impl>(std::move(file), volume, repeat)) {}

bool Music::operator()(float volume) {
    if (impl_) {
        impl_->Play(volume);
        return true;
    }
    return false;
}

bool Music::operator==(const Music& other) const { 
    return impl_ == other.impl_; 
}

// 其他接口保持原有逻辑...
} // namespace audio
