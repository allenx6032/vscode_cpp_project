
/*
 * Copyright 2012 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkDebugGLContext.h"

const GrGLInterface* SkDebugGLContext::createGLContext() {
    return GrGLCreateDebugInterface();
};
