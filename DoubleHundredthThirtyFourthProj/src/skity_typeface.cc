#include <codecvt>
#include <glm/gtc/matrix_transform.hpp>
#include <locale>
#include <skity_typeface.hpp>
#include <unordered_map>
#ifdef ENABLE_TEXT_RENDER
#include "skity_ft_library_wrap.hpp"
#endif

namespace skity {

#ifdef ENABLE_TEXT_RENDER

std::weak_ptr<FTLibrary> g_ft_library;

static std::shared_ptr<FTLibrary> GlobalFTLibrary() {
  auto library = g_ft_library.lock();
  if (library) {
    return library;
  } else {
    library = std::make_shared<FTLibrary>();
    g_ft_library = library;
    return library;
  }
}

// ---------------------------- Impl ----------------------------------
class Typeface::Impl {
 public:
  Impl() = default;
  ~Impl() = default;

  void SetFTLibrary(std::shared_ptr<FTLibrary> library) {
    ft_library_ = std::move(library);
  }

  bool LoadTypefaceFromFile(const char* path) {
    ft_typeface_ = ft_library_->LoadTypeface(path);
    return ft_typeface_ != nullptr;
  }

  bool LoadFromData(const Data* data) {
    ft_typeface_ = ft_library_->LoadTypeface(data);
    return ft_typeface_ != nullptr;
  }

  GlyphInfo GetGlyphInfo(GlyphID glyph_id, float font_size, bool load_path) {
    if (glyph_cache_.count(glyph_id) != 0) {
      if (load_path && glyph_cache_[glyph_id].path.isEmpty()) {
        glyph_cache_[glyph_id] = LoadGlyphInfo(glyph_id, font_size, true);
      }
      // got catch
      return ScaleInfo(glyph_cache_[glyph_id], font_size, load_path);
    } else {
      GlyphInfo info = LoadGlyphInfo(glyph_id, font_size, load_path);
      glyph_cache_[glyph_id] = info;
      return info;
    }
  }

  GlyphBitmapInfo getGlyphBitmapInfo(GlyphID glyph_id, float font_size) {
    auto info = ft_typeface_->LoadGlyphBitmap(glyph_id, font_size);

    return {info.width, info.height, info.buffer};
  }

  bool containGlyph(GlyphID glyph_id) {
    return ft_typeface_->containGlyph(glyph_id);
  }

 private:
  GlyphInfo ScaleInfo(GlyphInfo const& base_info, float target_font_size,
                      bool load_path) {
    if (target_font_size == base_info.font_size) {
      return base_info;
    }
    float scale = target_font_size / base_info.font_size;
    GlyphInfo target_info;
    target_info.id = base_info.id;
    target_info.advance_x = base_info.advance_x * scale;
    target_info.advance_y = base_info.advance_y * scale;
    target_info.ascent = base_info.ascent * scale;
    target_info.descent = base_info.descent * scale;
    target_info.width = base_info.width * scale;
    target_info.height = base_info.height * scale;
    target_info.bearing_x = base_info.bearing_x * scale;
    target_info.font_size = target_font_size;
    if (load_path) {
      target_info.path = base_info.path.copyWithScale(scale);
    }
    return target_info;
  }

  GlyphInfo LoadGlyphInfo(GlyphID glyph_id, float font_size, bool load_path) {
    GlyphInfo glyph_info{};

    if (!ft_typeface_) {
      glyph_info.id = 0;
      return glyph_info;
    }

    auto ft_info = ft_typeface_->LoadGlyph(glyph_id, font_size, load_path);

    glyph_info.id = glyph_id;
    glyph_info.path = ft_info.path;
    glyph_info.path_font_size = ft_info.path_font_size;
    glyph_info.advance_x = ft_info.advance_x;
    glyph_info.advance_y = ft_info.advance_y;
    glyph_info.ascent = ft_info.bearing_y;
    glyph_info.descent = glyph_info.height - glyph_info.ascent;
    glyph_info.bearing_x = ft_info.bearing_x;

    glyph_info.font_size = font_size;

    return glyph_info;
  }

 private:
  std::shared_ptr<FTLibrary> ft_library_;
  std::unique_ptr<FTTypeFace> ft_typeface_;
  std::unordered_map<GlyphID, GlyphInfo> glyph_cache_;
};

#else
class Typeface::Impl {
 public:
 private:
};
#endif

Typeface::~Typeface() = default;

std::shared_ptr<Typeface> Typeface::MakeFromFile(const char* path) {
#ifdef ENABLE_TEXT_RENDER
  std::unique_ptr<Typeface::Impl> impl{new Typeface::Impl};
  impl->SetFTLibrary(GlobalFTLibrary());
  if (!impl->LoadTypefaceFromFile(path)) {
    return nullptr;
  }

  std::shared_ptr<Typeface> typeface{new Typeface};
  typeface->impl_ = std::move(impl);
  return typeface;
#else
  return nullptr;
#endif
}

std::shared_ptr<Typeface> Typeface::MakeFromData(
    const std::shared_ptr<Data>& data) {
#ifdef ENABLE_TEXT_RENDER
  std::unique_ptr<Typeface::Impl> impl{new Typeface::Impl};
  impl->SetFTLibrary(GlobalFTLibrary());
  if (!impl->LoadFromData(data.get())) {
    return nullptr;
  }

  std::shared_ptr<Typeface> typeface{new Typeface};
  typeface->impl_ = std::move(impl);
  typeface->data_ = data;
  return typeface;
#else
  return nullptr;
#endif
}

void Typeface::getGlyphInfo(const std::vector<GlyphID>& glyph_id,
                            float font_size, std::vector<GlyphInfo>& info,
                            bool load_path) {
  for (auto id : glyph_id) {
    info.emplace_back(getGlyphInfo(id, font_size, load_path));
  }
}

GlyphInfo Typeface::getGlyphInfo(GlyphID glyph_id, float font_size,
                                 bool load_path) {
#ifdef ENABLE_TEXT_RENDER
  return impl_->GetGlyphInfo(glyph_id, font_size, load_path);
#else
  return {};
#endif
}

GlyphBitmapInfo Typeface::getGlyphBitmapInfo(GlyphID glyph_id,
                                             float font_size) {
#ifdef ENABLE_TEXT_RENDER
  return impl_->getGlyphBitmapInfo(glyph_id, font_size);
#else
  return {};
#endif
}

bool Typeface::containGlyph(GlyphID glyph_id) {
#ifdef ENABLE_TEXT_RENDER
  return impl_->containGlyph(glyph_id);
#else
  return false;
#endif
}

}  // namespace skity
