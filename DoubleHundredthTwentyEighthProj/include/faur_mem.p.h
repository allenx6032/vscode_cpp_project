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

#ifndef F_INC_MEMORY_MEM_P_H
#define F_INC_MEMORY_MEM_P_H

#include "faur_system_includes.h"

extern void* f_mem_malloc(size_t Size);
extern void* f_mem_mallocz(size_t Size);
extern void* f_mem_malloca(size_t Size, unsigned AlignExp);
extern void* f_mem_dup(const void* Buffer, size_t Size);
extern void f_mem_free(void* Buffer);
extern void f_mem_freea(void* Buffer);

#endif // F_INC_MEMORY_MEM_P_H
