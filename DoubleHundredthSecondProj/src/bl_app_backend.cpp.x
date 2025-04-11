// This file is part of Blend2D project <https://blend2d.com>
//
// See LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#include "bl_app.h"
#include "bl_app_backend.h"
#include "bl_app_shape_data.h"

#include <chrono>

namespace blbench {

// blbench::Backend - Construction & Destruction
// =================================================

Backend::Backend()
  : _name(),
    _params(),
    _duration(0),
    _rndCoord(0x19AE0DDAE3FA7391ull),
    _rndColor(0x94BD7A499AD10011ull),
    _rndExtra(0x1ABD9CC9CAF0F123ull),
    _rndSpriteId(0) {}
Backend::~Backend() {}

// blbench::Backend - Run
// ==========================

static void BenchModule_onDoShapeHelper(Backend* mod, RenderOp op, ShapeKind shapeKind) {
  ShapeData shapeData;
  getShapeData(shapeData, shapeKind);
  mod->renderShape(op, shapeData);
}

void Backend::run(const BenchApp& app, const BenchParams& params) {
  _params = params;

  _rndCoord.rewind();
  _rndColor.rewind();
  _rndExtra.rewind();
  _rndSpriteId = 0;

  // Initialize the sprites.
  for (uint32_t i = 0; i < kBenchNumSprites; i++) {
    _sprites[i] = app.getScaledSprite(i, params.shapeSize);
  }

  beforeRun();
  auto start = std::chrono::high_resolution_clock::now();

  switch (_params.testKind) {
    case TestKind::kFillAlignedRect   : renderRectA(RenderOp::kFillNonZero); break;
    case TestKind::kFillSmoothRect    : renderRectF(RenderOp::kFillNonZero); break;
    case TestKind::kFillRotatedRect   : renderRectRotated(RenderOp::kFillNonZero); break;
    case TestKind::kFillSmoothRound   : renderRoundF(RenderOp::kFillNonZero); break;
    case TestKind::kFillRotatedRound  : renderRoundRotated(RenderOp::kFillNonZero); break;
    case TestKind::kFillTriangle      : renderPolygon(RenderOp::kFillNonZero, 3); break;
    case TestKind::kFillPolygon10NZ   : renderPolygon(RenderOp::kFillNonZero, 10); break;
    case TestKind::kFillPolygon10EO   : renderPolygon(RenderOp::kFillEvenOdd, 10); break;
    case TestKind::kFillPolygon20NZ   : renderPolygon(RenderOp::kFillNonZero, 20); break;
    case TestKind::kFillPolygon20EO   : renderPolygon(RenderOp::kFillEvenOdd, 20); break;
    case TestKind::kFillPolygon40NZ   : renderPolygon(RenderOp::kFillNonZero, 40); break;
    case TestKind::kFillPolygon40EO   : renderPolygon(RenderOp::kFillEvenOdd, 40); break;
    case TestKind::kFillButterfly     : BenchModule_onDoShapeHelper(this, RenderOp::kFillNonZero, ShapeKind::kButterfly); break;
    case TestKind::kFillFish          : BenchModule_onDoShapeHelper(this, RenderOp::kFillNonZero, ShapeKind::kFish); break;
    case TestKind::kFillDragon        : BenchModule_onDoShapeHelper(this, RenderOp::kFillNonZero, ShapeKind::kDragon); break;
    case TestKind::kFillWorld         : BenchModule_onDoShapeHelper(this, RenderOp::kFillNonZero, ShapeKind::kWorld); break;

    case TestKind::kStrokeAlignedRect : renderRectA(RenderOp::kStroke); break;
    case TestKind::kStrokeSmoothRect  : renderRectF(RenderOp::kStroke); break;
    case TestKind::kStrokeRotatedRect : renderRectRotated(RenderOp::kStroke); break;
    case TestKind::kStrokeSmoothRound : renderRoundF(RenderOp::kStroke); break;
    case TestKind::kStrokeRotatedRound: renderRoundRotated(RenderOp::kStroke); break;
    case TestKind::kStrokeTriangle    : renderPolygon(RenderOp::kStroke, 3); break;
    case TestKind::kStrokePolygon10   : renderPolygon(RenderOp::kStroke, 10); break;
    case TestKind::kStrokePolygon20   : renderPolygon(RenderOp::kStroke, 20); break;
    case TestKind::kStrokePolygon40   : renderPolygon(RenderOp::kStroke, 40); break;
    case TestKind::kStrokeButterfly   : BenchModule_onDoShapeHelper(this, RenderOp::kStroke, ShapeKind::kButterfly); break;
    case TestKind::kStrokeFish        : BenchModule_onDoShapeHelper(this, RenderOp::kStroke, ShapeKind::kFish); break;
    case TestKind::kStrokeDragon      : BenchModule_onDoShapeHelper(this, RenderOp::kStroke, ShapeKind::kDragon); break;
    case TestKind::kStrokeWorld       : BenchModule_onDoShapeHelper(this, RenderOp::kStroke, ShapeKind::kWorld); break;
  }

  flush();

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  _duration = uint64_t(elapsed.count() * 1000000);

  afterRun();
}

void Backend::serializeInfo(JSONBuilder& json) const { (void)json; }

} // {blbench}
