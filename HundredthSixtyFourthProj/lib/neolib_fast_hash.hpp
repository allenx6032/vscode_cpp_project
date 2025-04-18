// fast_hash.hpp
/*
 *  Copyright (c) 2018 Leigh Johnston.
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

namespace neolib
{
	namespace detail
	{
		template <typename T>
		inline T fast_hash(const void* aInput, std::size_t aLength);

		// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function

		template <>
		inline uint32_t fast_hash<uint32_t>(const void* aInput, std::size_t aLength)
		{
			uint32_t hash = 2166136261u;
			const uint8_t* octet = static_cast<const uint8_t*>(aInput);
			auto endOctet = octet + aLength;
			while (octet != endOctet)
			{
				hash = hash ^ *octet++;
				hash = hash * 16777619u;
			}
			return hash;
		}

		template <>
		inline uint64_t fast_hash<uint64_t>(const void* aInput, std::size_t aLength)
		{
			uint64_t hash = 14695981039346656037ull;
			const uint8_t* octet = static_cast<const uint8_t*>(aInput);
			auto endOctet = octet + aLength;
			while (octet != endOctet)
			{
				hash = hash ^ *octet++;
				hash = hash * 1099511628211ull;
			}
			return hash;
		}
	}

	template <typename T>
	inline T fast_hash(const void* aInput, std::size_t aLength)
	{
		return detail::fast_hash<T>(aInput, aLength);
	}

	uint32_t fast_hash(const void* aInput, std::size_t aLength)
	{
		return fast_hash<uint32_t>(aInput, aLength);
	}
}
