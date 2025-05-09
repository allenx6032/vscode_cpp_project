/*
The MIT License

Copyright (c) 2009 - 2014 Liam Devine

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

/**
	\file oolua_dsl.h
	\brief Header which provides only what is needed for a class to be proxied using
	the DSL.
*/
#ifndef OOLUA_DSL_H_
#	define OOLUA_DSL_H_

#	include "oolua_dsl_va_args.h"
#	include "oolua_proxy_class.h"
#	include "oolua_proxy_constructor.h"
#	include "oolua_proxy_member_function.h"
#	include "oolua_proxy_none_member_function.h"
#	include "oolua_proxy_public_member.h"
#	include "oolua_proxy_tags.h"

#	include "oolua_default_trait_caller.h"

#	include "oolua_stack_fwd.h"
#	include "oolua_traits.h"

#endif //OOLUA_DSL_H_
