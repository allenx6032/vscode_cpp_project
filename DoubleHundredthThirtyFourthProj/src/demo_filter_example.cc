#include <skity.hpp>

#include "glad.h"

void draw_filter(skity::Canvas* canvas) {
  auto filter = skity::MaskFilter::MakeBlur(skity::BlurStyle::kNormal, 10.f);

  skity::Paint paint;
  paint.setStyle(skity::Paint::kStroke_Style);
  paint.setStrokeWidth(18);
  paint.setColor(0xff4285F4);
  paint.setStrokeCap(skity::Paint::kRound_Cap);

  paint.setMaskFilter(filter);

  skity::Path path;
  path.moveTo(10, 10);
  path.quadTo(256, 64, 128, 128);
  path.quadTo(10, 192, 250, 250);

  {
    skity::Vec4 colors[] = {
        skity::Vec4{0.f, 1.f, 1.f, 1.f},
        skity::Vec4{0.f, 0.f, 1.f, 1.f},
        skity::Vec4{1.f, 0.f, 0.f, 1.f},
    };

    std::vector<skity::Point> pts = {
        skity::Point{10.f, 10.f, 0.f, 1.f},
        skity::Point{250.f, 250.f, 0.f, 1.f},
    };

    auto lgs = skity::Shader::MakeLinear(pts.data(), colors, nullptr, 3);
    paint.setShader(lgs);
  }

  canvas->drawPath(path, paint);

  paint.setShader(nullptr);

  paint.setStyle(skity::Paint::kFill_Style);
  paint.setMaskFilter(
      skity::MaskFilter::MakeBlur(skity::BlurStyle::kNormal, 20.f));

  canvas->save();
  canvas->translate(50.f, 0.f);
  canvas->drawCircle(300, 100, 50, paint);
  canvas->restore();

  canvas->save();
  canvas->translate(200.f, 0.f);

  paint.setMaskFilter(
      skity::MaskFilter::MakeBlur(skity::BlurStyle::kOuter, 20.f));
  canvas->drawCircle(300.f, 100.f, 50.f, paint);

  canvas->restore();

  canvas->save();

  canvas->translate(350, 0.f);

  paint.setMaskFilter(
      skity::MaskFilter::MakeBlur(skity::BlurStyle::kInner, 20.f));

  canvas->drawCircle(300.f, 100.f, 50.f, paint);
  canvas->restore();

  skity::Path path2;
  path2.moveTo(199, 34);
  path2.lineTo(253, 143);
  path2.lineTo(374, 160);
  path2.lineTo(287, 244);
  path2.lineTo(307, 365);
  path2.lineTo(199, 309);
  path2.lineTo(97, 365);
  path2.lineTo(112, 245);
  path2.lineTo(26, 161);
  path2.lineTo(146, 143);
  path2.close();

  paint.setMaskFilter(
      skity::MaskFilter::MakeBlur(skity::BlurStyle::kNormal, 10.f));

  canvas->save();
  canvas->translate(300, 200);
  canvas->drawPath(path2, paint);
  canvas->restore();

  canvas->save();
  canvas->translate(0, 300);
  paint.setTextSize(40.f);
  paint.setMaskFilter(
      skity::MaskFilter::MakeBlur(skity::BlurStyle::kNormal, 5.f));
  paint.setColor(skity::Color_RED);

  paint.setStyle(skity::Paint::kFill_Style);

  canvas->drawSimpleText2("Hello World!", 12, 32, paint);
  paint.setMaskFilter(nullptr);
  paint.setColor(0xff4285F4);
  canvas->drawSimpleText2("Hello World!", 10, 30, paint);
  canvas->restore();
}