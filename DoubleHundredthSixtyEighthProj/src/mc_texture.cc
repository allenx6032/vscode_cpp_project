// #include "mc_texture.h"
// #include "mc_surface.h"
// #include "mc_window.h"
// #include "mc_window_impl.h"
// #include <filesystem>
// #include "mc_render.h"
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
// auto Clip(display::BoundingBox const& current, display::BoundingBox const& clip, std::shared_ptr<SDL_Texture> const& texture) -> display::BoundingBox
// {
//   display::BoundingBox box;
//   if(clip)
//   {
//     if(current)
//     {
//       display::BoundingBox temp(clip, display::BoundingBox());
//       temp.x(temp.x() + current.x());
//       temp.y(temp.y() + current.y());
//       box = display::BoundingBox(current, temp);
//     }
//     else if(texture)
//     {
//       box = display::BoundingBox(display::BoundingBox(0.f, 0.f, static_cast<float>(texture->w), static_cast<float>(texture->h)), clip);
//     }
//     else
//     {
//       box = clip;
//     }
//   }
//   else if(current)
//   {
//     box = display::BoundingBox(current, display::BoundingBox());
//   }
//   return box;
// }

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

// auto MakeTexture(SDL_Renderer* renderer, SDL_Surface* surface) -> std::shared_ptr<SDL_Texture>
// {
//   SDL_Texture* impl = SDL_CreateTextureFromSurface(renderer, surface);
//   if(!impl)
//   {
//     BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
//   }
//   return std::shared_ptr<SDL_Texture>(impl, SDL_DestroyTexture);
// }

// auto MakeTexture(SDL_Renderer* renderer, std::filesystem::path const& file) -> std::shared_ptr<SDL_Texture>
// {
//   std::shared_ptr<SDL_Texture> texture;

//   auto iter = cache.find(boost::make_tuple(renderer, file));
//   if(iter != cache.end())
//   {
//     texture = iter->texture_.lock();
//   }

//   if(!texture)
//   {
//     SDL_Texture* sdl_texture = IMG_LoadTexture(renderer, file.string().c_str());
//     if(!sdl_texture)
//     {
//       BOOST_THROW_EXCEPTION(img::Exception() << img::Exception::What(img::Error()));
//     }
//     texture = std::shared_ptr<SDL_Texture>(sdl_texture, DestroyTexture);
          
//     auto iter = cache.find(boost::make_tuple(renderer, file));
//     if(iter == cache.end())
//     {
//       cache.emplace(texture, file, renderer);
//     }
//     else
//     {
//       cache.modify(iter, [&](TextureRecord& record){record.texture_ = texture;});
//     }
//   }
//   return texture;
// }
// }

// namespace display
// {
// class Texture::Impl final : public std::enable_shared_from_this<Impl>
// {
// public:
//   Impl() = default;
//   Impl(std::filesystem::path const& file, Window& window, BoundingBox const& clip);
//   Impl(std::string const& text, sdl::Font const& font, float width, Window& window, BoundingBox const& clip);
//   Impl(std::string const& text, sdl::Font const& font, Window& window, BoundingBox const& clip);
//   auto Render(BoundingBox const& source, BoundingBox const& destination, float parallax, bool horizontal, bool vertical, double angle, Modulation const& modulation) -> bool;
//   auto Check() const -> bool;
//   display::Shape Shape() const;
//   Window::WeakPtr window_;
//   std::shared_ptr<SDL_Texture> texture_;
//   BoundingBox clip_;
// };

// Texture::Impl::Impl(std::filesystem::path const& file, Window& window, BoundingBox const& clip) : window_(window), texture_(MakeTexture(window.impl_->renderer_, file)), clip_(Clip(BoundingBox(), clip, texture_))
// {
// }

// Texture::Impl::Impl(std::string const& text, sdl::Font const& font, float width, Window& window, BoundingBox const& clip) : window_(window), texture_(MakeTexture(window.impl_->renderer_, static_cast<SDL_Surface*>(sdl::Surface(text, font, static_cast<Uint32>(width))))), clip_(Clip(BoundingBox(), clip, texture_))
// {
// }

// Texture::Impl::Impl(std::string const& text, sdl::Font const& font, Window& window, BoundingBox const& clip) : window_(window), texture_(MakeTexture(window.impl_->renderer_, static_cast<SDL_Surface*>(sdl::Surface(text, font)))), clip_(Clip(BoundingBox(), clip, texture_))
// {
// }

// auto Texture::Impl::Render(BoundingBox const& source, BoundingBox const& destination, float parallax, bool horizontal, bool vertical, double angle, Modulation const& modulation) -> bool
// {
//   bool locked = false;
//   if(auto window = window_.Lock())
//   {
//     auto& impl = window.impl_;
//     display::Render(impl->window_, impl->renderer_, texture_.get(), Clip(clip_, source, texture_), destination, parallax, horizontal, vertical, angle, modulation, impl->zoom_, &impl->view_, impl->scale_, &impl->view_angle_);
//     locked = true;
//   }
//   return locked;
// }

// auto Texture::Impl::Check() const -> bool
// {
//   return static_cast<bool>(window_.Lock());
// }

// auto Texture::Impl::Shape() const -> display::Shape
// {
//   if(clip_)
//   {
//     return display::Shape(clip_.w(), clip_.h());
//   }
//   else if(texture_)
//   {
//     return display::Shape(static_cast<float>(texture_->w), static_cast<float>(texture_->h));
//   }
//   return display::Shape();
// }

// auto Texture::operator()(BoundingBox const& source, BoundingBox const& destination, float parallax, bool horizontal, bool vertical, double angle, Modulation const& modulation) -> bool
// {
//   return static_cast<bool>(impl_) && impl_->Render(source, destination, parallax, horizontal, vertical, angle, modulation);
// }

// Texture::operator bool() const
// {
//   return static_cast<bool>(impl_) && impl_->Check();
// }

// auto Texture::Shape() const -> display::Shape
// {
//   return impl_->Shape();
// }

// Texture::Texture(std::filesystem::path const& file, Window& window, BoundingBox const& clip) : impl_(std::make_shared<Impl>(file, window, clip))
// { 
// }

// Texture::Texture(std::string const& text, sdl::Font const& font, float width, Window& window, BoundingBox const& clip) : impl_(std::make_shared<Impl>(text, font, width, window, clip))
// {
// }

// Texture::Texture(std::string const& text, sdl::Font const& font, Window& window, BoundingBox const& clip) : impl_(std::make_shared<Impl>(text, font, window, clip))
// {
// }
// }



#include "mc_texture.h"
#include "mc_surface.h"
#include "mc_window.h"
#include "mc_window_impl.h"
#include <filesystem>
#include "mc_render.h"
#include "mc_sdl_exception.h"
#include "SDL_image.h"
#include "mc_img_exception.h"
#include <unordered_map>

namespace 
{
struct TextureKey {
    SDL_Renderer* renderer;
    std::filesystem::path path;

    bool operator==(const TextureKey& other) const {
        return renderer == other.renderer && path == other.path;
    }
};

struct TextureKeyHash {
    size_t operator()(const TextureKey& key) const {
        size_t hash_renderer = std::hash<SDL_Renderer*>{}(key.renderer);
        size_t hash_path = std::hash<std::filesystem::path>{}(key.path);
        return hash_renderer ^ (hash_path << 1);
    }
};

auto Clip(display::BoundingBox const& current, display::BoundingBox const& clip, std::shared_ptr<SDL_Texture> const& texture) -> display::BoundingBox
{
  display::BoundingBox box;
  if(clip)
  {
    if(current)
    {
      display::BoundingBox temp(clip, display::BoundingBox());
      temp.x(temp.x() + current.x());
      temp.y(temp.y() + current.y());
      box = display::BoundingBox(current, temp);
    }
    else if(texture)
    {
      box = display::BoundingBox(display::BoundingBox(0.f, 0.f, static_cast<float>(texture->w), static_cast<float>(texture->h)), clip);
    }
    else
    {
      box = clip;
    }
  }
  else if(current)
  {
    box = display::BoundingBox(current, display::BoundingBox());
  }
  return box;
}

struct TextureRecord
{
  std::weak_ptr<SDL_Texture> texture_;
  std::filesystem::path path_;
  SDL_Renderer* renderer_;

  TextureRecord(std::shared_ptr<SDL_Texture> const& texture, std::filesystem::path const& path, SDL_Renderer* renderer) 
    : texture_(texture), path_(path), renderer_(renderer) {}
};

std::unordered_map<TextureKey, TextureRecord, TextureKeyHash> cache_by_key;
std::unordered_map<SDL_Texture*, TextureKey> cache_by_texture;

auto DestroyTexture(SDL_Texture* texture) noexcept -> void
{
  auto tex_it = cache_by_texture.find(texture);
  if (tex_it != cache_by_texture.end()) {
      const TextureKey& key = tex_it->second;
      cache_by_key.erase(key);
      cache_by_texture.erase(tex_it);
  }
  SDL_DestroyTexture(texture);
}

auto MakeTexture(SDL_Renderer* renderer, SDL_Surface* surface) -> std::shared_ptr<SDL_Texture>
{
  SDL_Texture* impl = SDL_CreateTextureFromSurface(renderer, surface);
  if(!impl)
  {
    BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
  }
  return std::shared_ptr<SDL_Texture>(impl, SDL_DestroyTexture);
}

auto MakeTexture(SDL_Renderer* renderer, std::filesystem::path const& file) -> std::shared_ptr<SDL_Texture>
{
  std::shared_ptr<SDL_Texture> texture;
  TextureKey key{renderer, file};

  auto it = cache_by_key.find(key);
  if (it != cache_by_key.end()) {
      texture = it->second.texture_.lock();
      if (texture) {
          return texture;
      } else {
          // Remove expired entry
          SDL_Texture* old_tex = it->second.texture_.lock().get();
          if (old_tex) {
              cache_by_texture.erase(old_tex);
          }
          cache_by_key.erase(it);
      }
  }

  SDL_Texture* sdl_texture = IMG_LoadTexture(renderer, file.string().c_str());
  if(!sdl_texture)
  {
    BOOST_THROW_EXCEPTION(img::Exception() << img::Exception::What(img::Error()));
  }
  texture = std::shared_ptr<SDL_Texture>(sdl_texture, DestroyTexture);

  // Update caches
  TextureRecord record{texture, file, renderer};
  auto [new_it, inserted] = cache_by_key.emplace(key, record);
  if (!inserted) {
      // Update existing record
      new_it->second = record;
      // Update texture map
      SDL_Texture* old_tex = new_it->second.texture_.lock().get();
      if (old_tex) {
          cache_by_texture.erase(old_tex);
      }
  }
  cache_by_texture.emplace(texture.get(), key);

  return texture;
}
}

namespace display
{
// Texture::Impl及其他成员保持不变，与原始代码相同
class Texture::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl() = default;
  Impl(std::filesystem::path const& file, Window& window, BoundingBox const& clip);
  Impl(std::string const& text, sdl::Font const& font, float width, Window& window, BoundingBox const& clip);
  Impl(std::string const& text, sdl::Font const& font, Window& window, BoundingBox const& clip);
  auto Render(BoundingBox const& source, BoundingBox const& destination, float parallax, bool horizontal, bool vertical, double angle, Modulation const& modulation) -> bool;
  auto Check() const -> bool;
  display::Shape Shape() const;
  Window::WeakPtr window_;
  std::shared_ptr<SDL_Texture> texture_;
  BoundingBox clip_;
};

Texture::Impl::Impl(std::filesystem::path const& file, Window& window, BoundingBox const& clip) 
  : window_(window), texture_(MakeTexture(window.impl_->renderer_, file)), clip_(Clip(BoundingBox(), clip, texture_)) {}

Texture::Impl::Impl(std::string const& text, sdl::Font const& font, float width, Window& window, BoundingBox const& clip) 
  : window_(window), texture_(MakeTexture(window.impl_->renderer_, static_cast<SDL_Surface*>(sdl::Surface(text, font, static_cast<Uint32>(width))))), clip_(Clip(BoundingBox(), clip, texture_)) {}

Texture::Impl::Impl(std::string const& text, sdl::Font const& font, Window& window, BoundingBox const& clip) 
  : window_(window), texture_(MakeTexture(window.impl_->renderer_, static_cast<SDL_Surface*>(sdl::Surface(text, font)))), clip_(Clip(BoundingBox(), clip, texture_)) {}

auto Texture::Impl::Render(BoundingBox const& source, BoundingBox const& destination, float parallax, bool horizontal, bool vertical, double angle, Modulation const& modulation) -> bool
{
  bool locked = false;
  if(auto window = window_.Lock())
  {
    auto& impl = window.impl_;
    display::Render(impl->window_, impl->renderer_, texture_.get(), Clip(clip_, source, texture_), destination, parallax, horizontal, vertical, angle, modulation, impl->zoom_, &impl->view_, impl->scale_, &impl->view_angle_);
    locked = true;
  }
  return locked;
}

auto Texture::Impl::Check() const -> bool
{
  return static_cast<bool>(window_.Lock());
}

auto Texture::Impl::Shape() const -> display::Shape
{
  if(clip_)
  {
    return display::Shape(clip_.w(), clip_.h());
  }
  else if(texture_)
  {
    return display::Shape(static_cast<float>(texture_->w), static_cast<float>(texture_->h));
  }
  return display::Shape();
}

auto Texture::operator()(BoundingBox const& source, BoundingBox const& destination, float parallax, bool horizontal, bool vertical, double angle, Modulation const& modulation) -> bool
{
  return static_cast<bool>(impl_) && impl_->Render(source, destination, parallax, horizontal, vertical, angle, modulation);
}

Texture::operator bool() const
{
  return static_cast<bool>(impl_) && impl_->Check();
}

auto Texture::Shape() const -> display::Shape
{
  return impl_->Shape();
}

Texture::Texture(std::filesystem::path const& file, Window& window, BoundingBox const& clip) 
  : impl_(std::make_shared<Impl>(file, window, clip)) {} 

Texture::Texture(std::string const& text, sdl::Font const& font, float width, Window& window, BoundingBox const& clip) 
  : impl_(std::make_shared<Impl>(text, font, width, window, clip)) {}

Texture::Texture(std::string const& text, sdl::Font const& font, Window& window, BoundingBox const& clip) 
  : impl_(std::make_shared<Impl>(text, font, window, clip)) {}
}