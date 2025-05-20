// #ifndef FONT_IMPL_H_
// #define FONT_IMPL_H_
// #include "mc_lua_stack.h"
// #include "SDL_TTF.h"
// #include "mc_ttf_library.h"
// #include <filesystem>
// #include <boost/flyweight.hpp>
// #include <boost/flyweight/key_value.hpp>
// #include "mc_ttf_exception.h"
// namespace sdl
// {
// typedef std::pair<std::filesystem::path, int> FontKey;

// struct FontContainer
// {
//   TTF_Font* font_;

//   FontContainer(FontKey const& key)
//   {
//     font_ = TTF_OpenFont(key.first.string().c_str(), key.second);
//     if(!font_)
//     {
//       BOOST_THROW_EXCEPTION(ttf::Exception() << ttf::Exception::What(ttf::Error()));
//     }
//   }

//   ~FontContainer()
//   {
//     if(font_)
//     {
//       TTF_CloseFont(font_);
//     }
//   }

//   operator TTF_Font*() const
//   {
//     return font_;
//   }
// };

// typedef boost::flyweight<boost::flyweights::key_value<FontKey, FontContainer>> FlyweightFont;

// class Font::Impl
// {
// public:
//   Impl(lua::Stack& lua, std::filesystem::path const& path);
//   auto LineSpacing() const -> float;
//   ttf::Library ttf_;
//   FlyweightFont font_;
//   bool bold_;
//   bool italic_;
//   bool outline_;
// };
// }
// #endif


#ifndef FONT_IMPL_H_
#define FONT_IMPL_H_
#include "mc_lua_stack.h"
#include "SDL_ttf.h"
#include "mc_ttf_library.h"
#include <filesystem>
#include <unordered_map>
#include <memory>
#include <mutex>
#include "mc_ttf_exception.h"

namespace sdl {
// 字体键类型（路径+字号）
using FontKey = std::pair<std::filesystem::path, int>;

// 字体资源容器
class FontContainer {
public:
    FontContainer(const FontKey& key) {
        font_ = TTF_OpenFont(key.first.string().c_str(), key.second);
        if (!font_) {
            throw ttf::Exception(TTF_GetError());
        }
    }

    ~FontContainer() {
        if (font_) {
            TTF_CloseFont(font_);
        }
    }

    operator TTF_Font*() const { return font_; }

private:
    TTF_Font* font_ = nullptr;
};

// 字体缓存系统
class FontCache {
public:
    static std::shared_ptr<FontContainer> Get(const FontKey& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto& weak = cache_[key];
        auto sp = weak.lock();
        if (!sp) {
            sp = std::make_shared<FontContainer>(key);
            weak = sp;
        }
        return sp;
    }

private:
    static inline std::mutex mutex_;
    static inline std::unordered_map<FontKey, std::weak_ptr<FontContainer>> cache_;
};

// 字体实现类
class Font::Impl {
public:
    Impl(lua::Stack& lua, const std::filesystem::path& path, int size)
        : font_(FontCache::Get({path, size})) {}

    float LineSpacing() const {
        return TTF_FontLineSpacing(*font_);
    }

    // 字体样式操作（示例）
    void SetBold(bool bold) { 
        TTF_SetFontStyle(*font_, bold ? TTF_STYLE_BOLD : TTF_STYLE_NORMAL);
    }

private:
    std::shared_ptr<FontContainer> font_;
    ttf::Library ttf_; // 维持 TTF 库初始化
};

} // namespace sdl
#endif
