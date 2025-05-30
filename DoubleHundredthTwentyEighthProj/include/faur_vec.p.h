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

#ifndef F_INC_MATH_VEC_P_H
#define F_INC_MATH_VEC_P_H

#include "faur_system_includes.h"

typedef struct FVecFix FVecFix;
typedef struct FVecInt FVecInt;

#include "faur_fix.p.h"

struct FVecFix {
    FFix x, y;
};

struct FVecInt {
    int x, y;
};

extern FVecFix f_vecfix_rotateCounter(FVecFix Vec, unsigned Angle);
extern FVecFix f_vecfix_rotateClockwise(FVecFix Vec, unsigned Angle);

static inline FVecInt f_vecfix_toInt(const FVecFix Fix)
{
    FVecInt v = {f_fix_toInt(Fix.x), f_fix_toInt(Fix.y)};

    return v;
}

static inline FVecFix f_vecint_toFix(const FVecInt Int)
{
    FVecFix v = {f_fix_fromInt(Int.x), f_fix_fromInt(Int.y)};

    return v;
}

static inline bool f_vecfix_equal(FVecFix A, FVecFix B)
{
    return A.x == B.x && A.y == B.y;
}

static inline bool f_vecint_equal(FVecInt A, FVecInt B)
{
    return A.x == B.x && A.y == B.y;
}

#endif // F_INC_MATH_VEC_P_H
