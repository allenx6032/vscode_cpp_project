/*
    Copyright 2018 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef F_INC_DATA_BLOCK_P_H
#define F_INC_DATA_BLOCK_P_H

#include "faur_system_includes.h"

typedef struct FBlock FBlock;

#include "faur_list.p.h"
#include "faur_color.p.h"
#include "faur_fix.p.h"

extern FBlock* f_block_new(const char* File);
extern void f_block_free(FBlock* Block);

extern const FList* f_block_blocksGet(const FBlock* Block);

extern const FBlock* f_block_keyGetBlock(const FBlock* Block, const char* Key);
extern const FList* f_block_keyGetBlocks(const FBlock* Block, const char* Key);
extern bool f_block_keyExists(const FBlock* Block, const char* Key);

extern int f_block_lineGetInt(const FBlock* Block, unsigned LineNumber);
extern int f_block_keyGetInt(const FBlock* Block, const char* Key);

extern unsigned f_block_lineGetIntu(const FBlock* Block, unsigned LineNumber);
extern unsigned f_block_keyGetIntu(const FBlock* Block, const char* Key);

extern FFix f_block_lineGetFix(const FBlock* Block, unsigned LineNumber);
extern FFix f_block_keyGetFix(const FBlock* Block, const char* Key);

extern FFixu f_block_lineGetAngle(const FBlock* Block, unsigned LineNumber);
extern FFixu f_block_keyGetAngle(const FBlock* Block, const char* Key);

extern FColorPixel f_block_lineGetPixel(const FBlock* Block, unsigned LineNumber);
extern FColorPixel f_block_keyGetPixel(const FBlock* Block, const char* Key);

extern const char* f_block_lineGetString(const FBlock* Block, unsigned LineNumber);
extern const char* f_block_keyGetString(const FBlock* Block, const char* Key);

extern FVecInt f_block_lineGetCoords(const FBlock* Block, unsigned LineNumber);
extern FVecInt f_block_keyGetCoords(const FBlock* Block, const char* Key);

extern FVecFix f_block_lineGetCoordsf(const FBlock* Block, unsigned LineNumber);
extern FVecFix f_block_keyGetCoordsf(const FBlock* Block, const char* Key);

extern int f_block_lineGetFmt(const FBlock* Block, unsigned LineNumber, const char* Format, ...) F__ATTRIBUTE_FORMAT(3);
extern int f_block_keyGetFmt(const FBlock* Block, const char* Key, const char* Format, ...) F__ATTRIBUTE_FORMAT(3);

extern int f_block_lineGetFmtv(const FBlock* Block, unsigned LineNumber, const char* Format, va_list Args);
extern int f_block_keyGetFmtv(const FBlock* Block, const char* Key, const char* Format, va_list Args);

#endif // F_INC_DATA_BLOCK_P_H
