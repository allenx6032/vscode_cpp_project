/*
    Copyright 2016 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_GENERAL_OUT_P_H
#define F_INC_GENERAL_OUT_P_H

#include "faur_system_includes.h"

extern void f_out_info(const char* Format, ...) F__ATTRIBUTE_FORMAT(1);
extern void f_out_warning(const char* Format, ...) F__ATTRIBUTE_FORMAT(1);
extern void f_out_error(const char* Format, ...) F__ATTRIBUTE_FORMAT(1);
extern void f_out_errorv(const char* Format, va_list Args);

#endif // F_INC_GENERAL_OUT_P_H
