/*
 *	Bedrock Selector
 *	Nana C++ Library(http://www.nanapro.org)
 *	Copyright(C) 2003-2015 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://nanapro.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/detail/bedrock_selector.cpp
 *
 *  This file is used to support the Nana project of some cross-platform IDE,
 *	
 */

#include <nana_config.hpp>

#if defined(NANA_WINDOWS)
	#include "nana_mswin_bedrock.hh"
#elif defined(NANA_LINUX)
	#include "nana_posix_bedrock.hh"
#endif

