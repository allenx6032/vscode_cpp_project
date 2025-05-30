/*
    Copyright 2010 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_GRAPHICS_SPRITELAYERS_V_H
#define F_INC_GRAPHICS_SPRITELAYERS_V_H

#include "faur_spritelayers.p.h"

typedef struct FSpriteLayersLayer FSpriteLayersLayer;

#include "faur_listintr.v.h"

struct FSpriteLayersLayer {
    FListIntrNode listNode;
    FSprite* sprite;
    FColorBlend blend;
    int r, g, b, a;
};

#endif // F_INC_GRAPHICS_SPRITELAYERS_V_H
