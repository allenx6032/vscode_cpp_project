/*
 * Copyright 2012 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "skia_PathOpsExtendedTest.h"
#include "skia_PathOpsThreadedCommon.h"
#include "SkThreadPool.h"

PathOpsThreadedTestRunner::~PathOpsThreadedTestRunner() {
    for (int index = 0; index < fRunnables.count(); index++) {
        SkDELETE(fRunnables[index]);
    }
}

void PathOpsThreadedTestRunner::render() {
    SkThreadPool pool(fNumThreads);
    for (int index = 0; index < fRunnables.count(); ++ index) {
        pool.add(fRunnables[index]);
    }
}
