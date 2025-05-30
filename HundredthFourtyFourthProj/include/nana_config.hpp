/*
 *	Nana Configuration
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/config.hpp
 */

#ifndef NANA_CONFIG_HPP
#define NANA_CONFIG_HPP

//Select platform automatically
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
//Windows:
	#define NANA_WINDOWS	1
	#define PLATFORM_SPEC_HPP <nana_platform_spec.hpp>
	#define GUI_BEDROCK_HPP <nana_bedrock.hpp>

	//Test if it is MINGW
	#if defined(__MINGW32__)
		#define NANA_MINGW
	#endif
#elif (defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)) && !defined(_CRAYC)
//Linux:
	#define NANA_LINUX	1
	#define NANA_X11	1
	#define PLATFORM_SPEC_HPP <nana_x11_platform_spec.hpp>
	#define GUI_BEDROCK_HPP <nana_bedrock.hpp>
#endif

//Here defines some flags that tell Nana what features will be supported.

#define NANA_UNICODE

#if defined(NANA_UNICODE) && defined(NANA_WINDOWS)
	#ifndef _UNICODE
		#define _UNICODE
	#endif

	#ifndef UNICODE
		#define UNICODE
	#endif
#endif

//Support for PNG
//	Comment it to disable the feature of support for PNG.
//#define NANA_ENABLE_PNG 1
#if defined(NANA_ENABLE_PNG)
	//Comment it to use libpng from operating system.
	#define NANA_LIBPNG 1
#endif


//If Boost C++ Library is installed in current system. Nana recommends
//enabling Boost instead of classes that provided by Nana. Only for C++03

//#define NANA_USE_BOOST_MUTEX_CONDITION_VARIABLE	1
//#define NANA_USE_BOOST_SMART_PTR	1

#endif
