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

#include <windows.h>
#include "Rapid2D_RDPortingPrint.h"
#include "Rapid2D_RDWin32Char.h"

#define RD_MAX_LOG_LENGTH (1024)

void RDPortingPrint_log(const char *format, va_list args)
{
	char szBuf[RD_MAX_LOG_LENGTH];

	vsnprintf_s(szBuf, RD_MAX_LOG_LENGTH, RD_MAX_LOG_LENGTH, format, args);
	printf(szBuf);

#ifdef RD_DEBUG
	// UTF8 -> Wchar
	WCHAR *wBuff = UTF8_To_Wchar(szBuf);
	if (wBuff) {
		OutputDebugStringW(wBuff);
		free(wBuff);
	}
#endif
}
