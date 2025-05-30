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

#include <stdio.h>
#include <stdarg.h>

#include "Rapid2D_RDLog.h"
#include "Rapid2D_RDPortingPrint.h"

void RDLog(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    RDPortingPrint_log(format, args);
    va_end(args);
}
