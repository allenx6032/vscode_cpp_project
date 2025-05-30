/*
** LuaAutoC - Automagically use C Functions and Structs with the Lua API
** https://github.com/orangeduck/LuaAutoC
** Daniel Holden - contact@theorangeduck.com
** Licensed under BSD
*/

#ifndef lautoc_h
#define lautoc_h

#ifndef LUAA_LUAIMPLEMENTATION
#define LUAA_LUAIMPLEMENTATION
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/*
** Open / Close
*/

#define LUAA_REGISTRYPREFIX "lautoc_"

void luaA_open(lua_State* L);
void luaA_close(lua_State* L);

/*
** Types
*/

#define luaA_type(L, type) luaA_type_add(L, #type, sizeof(type))

enum {
  LUAA_INVALID_TYPE = -1
};

typedef lua_Integer luaA_Type;
typedef int (*luaA_Pushfunc)(lua_State*, luaA_Type, const void*);
typedef void (*luaA_Tofunc)(lua_State*, luaA_Type, void*, int);

luaA_Type luaA_type_add(lua_State* L, const char* type, size_t size);
luaA_Type luaA_type_find(lua_State* L, const char* type);

const char* luaA_typename(lua_State* L, luaA_Type id);
size_t luaA_typesize(lua_State* L, luaA_Type id);

/*
** Stack
*/

#define luaA_push(L, type, c_in) luaA_push_type(L, luaA_type(L, type), c_in)
#define luaA_to(L, type, c_out, index) luaA_to_type(L, luaA_type(L, type), c_out, index)

#define luaA_conversion(L, type, push_func, to_func) luaA_conversion_type(L, luaA_type(L, type), push_func, to_func);
#define luaA_conversion_push(L, type, func) luaA_conversion_push_type(L, luaA_type(L, type), func)
#define luaA_conversion_to(L, type, func) luaA_conversion_to_type(L, luaA_type(L, type), func)

#define luaA_conversion_registered(L, type) luaA_conversion_registered_type(L, luaA_type(L, type));
#define luaA_conversion_push_registered(L, type) luaA_conversion_push_registered_typ(L, luaA_type(L, type));
#define luaA_conversion_to_registered(L, type) luaA_conversion_to_registered_type(L, luaA_type(L, type));

int luaA_push_type(lua_State* L, luaA_Type type, const void* c_in);
void luaA_to_type(lua_State* L, luaA_Type type, void* c_out, int index);

void luaA_conversion_type(lua_State* L, luaA_Type type_id, luaA_Pushfunc push_func, luaA_Tofunc to_func);
void luaA_conversion_push_type(lua_State* L, luaA_Type type_id, luaA_Pushfunc func);
void luaA_conversion_to_type(lua_State* L, luaA_Type type_id, luaA_Tofunc func);

bool luaA_conversion_registered_type(lua_State* L, luaA_Type type);
bool luaA_conversion_push_registered_type(lua_State* L, luaA_Type type);
bool luaA_conversion_to_registered_type(lua_State* L, luaA_Type type);

int luaA_push_bool(lua_State* L, luaA_Type, const void* c_in);
int luaA_push_char(lua_State* L, luaA_Type, const void* c_in);
int luaA_push_signed_char(lua_State* L,luaA_Type, const void* c_in);
int luaA_push_unsigned_char(lua_State* L, luaA_Type, const void* c_in);
int luaA_push_short(lua_State* L, luaA_Type, const void* c_in);
int luaA_push_unsigned_short(lua_State* L, luaA_Type, const void* c_in);
int luaA_push_int(lua_State* L, luaA_Type, const void* c_in);
int luaA_push_unsigned_int(lua_State* L, luaA_Type, const void* c_in);
int luaA_push_long(lua_State* L, luaA_Type, const void* c_in);
int luaA_push_unsigned_long(lua_State* L, luaA_Type, const void* c_in);
int luaA_push_long_long(lua_State* L, luaA_Type, const void* c_in);
int luaA_push_unsigned_long_long(lua_State* L, luaA_Type, const void* c_in);
int luaA_push_float(lua_State* L, luaA_Type, const void* c_in);
int luaA_push_double(lua_State* L, luaA_Type, const void* c_in);
int luaA_push_long_double(lua_State* L, luaA_Type, const void* c_in);
int luaA_push_char_ptr(lua_State* L, luaA_Type, const void* c_in);
int luaA_push_const_char_ptr(lua_State* L, luaA_Type, const void* c_in);
int luaA_push_void_ptr(lua_State* L, luaA_Type, const void* c_in);
int luaA_push_void(lua_State* L, luaA_Type, const void* c_in);

void luaA_to_bool(lua_State* L, luaA_Type, void* c_out, int index);
void luaA_to_char(lua_State* L, luaA_Type, void* c_out, int index);
void luaA_to_signed_char(lua_State* L, luaA_Type,  void* c_out, int index);
void luaA_to_unsigned_char(lua_State* L, luaA_Type,  void* c_out, int index);
void luaA_to_short(lua_State* L, luaA_Type,  void* c_out, int index);
void luaA_to_unsigned_short(lua_State* L, luaA_Type,  void* c_out, int index);
void luaA_to_int(lua_State* L, luaA_Type,  void* c_out, int index);
void luaA_to_unsigned_int(lua_State* L, luaA_Type,  void* c_out, int index);
void luaA_to_long(lua_State* L, luaA_Type,  void* c_out, int index);
void luaA_to_unsigned_long(lua_State* L, luaA_Type,  void* c_out, int index);
void luaA_to_long_long(lua_State* L, luaA_Type,  void* c_out, int index);
void luaA_to_unsigned_long_long(lua_State* L, luaA_Type,  void* c_out, int index);
void luaA_to_float(lua_State* L, luaA_Type,  void* c_out, int index);
void luaA_to_double(lua_State* L, luaA_Type,  void* c_out, int index);
void luaA_to_long_double(lua_State* L, luaA_Type,  void* c_out, int index);
void luaA_to_char_ptr(lua_State* L, luaA_Type,  void* c_out, int index);
void luaA_to_const_char_ptr(lua_State* L, luaA_Type,  void* c_out, int index);
void luaA_to_void_ptr(lua_State* L, luaA_Type,  void* c_out, int index);

/*
** Structs
*/
#define LUAA_INVALID_MEMBER_NAME NULL

#define luaA_struct(L, type) luaA_struct_type(L, luaA_type(L, type))
#define luaA_struct_member(L, type, member, member_type) luaA_struct_member_type(L, luaA_type(L, type), #member, luaA_type(L, member_type), offsetof(type, member))

#define luaA_struct_push(L, type, c_in) luaA_struct_push_type(L, luaA_type(L, type), c_in)
#define luaA_struct_push_member(L, type, member, c_in) luaA_struct_push_member_offset_type(L, luaA_type(L, type), offsetof(type, member), c_in)
#define luaA_struct_push_member_name(L, type, member, c_in) luaA_struct_push_member_name_type(L, luaA_type(L, type), member, c_in)

#define luaA_struct_to(L, type, c_out, index) luaA_struct_to_type(L, luaA_type(L, type), c_out, index)
#define luaA_struct_to_member(L, type, member, c_out, index) luaA_struct_to_member_offset_type(L, luaA_type(L, type), offsetof(type, member), c_out, index)
#define luaA_struct_to_member_name(L, type, member, c_out, index) luaA_struct_to_member_name_type(L, luaA_type(L, type), member, c_out, index)

#define luaA_struct_has_member(L, type, member) luaA_struct_has_member_offset_type(L, luaA_type(L, type), offsetof(type, member))
#define luaA_struct_has_member_name(L, type, member) luaA_struct_has_member_name_type(L, luaA_type(L, type), member)

#define luaA_struct_typeof_member(L, type, member) luaA_struct_typeof_member_offset_type(L, luaA_type(L, type), offsetof(type, member))
#define luaA_struct_typeof_member_name(L, type, member) luaA_struct_typeof_member_name_type(L, luaA_type(L, type), member)

#define luaA_struct_registered(L, type) luaA_struct_registered_type(L, luaA_type(L, type))
#define luaA_struct_next_member_name(L, type, member) luaA_struct_next_member_name_type(L, luaA_type(L,type), member)

void luaA_struct_type(lua_State* L, luaA_Type type);
void luaA_struct_member_type(lua_State* L, luaA_Type type, const char* member, luaA_Type member_type, size_t offset);

int luaA_struct_push_type(lua_State* L, luaA_Type type, const void* c_in);
int luaA_struct_push_member_offset_type(lua_State* L, luaA_Type type, size_t offset, const void* c_in);
int luaA_struct_push_member_name_type(lua_State* L, luaA_Type type, const char* member, const void* c_in);

void luaA_struct_to_type(lua_State* L, luaA_Type type, void* c_out, int index);
void luaA_struct_to_member_offset_type(lua_State* L, luaA_Type type, size_t offset, void* c_out, int index);
void luaA_struct_to_member_name_type(lua_State* L, luaA_Type type, const char* member, void* c_out, int index);

bool luaA_struct_has_member_offset_type(lua_State* L, luaA_Type type, size_t offset);
bool luaA_struct_has_member_name_type(lua_State* L, luaA_Type type, const char* member);

luaA_Type luaA_struct_typeof_member_offset_type(lua_State* L, luaA_Type type, size_t offset);
luaA_Type luaA_struct_typeof_member_name_type(lua_State* L, luaA_Type type, const char* member);

bool luaA_struct_registered_type(lua_State* L, luaA_Type type);

const char* luaA_struct_next_member_name_type(lua_State* L, luaA_Type type, const char* member);

/*
** Enums
*/

#define luaA_enum(L, type) luaA_enum_type(L, luaA_type(L, type), sizeof(type))
#define luaA_enum_value(L, type, value) luaA_enum_value_type(L, luaA_type(L, type), (const type[]){value}, #value);
#define luaA_enum_value_name(L, type, value, name) luaA_enum_value_type(L, luaA_type(L, type), (const type[]){value}, name);

#define luaA_enum_push(L, type, c_in) luaA_enum_push_type(L, luaA_type(L, type), c_in)
#define luaA_enum_to(L, type, c_out, index) luaA_enum_to_type(L, luaA_type(L, type), c_out, index)

#define luaA_enum_has_value(L, type, value) luaA_enum_has_value_type(L, luaA_type(L, type), (const type[]){value})
#define luaA_enum_has_name(L, type, name) luaA_enum_has_name_type(L, luaA_type(L, type), name)

#define luaA_enum_registered(L, type) luaA_enum_registered_type(L, luaA_type(L, type))
#define luaA_enum_next_value_name(L, type, member) luaA_enum_next_value_name_type(L, luaA_type(L,type), member)

void luaA_enum_type(lua_State* L, luaA_Type type, size_t size);
void luaA_enum_value_type(lua_State *L, luaA_Type type, const void* value, const char* name);

int luaA_enum_push_type(lua_State *L, luaA_Type type, const void* c_in);
void luaA_enum_to_type(lua_State* L, luaA_Type type, void *c_out, int index);

bool luaA_enum_has_value_type(lua_State* L, luaA_Type type, const void* value);
bool luaA_enum_has_name_type(lua_State* L, luaA_Type type, const char* name);

bool luaA_enum_registered_type(lua_State *L, luaA_Type type);
const char* luaA_enum_next_value_name_type(lua_State* L, luaA_Type type, const char* member);

/*
** Functions
*/


#define LUAA_EVAL(...) __VA_ARGS__

/* Join Three Strings */
#define LUAA_JOIN2(X, Y) X ## Y
#define LUAA_JOIN3(X, Y, Z) X ## Y ## Z

/* workaround for MSVC VA_ARGS expansion */
#define LUAA_APPLY(FUNC, ARGS) LUAA_EVAL(FUNC ARGS)

/* Argument Counter */
#define LUAA_COUNT(...) LUAA_COUNT_COLLECT(_, ##__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define LUAA_COUNT_COLLECT(...) LUAA_COUNT_SHIFT(__VA_ARGS__)
#define LUAA_COUNT_SHIFT(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _N, ...) _N

/* Detect Void */
#define LUAA_VOID(X) LUAA_JOIN2(LUAA_VOID_, X)
#define LUAA_VOID_void
#define LUAA_CHECK_N(X, N, ...) N
#define LUAA_CHECK(...) LUAA_CHECK_N(__VA_ARGS__, ,)
#define LUAA_SUFFIX(X) LUAA_SUFFIX_CHECK(LUAA_VOID(X))
#define LUAA_SUFFIX_CHECK(X) LUAA_CHECK(LUAA_JOIN2(LUAA_SUFFIX_, X))
#define LUAA_SUFFIX_ ~, _void,

/* Declaration and Register Macros */
#define LUAA_DECLARE(func, ret_t, count, suffix, ...) LUAA_APPLY(LUAA_JOIN3(luaA_function_declare, count, suffix), (func, ret_t, ##__VA_ARGS__))
//#define LUAA_DECLARE(func, ret_t, count, suffix, ...) LUAA_APPLY(LUAA_JOIN3(luaA_function_declare, count, suffix), (func, ret_t, ##__VA_ARGS__))
#define LUAA_REGISTER(L, func, ret_t, count, ...) LUAA_APPLY(LUAA_JOIN2(luaA_function_register, count), (L, func, ret_t, ##__VA_ARGS__))

/*
** MSVC does not allow nested functions
** so function is wrapped in nested struct
*/
#ifdef _MSC_VER

#define luaA_function_declare0(func, ret_t) \
struct __luaA_wrap_##func { static void __luaA_##func(char* out, char* args) { \
  *(ret_t*)out = func(); }; }

#define luaA_function_declare0_void(func, ret_t) \
struct __luaA_wrap_##func { static void __luaA_##func(char* out, char* args) { \
  func(); }; }

#define luaA_function_declare1(func, ret_t, arg0_t) \
struct __luaA_wrap_##func { static void __luaA_##func(char* out, char* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  *(ret_t*)out = func(a0); }; }

#define luaA_function_declare1_void(func, ret_t, arg0_t) \
struct __luaA_wrap_##func { static void __luaA_##func(char* out, char* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  func(a0); }; }

#define luaA_function_declare2(func, ret_t, arg0_t, arg1_t) \
struct __luaA_wrap_##func { static void __luaA_##func(char* out, char* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  *(ret_t*)out = func(a0, a1); }; }

#define luaA_function_declare2_void(func, ret_t, arg0_t, arg1_t) \
struct __luaA_wrap_##func { static void __luaA_##func(char* out, char* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  func(a0, a1); }; }

#define luaA_function_declare3(func, ret_t, arg0_t, arg1_t, arg2_t) \
struct __luaA_wrap_##func { static void __luaA_##func(char* out, char* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  *(ret_t*)out = func(a0, a1, a2); }; }

#define luaA_function_declare3_void(func, ret_t, arg0_t, arg1_t, arg2_t) \
struct __luaA_wrap_##func { static void __luaA_##func(char* out, char* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  func(a0, a1, a2); }; }

#define luaA_function_declare4(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t) \
struct __luaA_wrap_##func { static void __luaA_##func(char* out, char* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  *(ret_t*)out = func(a0, a1, a2, a3); }; }

#define luaA_function_declare4_void(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t) \
struct __luaA_wrap_##func { static void __luaA_##func(char* out, char* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  func(a0, a1, a2, a3); }; }

#define luaA_function_declare5(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t) \
struct __luaA_wrap_##func { static void __luaA_##func(char* out, char* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  arg4_t a4 = *(arg4_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)); \
  *(ret_t*)out = func(a0, a1, a2, a3, a4); }; }

#define luaA_function_declare5_void(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t) \
struct __luaA_wrap_##func { static void __luaA_##func(char* out, char* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  arg4_t a4 = *(arg4_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)); \
  func(a0, a1, a2, a3, a4); }; }

#define luaA_function_declare6(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t) \
struct __luaA_wrap_##func { static void __luaA_##func(char* out, char* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  arg4_t a4 = *(arg4_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)); \
  arg5_t a5 = *(arg5_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)); \
  *(ret_t*)out = func(a0, a1, a2, a3, a4, a5); }; }

#define luaA_function_declare6_void(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t) \
struct __luaA_wrap_##func { static void __luaA_##func(char* out, char* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  arg4_t a4 = *(arg4_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)); \
  arg5_t a5 = *(arg5_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)); \
  func(a0, a1, a2, a3, a4, a5); }; }

#define luaA_function_declare7(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t) \
struct __luaA_wrap_##func { static void __luaA_##func(char* out, char* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  arg4_t a4 = *(arg4_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)); \
  arg5_t a5 = *(arg5_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)); \
  arg6_t a6 = *(arg6_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)); \
  *(ret_t*)out = func(a0, a1, a2, a3, a4, a5, a6); }; }

#define luaA_function_declare7_void(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t) \
struct __luaA_wrap_##func { static void __luaA_##func(char* out, char* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  arg4_t a4 = *(arg4_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)); \
  arg5_t a5 = *(arg5_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)); \
  arg6_t a6 = *(arg6_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)); \
  func(a0, a1, a2, a3, a4, a5, a6); }; }

#define luaA_function_declare8(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t, arg7_t) \
struct __luaA_wrap_##func { static void __luaA_##func(char* out, char* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  arg4_t a4 = *(arg4_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)); \
  arg5_t a5 = *(arg5_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)); \
  arg6_t a6 = *(arg6_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)); \
  arg7_t a7 = *(arg7_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)+sizeof(arg6_t)); \
  *(ret_t*)out = func(a0, a1, a2, a3, a4, a5, a6, a7); }; }

#define luaA_function_declare8_void(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t, arg7_t) \
struct __luaA_wrap_##func { static void __luaA_##func(char* out, char* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  arg4_t a4 = *(arg4_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)); \
  arg5_t a5 = *(arg5_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)); \
  arg6_t a6 = *(arg6_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)); \
  arg7_t a7 = *(arg7_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)+sizeof(arg6_t)); \
  func(a0, a1, a2, a3, a4, a5, a6, a7); }; }

#define luaA_function_declare9(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t, arg7_t, arg8_t) \
struct __luaA_wrap_##func { static void __luaA_##func(char* out, char* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  arg4_t a4 = *(arg4_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)); \
  arg5_t a5 = *(arg5_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)); \
  arg6_t a6 = *(arg6_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)); \
  arg7_t a7 = *(arg7_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)+sizeof(arg6_t)); \
  arg8_t a8 = *(arg8_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)+sizeof(arg6_t)+sizeof(arg7_t)); \
  *(ret_t*)out = func(a0, a1, a2, a3, a4, a5, a6, a7, a8); }; }

#define luaA_function_declare9_void(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t, arg7_t, arg8_t) \
struct __luaA_wrap_##func { static void __luaA_##func(char* out, char* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  arg4_t a4 = *(arg4_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)); \
  arg5_t a5 = *(arg5_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)); \
  arg6_t a6 = *(arg6_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)); \
  arg7_t a7 = *(arg7_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)+sizeof(arg6_t)); \
  arg8_t a8 = *(arg8_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)+sizeof(arg6_t)+sizeof(arg7_t)); \
  func(a0, a1, a2, a3, a4, a5, a6, a7, a8); }; }

#define luaA_function_declare10(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t, arg7_t, arg8_t, arg9_t) \
struct __luaA_wrap_##func { static void __luaA_##func(char* out, char* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  arg4_t a4 = *(arg4_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)); \
  arg5_t a5 = *(arg5_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)); \
  arg6_t a6 = *(arg6_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)); \
  arg7_t a7 = *(arg7_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)+sizeof(arg6_t)); \
  arg8_t a8 = *(arg8_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)+sizeof(arg6_t)+sizeof(arg7_t)); \
  arg9_t a9 = *(arg9_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)+sizeof(arg6_t)+sizeof(arg7_t)+sizeof(arg8_t)); \
  *(ret_t*)out = func(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9); }; }

#define luaA_function_declare10_void(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t, arg7_t, arg8_t, arg9_t) \
struct __luaA_wrap_##func { static void __luaA_##func(char* out, char* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  arg4_t a4 = *(arg4_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)); \
  arg5_t a5 = *(arg5_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)); \
  arg6_t a6 = *(arg6_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)); \
  arg7_t a7 = *(arg7_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)+sizeof(arg6_t)); \
  arg8_t a8 = *(arg8_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)+sizeof(arg6_t)+sizeof(arg7_t)); \
  arg9_t a9 = *(arg9_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)+sizeof(arg6_t)+sizeof(arg7_t)+sizeof(arg8_t)); \
  func(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9); }; }

  
#define luaA_function_register0(L, func, ret_t) \
  luaA_function_register_type(L, func, (luaA_Func)__luaA_wrap_##func::__luaA_##func, #func, luaA_type(L, ret_t), 0)

#define luaA_function_register1(L, func, ret_t, arg0_t) \
  luaA_function_register_type(L, func, (luaA_Func)__luaA_wrap_##func::__luaA_##func, #func, luaA_type(L, ret_t), 1, luaA_type(L, arg0_t))

#define luaA_function_register2(L, func, ret_t, arg0_t, arg1_t) \
  luaA_function_register_type(L, func, (luaA_Func)__luaA_wrap_##func::__luaA_##func, #func, luaA_type(L, ret_t), 2, luaA_type(L, arg0_t), luaA_type(L, arg1_t))

#define luaA_function_register3(L, func, ret_t, arg0_t, arg1_t, arg2_t) \
  luaA_function_register_type(L, func, (luaA_Func)__luaA_wrap_##func::__luaA_##func, #func, luaA_type(L, ret_t), 3, luaA_type(L, arg0_t), luaA_type(L, arg1_t), luaA_type(L, arg2_t))

#define luaA_function_register4(L, func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t) \
  luaA_function_register_type(L, func, (luaA_Func)__luaA_wrap_##func::__luaA_##func, #func, luaA_type(L, ret_t), 4, luaA_type(L, arg0_t), luaA_type(L, arg1_t), luaA_type(L, arg2_t), luaA_type(L, arg3_t))
  
#define luaA_function_register5(L, func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t) \
  luaA_function_register_type(L, func, (luaA_Func)__luaA_wrap_##func::__luaA_##func, #func, luaA_type(L, ret_t), 5, luaA_type(L, arg0_t), luaA_type(L, arg1_t), luaA_type(L, arg2_t), luaA_type(L, arg3_t), luaA_type(L, arg4_t))
  
#define luaA_function_register6(L, func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t) \
  luaA_function_register_type(L, func, (luaA_Func)__luaA_wrap_##func::__luaA_##func, #func, luaA_type(L, ret_t), 6, luaA_type(L, arg0_t), luaA_type(L, arg1_t), luaA_type(L, arg2_t), luaA_type(L, arg3_t), luaA_type(L, arg4_t), luaA_type(L, arg5_t))

#define luaA_function_register7(L, func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t) \
  luaA_function_register_type(L, func, (luaA_Func)__luaA_wrap_##func::__luaA_##func, #func, luaA_type(L, ret_t), 7, luaA_type(L, arg0_t), luaA_type(L, arg1_t), luaA_type(L, arg2_t), luaA_type(L, arg3_t), luaA_type(L, arg4_t), luaA_type(L, arg5_t), luaA_type(L, arg6_t))
  
#define luaA_function_register8(L, func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t, arg7_t) \
  luaA_function_register_type(L, func, (luaA_Func)__luaA_wrap_##func::__luaA_##func, #func, luaA_type(L, ret_t), 8, luaA_type(L, arg0_t), luaA_type(L, arg1_t), luaA_type(L, arg2_t), luaA_type(L, arg3_t), luaA_type(L, arg4_t), luaA_type(L, arg5_t), luaA_type(L, arg6_t), luaA_type(L, arg7_t))
  
#define luaA_function_register9(L, func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t, arg7_t, arg8_t) \
  luaA_function_register_type(L, func, (luaA_Func)__luaA_wrap_##func::__luaA_##func, #func, luaA_type(L, ret_t), 9, luaA_type(L, arg0_t), luaA_type(L, arg1_t), luaA_type(L, arg2_t), luaA_type(L, arg3_t), luaA_type(L, arg4_t), luaA_type(L, arg5_t), luaA_type(L, arg6_t), luaA_type(L, arg7_t), luaA_type(L, arg8_t))
  
#define luaA_function_register10(L, func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t, arg7_t, arg8_t, arg9_t) \
  luaA_function_register_type(L, func, (luaA_Func)__luaA_wrap_##func::__luaA_##func, #func, luaA_type(L, ret_t), 10, luaA_type(L, arg0_t), luaA_type(L, arg1_t), luaA_type(L, arg2_t), luaA_type(L, arg3_t), luaA_type(L, arg4_t), luaA_type(L, arg5_t), luaA_type(L, arg6_t), luaA_type(L, arg7_t), luaA_type(L, arg8_t), luaA_type(L, arg9_t))

  
#else


#define luaA_function_declare0(func, ret_t) \
void __luaA_##func(void* out, void* args) { \
  *(ret_t*)out = func(); }

#define luaA_function_declare0_void(func, ret_t) \
void __luaA_##func(void* out, void* args) { \
  func(); }

#define luaA_function_declare1(func, ret_t, arg0_t) \
void __luaA_##func(void* out, void* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  *(ret_t*)out = func(a0); }

#define luaA_function_declare1_void(func, ret_t, arg0_t) \
void __luaA_##func(void* out, void* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  func(a0); }

#define luaA_function_declare2(func, ret_t, arg0_t, arg1_t) \
void __luaA_##func(void* out, void* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  *(ret_t*)out = func(a0, a1); }

#define luaA_function_declare2_void(func, ret_t, arg0_t, arg1_t) \
void __luaA_##func(void* out, void* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  func(a0, a1); }

#define luaA_function_declare3(func, ret_t, arg0_t, arg1_t, arg2_t) \
void __luaA_##func(void* out, void* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  *(ret_t*)out = func(a0, a1, a2); }

#define luaA_function_declare3_void(func, ret_t, arg0_t, arg1_t, arg2_t) \
void __luaA_##func(void* out, void* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  func(a0, a1, a2); }

#define luaA_function_declare4(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t) \
void __luaA_##func(void* out, void* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  *(ret_t*)out = func(a0, a1, a2, a3); }

#define luaA_function_declare4_void(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t) \
void __luaA_##func(void* out, void* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  func(a0, a1, a2, a3); }

#define luaA_function_declare5(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t) \
void __luaA_##func(void* out, void* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  arg4_t a4 = *(arg4_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)); \
  *(ret_t*)out = func(a0, a1, a2, a3, a4); }

#define luaA_function_declare5_void(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t) \
void __luaA_##func(void* out, void* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  arg4_t a4 = *(arg4_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)); \
  func(a0, a1, a2, a3, a4); }

#define luaA_function_declare6(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t) \
void __luaA_##func(void* out, void* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  arg4_t a4 = *(arg4_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)); \
  arg5_t a5 = *(arg5_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)); \
  *(ret_t*)out = func(a0, a1, a2, a3, a4, a5); }

#define luaA_function_declare6_void(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t) \
void __luaA_##func(void* out, void* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  arg4_t a4 = *(arg4_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)); \
  arg5_t a5 = *(arg5_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)); \
  func(a0, a1, a2, a3, a4, a5); }

#define luaA_function_declare7(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t) \
void __luaA_##func(void* out, void* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  arg4_t a4 = *(arg4_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)); \
  arg5_t a5 = *(arg5_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)); \
  arg6_t a6 = *(arg6_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)); \
  *(ret_t*)out = func(a0, a1, a2, a3, a4, a5, a6); }

#define luaA_function_declare7_void(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t) \
void __luaA_##func(void* out, void* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  arg4_t a4 = *(arg4_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)); \
  arg5_t a5 = *(arg5_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)); \
  arg6_t a6 = *(arg6_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)); \
  func(a0, a1, a2, a3, a4, a5, a6); }

#define luaA_function_declare8(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t, arg7_t) \
void __luaA_##func(void* out, void* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  arg4_t a4 = *(arg4_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)); \
  arg5_t a5 = *(arg5_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)); \
  arg6_t a6 = *(arg6_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)); \
  arg7_t a7 = *(arg7_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)+sizeof(arg6_t)); \
  *(ret_t*)out = func(a0, a1, a2, a3, a4, a5, a6, a7); }

#define luaA_function_declare8_void(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t, arg7_t) \
void __luaA_##func(void* out, void* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  arg4_t a4 = *(arg4_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)); \
  arg5_t a5 = *(arg5_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)); \
  arg6_t a6 = *(arg6_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)); \
  arg7_t a7 = *(arg7_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)+sizeof(arg6_t)); \
  func(a0, a1, a2, a3, a4, a5, a6, a7); }

#define luaA_function_declare9(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t, arg7_t, arg8_t) \
void __luaA_##func(void* out, void* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  arg4_t a4 = *(arg4_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)); \
  arg5_t a5 = *(arg5_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)); \
  arg6_t a6 = *(arg6_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)); \
  arg7_t a7 = *(arg7_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)+sizeof(arg6_t)); \
  arg8_t a8 = *(arg8_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)+sizeof(arg6_t)+sizeof(arg7_t)); \
  *(ret_t*)out = func(a0, a1, a2, a3, a4, a5, a6, a7, a8); }

#define luaA_function_declare9_void(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t, arg7_t, arg8_t) \
void __luaA_##func(void* out, void* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  arg4_t a4 = *(arg4_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)); \
  arg5_t a5 = *(arg5_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)); \
  arg6_t a6 = *(arg6_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)); \
  arg7_t a7 = *(arg7_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)+sizeof(arg6_t)); \
  arg8_t a8 = *(arg8_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)+sizeof(arg6_t)+sizeof(arg7_t)); \
  func(a0, a1, a2, a3, a4, a5, a6, a7, a8); }

#define luaA_function_declare10(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t, arg7_t, arg8_t, arg9_t) \
void __luaA_##func(void* out, void* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  arg4_t a4 = *(arg4_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)); \
  arg5_t a5 = *(arg5_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)); \
  arg6_t a6 = *(arg6_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)); \
  arg7_t a7 = *(arg7_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)+sizeof(arg6_t)); \
  arg8_t a8 = *(arg8_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)+sizeof(arg6_t)+sizeof(arg7_t)); \
  arg9_t a9 = *(arg9_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)+sizeof(arg6_t)+sizeof(arg7_t)+sizeof(arg8_t)); \
  *(ret_t*)out = func(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9); }

#define luaA_function_declare10_void(func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t, arg7_t, arg8_t, arg9_t) \
void __luaA_##func(void* out, void* args) { \
  arg0_t a0 = *(arg0_t*)args; \
  arg1_t a1 = *(arg1_t*)(args+sizeof(arg0_t)); \
  arg2_t a2 = *(arg2_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)); \
  arg3_t a3 = *(arg3_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)); \
  arg4_t a4 = *(arg4_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)); \
  arg5_t a5 = *(arg5_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)); \
  arg6_t a6 = *(arg6_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)); \
  arg7_t a7 = *(arg7_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)+sizeof(arg6_t)); \
  arg8_t a8 = *(arg8_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)+sizeof(arg6_t)+sizeof(arg7_t)); \
  arg9_t a9 = *(arg9_t*)(args+sizeof(arg0_t)+sizeof(arg1_t)+sizeof(arg2_t)+sizeof(arg3_t)+sizeof(arg4_t)+sizeof(arg5_t)+sizeof(arg6_t)+sizeof(arg7_t)+sizeof(arg8_t)); \
  func(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9); }

  
#define luaA_function_register0(L, func, ret_t) \
  luaA_function_register_type(L, func, __luaA_##func, #func, luaA_type(L, ret_t), 0)

#define luaA_function_register1(L, func, ret_t, arg0_t) \
  luaA_function_register_type(L, func, __luaA_##func, #func, luaA_type(L, ret_t), 1, luaA_type(L, arg0_t))

#define luaA_function_register2(L, func, ret_t, arg0_t, arg1_t) \
  luaA_function_register_type(L, func, __luaA_##func, #func, luaA_type(L, ret_t), 2, luaA_type(L, arg0_t), luaA_type(L, arg1_t))

#define luaA_function_register3(L, func, ret_t, arg0_t, arg1_t, arg2_t) \
  luaA_function_register_type(L, func, __luaA_##func, #func, luaA_type(L, ret_t), 3, luaA_type(L, arg0_t), luaA_type(L, arg1_t), luaA_type(L, arg2_t))

#define luaA_function_register4(L, func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t) \
  luaA_function_register_type(L, func, __luaA_##func, #func, luaA_type(L, ret_t), 4, luaA_type(L, arg0_t), luaA_type(L, arg1_t), luaA_type(L, arg2_t), luaA_type(L, arg3_t))
  
#define luaA_function_register5(L, func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t) \
  luaA_function_register_type(L, func, __luaA_##func, #func, luaA_type(L, ret_t), 5, luaA_type(L, arg0_t), luaA_type(L, arg1_t), luaA_type(L, arg2_t), luaA_type(L, arg3_t), luaA_type(L, arg4_t))
  
#define luaA_function_register6(L, func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t) \
  luaA_function_register_type(L, func, __luaA_##func, #func, luaA_type(L, ret_t), 6, luaA_type(L, arg0_t), luaA_type(L, arg1_t), luaA_type(L, arg2_t), luaA_type(L, arg3_t), luaA_type(L, arg4_t), luaA_type(L, arg5_t))

#define luaA_function_register7(L, func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t) \
  luaA_function_register_type(L, func, __luaA_##func, #func, luaA_type(L, ret_t), 7, luaA_type(L, arg0_t), luaA_type(L, arg1_t), luaA_type(L, arg2_t), luaA_type(L, arg3_t), luaA_type(L, arg4_t), luaA_type(L, arg5_t), luaA_type(L, arg6_t))
  
#define luaA_function_register8(L, func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t, arg7_t) \
  luaA_function_register_type(L, func, __luaA_##func, #func, luaA_type(L, ret_t), 8, luaA_type(L, arg0_t), luaA_type(L, arg1_t), luaA_type(L, arg2_t), luaA_type(L, arg3_t), luaA_type(L, arg4_t), luaA_type(L, arg5_t), luaA_type(L, arg6_t), luaA_type(L, arg7_t))
  
#define luaA_function_register9(L, func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t, arg7_t, arg8_t) \
  luaA_function_register_type(L, func, __luaA_##func, #func, luaA_type(L, ret_t), 9, luaA_type(L, arg0_t), luaA_type(L, arg1_t), luaA_type(L, arg2_t), luaA_type(L, arg3_t), luaA_type(L, arg4_t), luaA_type(L, arg5_t), luaA_type(L, arg6_t), luaA_type(L, arg7_t), luaA_type(L, arg8_t))
  
#define luaA_function_register10(L, func, ret_t, arg0_t, arg1_t, arg2_t, arg3_t, arg4_t, arg5_t, arg6_t, arg7_t, arg8_t, arg9_t) \
  luaA_function_register_type(L, func, __luaA_##func, #func, luaA_type(L, ret_t), 10, luaA_type(L, arg0_t), luaA_type(L, arg1_t), luaA_type(L, arg2_t), luaA_type(L, arg3_t), luaA_type(L, arg4_t), luaA_type(L, arg5_t), luaA_type(L, arg6_t), luaA_type(L, arg7_t), luaA_type(L, arg8_t), luaA_type(L, arg9_t))

  
#endif

// #ifdef MSVC
// #define luaA_function(L, func, ret_t, ...) luaA_function_declare(func, ret_t, ##__VA_ARGS__); luaA_function_register(L, func, ret_t, ##__VA_ARGS__)
#define luaA_function_declare(func, ret_t, ...) LUAA_DECLARE(func, ret_t, LUAA_COUNT(__VA_ARGS__), LUAA_SUFFIX(ret_t), ##__VA_ARGS__)
#define luaA_function_register(L, func, ret_t, ...) LUAA_REGISTER(L, func, ret_t, LUAA_COUNT(__VA_ARGS__), ##__VA_ARGS__)
// #else
// // 主宏定义
// #define luaA_function(L, func, ret_t, ...) \
//     luaA_function_declare(func, ret_t, ##__VA_ARGS__); \
//     luaA_function_register(L, func, ret_t, ##__VA_ARGS__)
// #define luaA_function_declare(func, ret_t, ...) \
//     LUAA_DECLARE(func, ret_t, LUAA_COUNT(__VA_ARGS__), LUAA_SUFFIX(ret_t), __VA_ARGS__)
// #define luaA_function_register(L, func, ret_t, ...) \
//     LUAA_REGISTER(L, func, ret_t, LUAA_COUNT(__VA_ARGS__), __VA_ARGS__)
// #endif

enum {
  LUAA_RETURN_STACK_SIZE   =  256,
  LUAA_ARGUMENT_STACK_SIZE = 2048
};

typedef void (*luaA_Func)(void*, void*);

int luaA_call(lua_State* L, void* func_ptr);
int luaA_call_name(lua_State* L, const char* func_name);

void luaA_function_register_type(lua_State* L, void* src_func, luaA_Func auto_func, const char* name, luaA_Type ret_tid, int num_args, ...);

#endif
