#include <skity_text_run.hpp>
#include <skity_typeface.hpp>

namespace skity {

TextRun::TextRun(std::shared_ptr<Typeface> const& typeface,
                 std::vector<GlyphInfo> info, float font_size)
    : typeface_(typeface),
      glyph_info_(std::move(info)),
      font_size_(font_size) {}

TextRun::~TextRun() = default;

std::vector<GlyphInfo> const& TextRun::getGlyphInfo() const {
  return glyph_info_;
}

GlyphBitmapInfo TextRun::queryBitmapInfo(GlyphID glyph_id) {
  auto typeface = lockTypeface();

  if (!typeface) {
    return {};
  }

  return typeface->getGlyphBitmapInfo(glyph_id, font_size_);
}

}  // namespace skity