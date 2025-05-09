// The MIT License (MIT)

// Copyright (c) 2013-2019 Rapptz, ThePhD and contributors

// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// This file was generated with a script.
// Generated 2024-12-19 02:54:36.906782 UTC
// This header was generated with sol  (revision )
// https://github.com/ThePhD/sol2

#ifndef SOL_SINGLE_INCLUDE_FORWARD_HPP
#define SOL_SINGLE_INCLUDE_FORWARD_HPP

// beginning of sol/forward.hpp

#ifndef SOL_FORWARD_HPP
#define SOL_FORWARD_HPP

// beginning of sol/feature_test.hpp

#if (defined(__cplusplus) && __cplusplus >= 201703L) || (defined(_MSC_VER) && _MSC_VER > 1900 && ((defined(_HAS_CXX17) && _HAS_CXX17 == 1) || (defined(_MSVC_LANG) && (_MSVC_LANG > 201402L))))
#ifndef SOL_CXX17_FEATURES
#define SOL_CXX17_FEATURES 1
#endif // C++17 features macro
#endif // C++17 features check

#if defined(SOL_CXX17_FEATURES) && SOL_CXX17_FEATURES
#if defined(__cpp_noexcept_function_type) || ((defined(_MSC_VER) && _MSC_VER > 1911) && (defined(_MSVC_LANG) && ((_MSVC_LANG >= 201403L))))
#ifndef SOL_NOEXCEPT_FUNCTION_TYPE
#define SOL_NOEXCEPT_FUNCTION_TYPE 1
#endif // noexcept is part of a function's type
#endif // compiler-specific checks
#if defined(__clang__) && defined(__APPLE__)
#if defined(__has_include)
#if __has_include(<variant>)
#define SOL_STD_VARIANT 1
#endif // has include nonsense
#endif // __has_include
#else
#define SOL_STD_VARIANT 1
#endif // Clang screws up variant
#endif // C++17 only

// beginning of sol/config.hpp

#ifdef _MSC_VER
	#if defined(_DEBUG) && !defined(NDEBUG)
		#ifndef SOL_IN_DEBUG_DETECTED
			#define SOL_IN_DEBUG_DETECTED 1
		#endif
	#endif // VC++ Debug macros

	#if !defined(_CPPUNWIND)
		#if !defined(SOL_NO_EXCEPTIONS)
			#define SOL_NO_EXCEPTIONS 1
		#endif
	#endif // Automatic Exceptions

	#if !defined(_CPPRTTI)
		#if !defined(SOL_NO_RTTI)
			#define SOL_NO_RTTI 1
		#endif
	#endif // Automatic RTTI
#elif defined(__GNUC__) || defined(__clang__)

	#if !defined(NDEBUG) && !defined(__OPTIMIZE__)
		#if !defined(SOL_IN_DEBUG_DETECTED)
			#define SOL_IN_DEBUG_DETECTED 1
		#endif
	#endif // Not Debug && g++ optimizer flag

	#if !defined(__EXCEPTIONS)
		#if !defined(SOL_NO_EXCEPTIONS)
			#define SOL_NO_EXCEPTIONS 1
		#endif
	#endif // No Exceptions

	#if !defined(__GXX_RTTI)
		#if !defined(SOL_NO_RTTI)
			#define SOL_NO_RTTI 1
		#endif
	#endif // No RTTI

#endif // vc++ || clang++/g++

#if defined(SOL_CHECK_ARGUMENTS) && SOL_CHECK_ARGUMENTS != 0
	#if defined(SOL_ALL_SAFETIES_ON)
		#define SOL_ALL_SAFETIES_ON 1
	#endif // turn all the safeties on
#endif // Compatibility Define for Safety

#if defined(SOL_ALL_SAFETIES_ON) && SOL_ALL_SAFETIES_ON != 0

	// Checks low-level getter function
	// (and thusly, affects nearly entire framework)
	#if !defined(SOL_SAFE_GETTER)
		#define SOL_SAFE_GETTER 1
	#endif

	// Checks access on usertype functions
	// local my_obj = my_type.new()
	// my_obj.my_member_function()
	// -- bad syntax and crash
	#if !defined(SOL_SAFE_USERTYPE)
		#define SOL_SAFE_USERTYPE 1
	#endif

	// Checks sol::reference derived boundaries
	// sol::function ref(L, 1);
	// sol::userdata sref(L, 2);
	#if !defined(SOL_SAFE_REFERENCES)
		#define SOL_SAFE_REFERENCES 1
	#endif

	// Changes all typedefs of sol::function to point to the 
	// protected_function version, instead of unsafe_function
	#if !defined(SOL_SAFE_FUNCTION)
		#define SOL_SAFE_FUNCTION 1
	#endif

	// Checks function parameters and
	// returns upon call into/from Lua
	// local a = 1
	// local b = "woof"
	// my_c_function(a, b)
	#if !defined(SOL_SAFE_FUNCTION_CALLS)
		#define SOL_SAFE_FUNCTION_CALLS 1
	#endif

	// Checks conversions
	// int v = lua["bark"];
	// int v2 = my_sol_function();
	#if !defined(SOL_SAFE_PROXIES)
		#define SOL_SAFE_PROXIES 1
	#endif

	// Check overflowing number conversions
	// for things like 64 bit integers that don't fit in a typical lua_Number
	// for Lua 5.1 and 5.2
	#if !defined(SOL_SAFE_NUMERICS)
		#define SOL_SAFE_NUMERICS 1
	#endif

	// Turn off Number Precision Checks
	// if this is defined, we do not do range 
	// checks on integers / unsigned integers that might
	// be bigger than what Lua can represent
	#if !defined(SOL_NO_CHECK_NUMBER_PRECISION)
	// off by default
		#define SOL_NO_CHECK_NUMBER_PRECISION 0
	#endif

	// Print any exceptions / errors that occur
	// in debug mode to the default error stream / console
	#if !defined(SOL_SAFE_STACK_CHECK)
		#define SOL_SAFE_STACK_CHECK 1
	#endif

#endif // Turn on Safety for all if top-level macro is defined

#if defined(SOL_IN_DEBUG_DETECTED) && SOL_IN_DEBUG_DETECTED != 0

	#if !defined(SOL_SAFE_REFERENCES)
	// Ensure that references are forcefully type-checked upon construction
		#define SOL_SAFE_REFERENCES 1
	#endif

	// Safe usertypes checks for errors such as
	// obj = my_type.new()
	// obj.f() -- note the '.' instead of ':'
	// usertypes should be safe no matter what
	#if !defined(SOL_SAFE_USERTYPE)
		#define SOL_SAFE_USERTYPE 1
	#endif

	#if !defined(SOL_SAFE_FUNCTION_CALLS)
	// Function calls from Lua should be automatically safe in debug mode
		#define SOL_SAFE_FUNCTION_CALLS 1
	#endif

	// Print any exceptions / errors that occur
	// in debug mode to the default error stream / console
	#if !defined(SOL_PRINT_ERRORS)
		#define SOL_PRINT_ERRORS 1
	#endif

	// Print any exceptions / errors that occur
	// in debug mode to the default error stream / console
	#if !defined(SOL_SAFE_STACK_CHECK)
		#define SOL_SAFE_STACK_CHECK 1
	#endif

#endif // DEBUG: Turn on all debug safety features for VC++ / g++ / clang++ and similar

#if !defined(SOL_PRINT_ERRORS)
#define SOL_PRINT_ERRORS 0
#endif

#if !defined(SOL_DEFAULT_PASS_ON_ERROR)
#define SOL_DEFAULT_PASS_ON_ERROR 0
#endif

#if !defined(SOL_ENABLE_INTEROP)
#define SOL_ENABLE_INTEROP 0
#endif

#if defined(__MAC_OS_X_VERSION_MAX_ALLOWED) || defined(__OBJC__) || defined(nil)
#if !defined(SOL_NO_NIL)
#define SOL_NO_NIL 1
#endif
#endif // avoiding nil defines / keywords

#if defined(SOL_USE_BOOST) && SOL_USE_BOOST
#if !defined(SOL_UNORDERED_MAP_COMPATIBLE_HASH)
#define SOL_UNORDERED_MAP_COMPATIBLE_HASH 1
#endif // SOL_UNORDERED_MAP_COMPATIBLE_HASH
#endif 

#ifndef SOL_STACK_STRING_OPTIMIZATION_SIZE
#define SOL_STACK_STRING_OPTIMIZATION_SIZE 1024
#endif // Optimized conversion routines using a KB or so off the stack

#if !defined(SOL_SAFE_STACK_CHECK)
#define SOL_SAFE_STACK_CHECK 0
#endif // use luaL_checkstack to check stack overflow / overrun

// end of sol/config.hpp

// beginning of sol/config_setup.hpp

// end of sol/config_setup.hpp

// end of sol/feature_test.hpp

#include <utility>
#include <type_traits>
#include <string_view>

#if defined(SOL_USING_CXX_LUA) && SOL_USING_CXX_LUA
struct lua_State;
#else
extern "C" {
struct lua_State;
}
#endif // C++ Mangling for Lua vs. Not

namespace sol {

	enum class type;

	class stateless_reference;
	template <bool b>
	class basic_reference;
	using reference = basic_reference<false>;
	using main_reference = basic_reference<true>;
	class stateless_stack_reference;
	class stack_reference;

	template <typename A>
	class basic_bytecode;

	struct lua_value;

	struct proxy_base_tag;
	template <typename>
	struct proxy_base;
	template <typename, typename>
	struct table_proxy;

	template <bool, typename>
	class basic_table_core;
	template <bool b>
	using table_core = basic_table_core<b, reference>;
	template <bool b>
	using main_table_core = basic_table_core<b, main_reference>;
	template <bool b>
	using stack_table_core = basic_table_core<b, stack_reference>;
	template <typename base_type>
	using basic_table = basic_table_core<false, base_type>;
	using table = table_core<false>;
	using global_table = table_core<true>;
	using main_table = main_table_core<false>;
	using main_global_table = main_table_core<true>;
	using stack_table = stack_table_core<false>;
	using stack_global_table = stack_table_core<true>;

	template <typename>
	struct basic_lua_table;
	using lua_table = basic_lua_table<reference>;
	using stack_lua_table = basic_lua_table<stack_reference>;

	template <typename T, typename base_type>
	class basic_usertype;
	template <typename T>
	using usertype = basic_usertype<T, reference>;
	template <typename T>
	using stack_usertype = basic_usertype<T, stack_reference>;

	template <typename base_type>
	class basic_metatable;
	using metatable = basic_metatable<reference>;
	using stack_metatable = basic_metatable<stack_reference>;

	template <typename base_t>
	struct basic_environment;
	using environment = basic_environment<reference>;
	using main_environment = basic_environment<main_reference>;
	using stack_environment = basic_environment<stack_reference>;

	template <typename T, bool>
	class basic_function;
	template <typename T, bool, typename H>
	class basic_protected_function;
	using unsafe_function = basic_function<reference, false>;
	using safe_function = basic_protected_function<reference, false, reference>;
	using main_unsafe_function = basic_function<main_reference, false>;
	using main_safe_function = basic_protected_function<main_reference, false, reference>;
	using stack_unsafe_function = basic_function<stack_reference, false>;
	using stack_safe_function = basic_protected_function<stack_reference, false, reference>;
	using stack_aligned_unsafe_function = basic_function<stack_reference, true>;
	using stack_aligned_safe_function = basic_protected_function<stack_reference, true, reference>;
	using protected_function = safe_function;
	using main_protected_function = main_safe_function;
	using stack_protected_function = stack_safe_function;
	using stack_aligned_protected_function = stack_aligned_safe_function;
#if defined(SOL_SAFE_FUNCTION) && SOL_SAFE_FUNCTION
	using function = protected_function;
	using main_function = main_protected_function;
	using stack_function = stack_protected_function;
	using stack_aligned_function = stack_aligned_safe_function;
#else
	using function = unsafe_function;
	using main_function = main_unsafe_function;
	using stack_function = stack_unsafe_function;
	using stack_aligned_function = stack_aligned_unsafe_function;
#endif
	using stack_aligned_stack_handler_function = basic_protected_function<stack_reference, true, stack_reference>;

	struct unsafe_function_result;
	struct protected_function_result;
	using safe_function_result = protected_function_result;
#if defined(SOL_SAFE_FUNCTION) && SOL_SAFE_FUNCTION
	using function_result = safe_function_result;
#else
	using function_result = unsafe_function_result;
#endif

	template <typename base_t>
	class basic_object_base;
	template <typename base_t>
	class basic_object;
	template <typename base_t>
	class basic_userdata;
	template <typename base_t>
	class basic_lightuserdata;
	template <typename base_t>
	class basic_coroutine;
	template <typename base_t>
	class basic_thread;

	using object = basic_object<reference>;
	using userdata = basic_userdata<reference>;
	using lightuserdata = basic_lightuserdata<reference>;
	using thread = basic_thread<reference>;
	using coroutine = basic_coroutine<reference>;
	using main_object = basic_object<main_reference>;
	using main_userdata = basic_userdata<main_reference>;
	using main_lightuserdata = basic_lightuserdata<main_reference>;
	using main_coroutine = basic_coroutine<main_reference>;
	using stack_object = basic_object<stack_reference>;
	using stack_userdata = basic_userdata<stack_reference>;
	using stack_lightuserdata = basic_lightuserdata<stack_reference>;
	using stack_thread = basic_thread<stack_reference>;
	using stack_coroutine = basic_coroutine<stack_reference>;

	struct stack_proxy_base;
	struct stack_proxy;
	struct variadic_args;
	struct variadic_results;
	struct stack_count;
	struct this_state;
	struct this_main_state;
	struct this_environment;

	class state_view;
	class state;

	template <typename T>
	struct as_table_t;
	template <typename T>
	struct as_container_t;
	template <typename T>
	struct nested;
	template <typename T>
	struct light;
	template <typename T>
	struct user;
	template <typename T>
	struct as_args_t;
	template <typename T>
	struct protect_t;
	template <typename F, typename... Policies>
	struct policy_wrapper;

	template <typename T>
	struct usertype_traits;
	template <typename T>
	struct unique_usertype_traits;

	template <typename... Args>
	struct types {
		typedef std::make_index_sequence<sizeof...(Args)> indices;
		static constexpr std::size_t size() {
			return sizeof...(Args);
		}
	};

	template <typename T>
	struct derive : std::false_type {
		typedef types<> type;
	};

	template <typename T>
	struct base : std::false_type {
		typedef types<> type;
	};

	template <typename T>
	struct weak_derive {
		static bool value;
	};

	template <typename T>
	bool weak_derive<T>::value = false;

	namespace stack {
		struct record;
	}

#if !defined(SOL_USE_BOOST) || (SOL_USE_BOOST == 0)
	template <class T>
	class optional;

	template <class T>
	class optional<T&>;
#endif

	using check_handler_type = int(lua_State*, int, type, type, const char*);

} // namespace sol

#define SOL_BASE_CLASSES(T, ...)                       \
	namespace sol {                                   \
		template <>                                  \
		struct base<T> : std::true_type {            \
			typedef ::sol::types<__VA_ARGS__> type; \
		};                                           \
	}                                                 \
	void a_sol3_detail_function_decl_please_no_collide()
#define SOL_DERIVED_CLASSES(T, ...)                    \
	namespace sol {                                   \
		template <>                                  \
		struct derive<T> : std::true_type {          \
			typedef ::sol::types<__VA_ARGS__> type; \
		};                                           \
	}                                                 \
	void a_sol3_detail_function_decl_please_no_collide()

#endif // SOL_FORWARD_HPP
// end of sol/forward.hpp

#endif // SOL_SINGLE_INCLUDE_FORWARD_HPP
