/*
    Copyright 2019 Alex Margarit <alex@alxm.org>
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

#ifndef F_INC_PLATFORM_INPUT_ODROID_GO_INPUT_V_H
#define F_INC_PLATFORM_INPUT_ODROID_GO_INPUT_V_H

#include "faur_odroid_go_input.p.h"

#include "faur_platform.v.h"

extern void f_platform_odroid_go_input__init(void);

extern FCallApi_InputPoll f_platform_api_odroidgo__inputPoll;

extern FCallApi_InputButtonGet f_platform_api_odroidgo__inputButtonGet;
extern FCallApi_InputButtonPressGet f_platform_api_odroidgo__inputButtonPressGet;

#endif // F_INC_PLATFORM_INPUT_ODROID_GO_INPUT_V_H
