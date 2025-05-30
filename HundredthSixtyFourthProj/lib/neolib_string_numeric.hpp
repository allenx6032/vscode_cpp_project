// string_numeric.hpp
/*
 *  Copyright (c) 2007 Leigh Johnston.
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 *     * Neither the name of Leigh Johnston nor the names of any
 *       other contributors to this software may be used to endorse or
 *       promote products derived from this software without specific prior
 *       written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "neolib.hpp"
#include <string>
#include <string_view.hpp>
// #include <boost/spirit/include/qi_parse.hpp>
// #include <boost/spirit/include/qi_numeric.hpp>

namespace neolib 
{
	template <typename CharT, typename Traits>
	inline int32_t string_to_int32(const std::basic_string_view<CharT, Traits>& aStringView)
	{
		namespace qi = boost::spirit::qi;
		int32_t result = 0;
		qi::parse(aStringView.begin(), aStringView.end(), qi::int_, result);
		return result;
	}

	template <typename CharT, typename Traits, typename Alloc>
	inline int32_t string_to_int32(const std::basic_string<CharT, Traits, Alloc>& aString, int aBase = 10)
	{
		if (aBase == 10) // todo: does spirit provide other bases?
			string_to_int32(static_cast<std::basic_string_view<CharT, Traits>>(aString));
		return strtol(aString.c_str(), nullptr, aBase);
	}
		
	template <typename CharT, typename Traits>
	inline int64_t string_to_int64(const std::basic_string_view<CharT, Traits>& aStringView)
	{
		namespace qi = boost::spirit::qi;
		int64_t result = 0ll;
		qi::parse(aStringView.begin(), aStringView.end(), qi::long_long, result);
		return result;
	}

	template <typename CharT, typename Traits, typename Alloc>
	inline int64_t string_to_int64(const std::basic_string<CharT, Traits, Alloc>& aString, int aBase = 10)
	{
		if (aBase == 10) // todo: does spirit provide other bases?
			return string_to_int64(static_cast<std::basic_string_view<CharT, Traits>>(aString));
		return strtoll(aString.c_str(), nullptr, aBase);
	}

	template <typename CharT, typename Traits>
	inline uint32_t string_to_uint32(const std::basic_string_view<CharT, Traits>& aStringView)
	{
		namespace qi = boost::spirit::qi;
		uint32_t result = 0u;
		qi::parse(aStringView.begin(), aStringView.end(), qi::uint_, result);
		return result;
	}

	template <typename CharT, typename Traits, typename Alloc>
	inline uint32_t string_to_uint32(const std::basic_string<CharT, Traits, Alloc>& aString, int aBase = 10)
	{
		if (aBase == 10)
			return string_to_uint32(static_cast<std::basic_string_view<CharT, Traits>>(aString));
		return strtoul(aString.c_str(), 0, aBase);
	}

	template <typename CharT, typename Traits>
	inline uint64_t string_to_uint64(const std::basic_string_view<CharT, Traits>& aStringView)
	{
		namespace qi = boost::spirit::qi;
		int64_t result = 0ll;
		qi::parse(aStringView.begin(), aStringView.end(), qi::long_long, result);
		return result;
	}

	template <typename CharT, typename Traits, typename Alloc>
	inline uint64_t string_to_uint64(const std::basic_string<CharT, Traits, Alloc>& aString, int aBase = 10)
	{
		if (aBase == 10)
			return string_to_uint64(static_cast<std::basic_string_view<CharT, Traits>>(aString));
		return strtoull(aString.c_str(), 0, aBase);
	}

	template <typename CharT, typename Traits>
	inline double string_to_double(const std::basic_string_view<CharT, Traits>& aStringView)
	{
		namespace qi = boost::spirit::qi;
		double result = 0.0;
		qi::parse(aStringView.begin(), aStringView.end(), qi::double_, result);
		return result;
	}

	template <typename CharT, typename Traits, typename Alloc>
	inline double string_to_double(const std::basic_string<CharT, Traits, Alloc>& aString)
	{
		return string_to_double(static_cast<std::basic_string_view<CharT, Traits>>(aString));
	}

	template <typename CharT, typename Traits = std::char_traits<CharT>, typename Alloc = std::allocator<CharT>>
	inline std::basic_string<CharT, Traits, Alloc> int32_to_string(int32_t aint32, int aBase = 10, std::size_t aWidth = 0, CharT aFill = '0')
	{
		std::basic_stringstream<CharT, Traits, Alloc> string;
		if (aBase == 16)
			string << std::hex << std::uppercase;
		if (aWidth != 0)
		{
			string.width(aWidth);
			string.fill(aFill);
		}
		string << aint32;
		return string.str();
	}

	template <typename CharT, typename Traits = std::char_traits<CharT>, typename Alloc = std::allocator<CharT>>
	inline std::basic_string<CharT, Traits, Alloc> int64_to_string(int64_t aint32, int aBase = 10, std::size_t aWidth = 0, CharT aFill = '0')
	{
		std::basic_stringstream<CharT, Traits, Alloc> string;
		switch (aBase)
		{
		case 8:
			string << std::oct;
			break;
		case 16:
			string << std::hex << std::uppercase;
			break;
		}
		if (aWidth != 0)
		{
			string.width(aWidth);
			string.fill(aFill);
		}
		string << aint32;
		return string.str();
	}

	template <typename CharT, typename Traits = std::char_traits<CharT>, typename Alloc = std::allocator<CharT>>
	inline std::basic_string<CharT, Traits, Alloc> uint32_to_string(uint32_t aint32, int aBase = 10, std::size_t aWidth = 0, CharT aFill = '0')
	{
		std::basic_stringstream<CharT, Traits, Alloc> string;
		if (aBase == 16)
			string << std::hex << std::uppercase;
		if (aWidth != 0)
		{
			string.width(aWidth);
			string.fill(aFill);
		}
		string << aint32;
		return string.str();
	}

	template <typename CharT, typename Traits = std::char_traits<CharT>, typename Alloc = std::allocator<CharT>>
	inline std::basic_string<CharT, Traits, Alloc> uint64_to_string(uint64_t aint32, int aBase = 10, std::size_t aWidth = 0, CharT aFill = '0')
	{
		std::basic_stringstream<CharT, Traits, Alloc> string;
		if (aBase == 16)
			string << std::hex << std::uppercase;
		if (aWidth != 0)
		{
			string.width(aWidth);
			string.fill(aFill);
		}
		string << aint32;
		return string.str();
	}

	template <typename CharT, typename Traits = std::char_traits<CharT>, typename Alloc = std::allocator<CharT>>
	inline std::basic_string<CharT, Traits, Alloc> double_to_string(double aDouble, std::size_t aPrecision = 0, bool aFixed = true, std::size_t aWidth = 0, CharT aFill = '0')
	{
		std::basic_stringstream<CharT, Traits, Alloc> string;
		if (aPrecision != 0)
			string.precision(aPrecision);
		if (aFixed)
			string << std::fixed;
		else
			string << std::scientific;
		if (aWidth != 0)
		{
			string.width(aWidth);
			string.fill(aFill);
		}
		string << aDouble;
		return string.str();
	}

}
