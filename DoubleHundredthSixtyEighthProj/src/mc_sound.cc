// #include "mc_sound.h"
// #include "SDL_mixer.h"
// #include "mc_mix_exception.h"
// #include "mc_mix_library.h"
// #include "mc_log.h"
// #include <iostream>
// #include <vector>
// #include "mc_mix_thread.h"
// #include <mutex>
// #include <boost/flyweight.hpp>
// #include <boost/flyweight/key_value.hpp>
// namespace
// {
// struct ChunkContainer
// {
//   Mix_Chunk* chunk_;

//   ChunkContainer(std::filesystem::path const& file)
//   {
//     chunk_ = Mix_LoadWAV(file.string().c_str());
//     if(!chunk_)
//     {
//       BOOST_THROW_EXCEPTION(mix::Exception() << mix::Exception::What(mix::Error()));
//     }
//   }

//   ~ChunkContainer()
//   {
//     Mix_FreeChunk(chunk_);
//   }

//   operator Mix_Chunk*() const
//   {
//     return chunk_;
//   }
// };

// typedef boost::flyweight<boost::flyweights::key_value<std::filesystem::path, ChunkContainer>> Chunk;
// }

// namespace audio
// {
// class Sound::Impl
// {
// public:
//   Impl(std::filesystem::path const& file, float volume, bool repeat);
//   ~Impl();
//   auto Pause() -> void;
//   auto Resume() -> void;
//   auto End() -> void;
//   auto Play(float volume) -> void;
//   mix::Library mix_;
//   Chunk chunk_;
//   int channel_;
//   bool repeat_;
//   float play_volume_;
//   float volume_;
//   bool paused_;
//   bool end_;
// };
// }

// namespace
// {
// std::vector<audio::Sound::Impl*> sounds;

// auto ChannelFinished(int channel) noexcept -> void
// {
//   try
//   {
//     std::lock_guard<mix::Mutex> lock(mix::mutex);
//     if(channel < static_cast<int>(sounds.size()))
//     {
//       audio::Sound::Impl* sound = sounds[channel];
//       sounds[channel] = nullptr;
//       if(sound)
//       {
//         sound->channel_ = -1;
//         if(sound->repeat_ && !sound->end_)
//         {
//           sound->Play(sound->play_volume_);
//         }
//       }
//     }
//   }
//   catch(...)
//   {
//     exception::Log("Swallowed exception");
//   }
// }

// auto InitHookSound() -> void
// {
//   static bool initialised;
//   if(!initialised)
//   {
//     Mix_ChannelFinished(ChannelFinished);
//     initialised = true;
//   }
// }
// }

// namespace audio
// {
// auto Sound::Impl::Play(float volume) -> void
// {
//   std::lock_guard<mix::Mutex> lock(mix::mutex);
//   play_volume_ = volume;
//   if(channel_ == -1)
//   {
//     int size = Mix_AllocateChannels(-1);
//     if(Mix_Playing(-1) < size)
//     {
//       channel_ = Mix_PlayChannel(-1, chunk_.get(), 0);
//       if(channel_ == -1)
//       {
//         BOOST_THROW_EXCEPTION(mix::Exception() << mix::Exception::What(mix::Error()));
//       }
//       if(paused_)
//       {
//         Mix_Pause(channel_);
//       }
//       sounds.resize(size);
//       sounds[channel_] = this;
//     }
//   }
//   if(channel_ != -1)
//   {
//     Mix_Volume(channel_, static_cast<int>(play_volume_ * volume_ * MIX_MAX_VOLUME));
//   }
//   end_ = false;
// }

// Sound::Impl::Impl(std::filesystem::path const& file, float volume, bool repeat) : channel_(-1), repeat_(repeat), play_volume_(1.f), volume_(volume), paused_(true), end_(false), chunk_(file)
// {
//   InitHookSound();
// }

// Sound::Impl::~Impl()
// {
//   std::lock_guard<mix::Mutex> lock(mix::mutex);
//   if(channel_ != -1)
//   {
//     end_ = true;
//     Mix_HaltChannel(channel_);
//   }
// }

// auto Sound::Impl::Pause() -> void
// {
//   std::lock_guard<mix::Mutex> lock(mix::mutex);
//   paused_ = true;
//   if(channel_ != -1)
//   {
//     Mix_Pause(channel_);
//   }
// }

// auto Sound::Impl::Resume() -> void
// {
//   std::lock_guard<mix::Mutex> lock(mix::mutex);
//   paused_ = false;
//   if(channel_ != -1)
//   {
//     Mix_Resume(channel_);
//   }
// }

// auto Sound::Impl::End() -> void
// {
//   std::lock_guard<mix::Mutex> lock(mix::mutex);
//   end_ = true;
// }

// Sound::Sound(std::filesystem::path const& file, float volume, bool repeat) : impl_(std::make_shared<Impl>(file, volume, repeat))
// {
// }

// Sound::Sound(lua::Stack& lua, std::filesystem::path const& path) : Sound(path / lua.Field<std::string>("file"), lua.Field<float>("volume"), lua.Field<bool>("repeat"))
// {
// }

// auto Sound::Pause() -> void
// {
//   impl_->Pause();
// }

// auto Sound::Resume() -> void
// {
//   impl_->Resume();
// }

// auto Sound::End() -> void
// {
//   impl_->End();
// }

// auto Sound::operator()(float volume) -> bool
// {
//   bool valid = static_cast<bool>(impl_);
//   if(valid)
//   {
//     impl_->Play(volume);
//   }
//   return valid;
// }

// Sound::operator bool() const
// {
//   bool valid = static_cast<bool>(impl_);
//   if(valid)
//   {
//     std::lock_guard<mix::Mutex> lock(mix::mutex);
//     valid = (impl_->channel_ != -1);
//   }
//   return valid;
// }

// size_t Sound::Hash(Sound const& sound)
// {
//   return std::hash<std::shared_ptr<Impl>>()(sound.impl_);
// }

// bool Sound::operator==(Sound const& other) const
// {
//   return impl_ == other.impl_;
// }
// }

#include "mc_sound.h"
#include "SDL_mixer.h"
#include "mc_mix_exception.h"
#include "mc_mix_library.h"
#include "mc_log.h"
#include <iostream>
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace {
// 音频块缓存系统
class ChunkCache {
public:
    static Mix_Chunk* Get(const std::filesystem::path& file) {
        std::lock_guard lock(mutex_);
        auto& weak = cache_[file];
        auto sp = weak.lock();
        
        if (!sp) {
            if (Mix_Chunk* chunk = Mix_LoadWAV(file.string().c_str())) {
                sp.reset(chunk, Mix_FreeChunk); // 自定义删除器
                weak = sp;
            } else {
                throw mix::Exception(Mix_GetError());
            }
        }
        return sp.get();
    }

private:
    static inline std::mutex mutex_;
    static inline std::unordered_map<
        std::filesystem::path, 
        std::weak_ptr<Mix_Chunk>,
        std::hash<std::filesystem::path>
    > cache_;
};

// 全局播放状态跟踪
struct SoundState {
    std::mutex mutex;
    std::vector<audio::Sound::Impl*> sounds;
} g_sound_state;

void ChannelFinished(int channel) noexcept {
    std::lock_guard lock(g_sound_state.mutex);
    try {
        if (channel >= 0 && channel < g_sound_state.sounds.size()) {
            if (auto* impl = g_sound_state.sounds[channel]) {
                impl->channel_ = -1;
                if (impl->repeat_ && !impl->end_) {
                    impl->Play(impl->play_volume_);
                }
            }
        }
    } catch (...) {
        exception::Log("Channel callback error");
    }
}

void InitHook() {
    static std::once_flag flag;
    std::call_once(flag, []{ Mix_ChannelFinished(ChannelFinished); });
}
} // namespace

namespace audio {
class Sound::Impl {
public:
    Impl(std::filesystem::path file, float volume, bool repeat)
        : chunk_(ChunkCache::Get(file)),
          volume_(volume),
          repeat_(repeat) {
        InitHook();
    }

    void Play(float volume);
    void Pause();
    void Resume();
    void End();

    Mix_Chunk* chunk_;
    float volume_;
    bool repeat_;
    std::atomic<int> channel_{-1};
    std::atomic<float> play_volume_{1.0f};
    std::atomic<bool> paused_{true};
    std::atomic<bool> end_{false};
};

void Sound::Impl::Play(float volume) {
    std::lock_guard lock(g_sound_state.mutex);
    play_volume_ = volume;
    
    if (channel_ == -1) {
        // 获取可用音频通道
        const int allocated = Mix_AllocateChannels(-1);
        channel_ = Mix_PlayChannel(-1, chunk_, repeat_ ? -1 : 0);
        
        if (channel_ == -1) {
            throw mix::Exception(Mix_GetError());
        }

        // 扩展容器尺寸并记录实例
        if (channel_ >= g_sound_state.sounds.size()) {
            g_sound_state.sounds.resize(channel_ + 1);
        }
        g_sound_state.sounds[channel_] = this;
    }

    // 设置音量并更新状态
    Mix_Volume(channel_, play_volume_ * volume_ * MIX_MAX_VOLUME);
    paused_ = false;
    end_ = false;
    if (paused_) Mix_Pause(channel_);
}

void Sound::Impl::Pause() {
    if (channel_ != -1) {
        Mix_Pause(channel_);
        paused_ = true;
    }
}

void Sound::Impl::Resume() {
    if (channel_ != -1) {
        Mix_Resume(channel_);
        paused_ = false;
    }
}

void Sound::Impl::End() {
    if (channel_ != -1) {
        Mix_HaltChannel(channel_);
        end_ = true;
        channel_ = -1;
    }
}

// 公开接口保持不变
Sound::Sound(std::filesystem::path file, float volume, bool repeat)
    : impl_(std::make_shared<Impl>(std::move(file), volume, repeat)) {}

bool Sound::operator()(float volume) {
    if (impl_) {
        impl_->Play(volume);
        return true;
    }
    return false;
}

// 其他接口实现...
} // namespace audio
