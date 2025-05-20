// #include "mc_sdl_texture.h"
// #include "mc_sdl_exception.h"
// #include "SDL_image.h"
// #include "mc_img_exception.h"
// #include <boost/multi_index_container.hpp>
// #include <boost/multi_index/hashed_index.hpp>
// #include <boost/multi_index/member.hpp>
// #include <boost/multi_index/composite_key.hpp>
// #include <boost/multi_index/global_fun.hpp>
// namespace
// {
// struct TextureRecord
// {
//   std::weak_ptr<SDL_Texture> texture_;
//   std::filesystem::path path_;
//   SDL_Renderer* renderer_;

//   TextureRecord(std::shared_ptr<SDL_Texture> const& texture, std::filesystem::path const& path, SDL_Renderer* renderer) : texture_(texture), path_(path), renderer_(renderer)
//   {
//   }
// };

// auto Get(TextureRecord const& record) -> SDL_Texture*
// {
//   return record.texture_.lock().get();
// }

// typedef boost::multi_index_container
// <
//   TextureRecord,
//   boost::multi_index::indexed_by
//   <
//     boost::multi_index::hashed_unique
//     <
//       boost::multi_index::composite_key
//       <
//         TextureRecord,
//         boost::multi_index::member<TextureRecord, SDL_Renderer*, &TextureRecord::renderer_>,
//         boost::multi_index::member<TextureRecord, std::filesystem::path, &TextureRecord::path_>
//       >
//     >,
//     boost::multi_index::hashed_unique
//     <
//       boost::multi_index::global_fun
//       <
//         TextureRecord const&, SDL_Texture*, &Get
//       > 
//     >
//   >
// > TextureMap;

// TextureMap cache;

// auto DestroyTexture(SDL_Texture* texture) noexcept -> void
// {
//   auto& view = cache.get<1>();
//   auto iter = view.find(texture);
//   if(iter != view.end())
//   {
//     view.erase(iter);
//   }
//   SDL_DestroyTexture(texture);
// }
// }

// namespace sdl
// {
// Texture::Texture(SDL_Renderer* renderer, SDL_Surface* surface)
// {
//   SDL_Texture* impl = SDL_CreateTextureFromSurface(renderer, surface);
//   if(!impl)
//   {
//     BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error()));
//   }
//   impl_ = std::shared_ptr<SDL_Texture>(impl, SDL_DestroyTexture);
// }

// Texture::Texture(SDL_Renderer* renderer, std::filesystem::path const& file)
// {
//   std::shared_ptr<SDL_Texture> impl;
//   auto iter = cache.find(boost::make_tuple(renderer, file));
//   if(iter != cache.end())
//   {
//     impl = iter->texture_.lock();
//   }

//   if(impl)
//   {
//     impl_ = impl;
//   }
//   else
//   {
//     SDL_Texture* impl = IMG_LoadTexture(renderer, file.string().c_str());
//     if(!impl)
//     {
//       BOOST_THROW_EXCEPTION(img::Exception() << img::Exception::What(img::Error()));
//     }
//     impl_ = std::shared_ptr<SDL_Texture>(impl, DestroyTexture);

//     if(iter == cache.end())
//     {
//       cache.emplace(impl_, file, renderer);
//     }
//     else
//     {
//       cache.modify(iter, [&](TextureRecord& record){record.texture_ = impl_;});
//     }
//   }
// }

// Texture::operator SDL_Texture*() const
// {
//   return impl_.get();
// }

// auto Texture::operator->() const -> SDL_Texture*
// {
//   return impl_.get();
// }

// Texture::operator bool() const
// {
//   return bool(impl_);
// }
// }

#include "mc_sdl_texture.h"
#include "mc_sdl_exception.h"
#include "SDL_image.h"
#include "mc_img_exception.h"
#include <unordered_map>
#include <memory>

namespace {
// 组合键结构体 (替代 boost::multi_index 的 composite_key)
struct TextureKey {
    SDL_Renderer* renderer;
    std::filesystem::path path;

    bool operator==(const TextureKey& other) const {
        return renderer == other.renderer && path == other.path;
    }
};

// 自定义哈希函数 (替代 boost 的复合键哈希)
struct TextureKeyHash {
    size_t operator()(const TextureKey& key) const {
        return std::hash<SDL_Renderer*>{}(key.renderer) ^ 
              (std::hash<std::filesystem::path>{}(key.path) << 1);
    }
};

// 使用两个哈希表替代多索引容器
struct TextureRecord {
    std::weak_ptr<SDL_Texture> texture_;
    std::filesystem::path path_;
    SDL_Renderer* renderer_;

    TextureRecord(std::shared_ptr<SDL_Texture> texture, 
                 std::filesystem::path path,
                 SDL_Renderer* renderer)
        : texture_(std::move(texture)), 
          path_(std::move(path)), 
          renderer_(renderer) {}
};

// 主缓存 (renderer + path 作为键)
std::unordered_map<TextureKey, TextureRecord, TextureKeyHash> main_cache;

// 反向缓存 (SDL_Texture* 作为键)
std::unordered_map<SDL_Texture*, TextureKey> reverse_cache;

void DestroyTexture(SDL_Texture* texture) noexcept {
    // 从反向缓存获取组合键
    if (auto it = reverse_cache.find(texture); it != reverse_cache.end()) {
        const auto& key = it->second;
        // 从主缓存删除
        main_cache.erase(key);
        reverse_cache.erase(it);
    }
    SDL_DestroyTexture(texture);
}
} // namespace

namespace sdl {
Texture::Texture(SDL_Renderer* renderer, SDL_Surface* surface) {
    SDL_Texture* impl = SDL_CreateTextureFromSurface(renderer, surface);
    if (!impl) {
        throw Exception("Failed to create texture: " + std::string(SDL_GetError()));
    }
    impl_ = std::shared_ptr<SDL_Texture>(impl, SDL_DestroyTexture);
}

Texture::Texture(SDL_Renderer* renderer, const std::filesystem::path& file) {
    const TextureKey key{renderer, file};

    // 查找现有缓存
    if (auto it = main_cache.find(key); it != main_cache.end()) {
        if (auto sp = it->second.texture_.lock()) {
            impl_ = std::move(sp);
            return;
        }
        // 弱引用已失效，清理缓存
        reverse_cache.erase(sp.get());
        main_cache.erase(it);
    }

    // 创建新纹理
    SDL_Texture* impl = IMG_LoadTexture(renderer, file.string().c_str());
    if (!impl) {
        throw img::Exception("Failed to load texture: " + 
                            std::string(IMG_GetError()));
    }

    // 创建托管指针
    impl_ = std::shared_ptr<SDL_Texture>(impl, DestroyTexture);
    
    // 更新缓存
    auto [it, inserted] = main_cache.emplace(
        key, 
        TextureRecord{impl_, file, renderer}
    );
    reverse_cache[impl] = key;
}

Texture::operator SDL_Texture*() const { return impl_.get(); }
SDL_Texture* Texture::operator->() const { return impl_.get(); }
Texture::operator bool() const { return static_cast<bool>(impl_); }
} // namespace sdl
