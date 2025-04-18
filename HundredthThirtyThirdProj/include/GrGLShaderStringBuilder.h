/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef GrGLShaderStringBuilder_DEFINED
#define GrGLShaderStringBuilder_DEFINED

#include "GrAllocator.h"
#include "GrGpu.h"
#include "GrGLContext.h"
#include "SkTypes.h"

GrGLuint GrGLCompileAndAttachShader(const GrGLContext& glCtx,
                                    GrGLuint programId,
                                    GrGLenum type,
                                    const char** strings,
                                    int* lengths,
                                    int count,
                                    GrGpu::Stats*);

#endif
