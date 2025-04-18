/**
 * Copyright (c) 2006-2015 LOVE Development Team
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 **/

// LOVE
#include "love_Window.h"

namespace love
{
namespace window
{

Window *Window::singleton = nullptr;

Window::~Window()
{
	if (singleton == this)
		singleton = nullptr;
}

void Window::swapBuffers()
{
}

WindowSettings::WindowSettings()
	: fullscreen(false)
	, fstype(Window::FULLSCREEN_TYPE_NORMAL)
	, vsync(true)
	, msaa(0)
	, resizable(false)
	, minwidth(1)
	, minheight(1)
	, borderless(false)
	, centered(true)
	, display(0)
	, highdpi(false)
	, sRGB(false)
	, refreshrate(0.0)
	, useposition(false)
	, x(0)
	, y(0)
{
}

bool Window::getConstant(const char *in, Window::FullscreenType &out)
{
	return fullscreenTypes.find(in, out);
}

bool Window::getConstant(Window::FullscreenType in, const char *&out)
{
	return fullscreenTypes.find(in, out);
}

bool Window::getConstant(const char *in, Window::Setting &out)
{
	return settings.find(in, out);
}

bool Window::getConstant(Window::Setting in, const char *&out)
{
	return settings.find(in, out);
}

bool Window::getConstant(const char *in, MessageBoxType &out)
{
	return messageBoxTypes.find(in, out);
}

bool Window::getConstant(MessageBoxType in, const char *&out)
{
	return messageBoxTypes.find(in, out);
}

StringMap<Window::Setting, Window::SETTING_MAX_ENUM>::Entry Window::settingEntries[] =
{
	{"fullscreen", SETTING_FULLSCREEN},
	{"fullscreentype", SETTING_FULLSCREEN_TYPE},
	{"vsync", SETTING_VSYNC},
	{"fsaa", SETTING_FSAA},
	{"msaa", SETTING_MSAA},
	{"resizable", SETTING_RESIZABLE},
	{"minwidth", SETTING_MIN_WIDTH},
	{"minheight", SETTING_MIN_HEIGHT},
	{"borderless", SETTING_BORDERLESS},
	{"centered", SETTING_CENTERED},
	{"display", SETTING_DISPLAY},
	{"highdpi", SETTING_HIGHDPI},
	{"srgb", SETTING_SRGB},
	{"refreshrate", SETTING_REFRESHRATE},
	{"x", SETTING_X},
	{"y", SETTING_Y},
};

StringMap<Window::Setting, Window::SETTING_MAX_ENUM> Window::settings(Window::settingEntries, sizeof(Window::settingEntries));

StringMap<Window::FullscreenType, Window::FULLSCREEN_TYPE_MAX_ENUM>::Entry Window::fullscreenTypeEntries[] =
{
	{"normal", Window::FULLSCREEN_TYPE_NORMAL},
	{"desktop", Window::FULLSCREEN_TYPE_DESKTOP},
};

StringMap<Window::FullscreenType, Window::FULLSCREEN_TYPE_MAX_ENUM> Window::fullscreenTypes(Window::fullscreenTypeEntries, sizeof(Window::fullscreenTypeEntries));

StringMap<Window::MessageBoxType, Window::MESSAGEBOX_MAX_ENUM>::Entry Window::messageBoxTypeEntries[] =
{
	{"error", Window::MESSAGEBOX_ERROR},
	{"warning", Window::MESSAGEBOX_WARNING},
	{"info", Window::MESSAGEBOX_INFO},
};

StringMap<Window::MessageBoxType, Window::MESSAGEBOX_MAX_ENUM> Window::messageBoxTypes(Window::messageBoxTypeEntries, sizeof(Window::messageBoxTypeEntries));

} // window
} // love
