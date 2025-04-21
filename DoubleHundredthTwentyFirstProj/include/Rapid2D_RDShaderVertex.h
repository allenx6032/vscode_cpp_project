// Copyright 2016 KeNan Liu
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __RDShaderVertex_H__
#define __RDShaderVertex_H__

#include "Rapid2D_RDMacros.h"

static const char *_vertexShaderVSH = RDToString(
attribute vec4 Position;
attribute vec4 SourceColor;
uniform mat4 Projection;
uniform mat4 Modelview;
uniform float PointSize;

varying vec4 DestinationColor;// out param to fsh

void main(void) {
    DestinationColor = SourceColor;
    gl_Position = Projection * Modelview * Position;
    gl_PointSize = PointSize;
}
);

static const char *_vertexShaderFSH = RDToString(
\n#ifdef GL_ES\n
// define float for OpenGL ES
precision lowp float;
\n#else\n
// lowp is not implementation in OpenGL, but works in OpenGL ES
\n#define lowp\n
\n#endif\n

varying lowp vec4 DestinationColor;

void main(void) {
    gl_FragColor = DestinationColor;
}
);

#endif // __RDShaderVertex_H__
