
#ifndef ME_LUA_WRAPPER_EXT_HPP
#define ME_LUA_WRAPPER_EXT_HPP

#include "FallingSand_lua_wrapper.hpp"

class LuaVector final : public std::vector<float> {
public:
    static constexpr const char *LUA_TYPE_NAME = "vector";

    static void RegisterMetaTable(lua_State *L);
    static LuaVector *CheckArg(lua_State *L, int arg);

private:
    static int New(lua_State *L);
    static int GarbageCollect(lua_State *L);
    static int ToString(lua_State *L);
    static int Index(lua_State *L);
    static int NewIndex(lua_State *L);
    static int Len(lua_State *L);
    static int Append(lua_State *L);
    static int Pop(lua_State *L);
    static int Extend(lua_State *L);
    static int Insert(lua_State *L);
    static int Erase(lua_State *L);
    static int Sort(lua_State *L);
};

#ifndef luaSD_MAXDEPTH
#define luaSD_MAXDEPTH 2
#endif

typedef int (*luaSD_printf)(const char *__restrict __format, ...);

void luaSD_stackdump(lua_State *state, luaSD_printf luasdprintf);
void luaSD_stackdump_default(lua_State *state);

#if LUA_VERSION_NUM >= 503
#define lua53_getuservalue lua_getuservalue
#define lua53_gettable lua_gettable
#define lua53_getfield lua_getfield
#define lua53_rawget lua_rawget
#define lua53_rawgeti lua_rawgeti
#define lua53_rawgetp lua_rawgetp
#define lua53_rotate lua_rotate
#else
LUA_API int lua53_getuservalue(lua_State *L, int idx);
LUA_API int lua53_gettable(lua_State *L, int idx);
LUA_API int lua53_getfield(lua_State *L, int idx, const char *field);
LUA_API int lua53_rawget(lua_State *L, int idx);
LUA_API int lua53_rawgeti(lua_State *L, int idx, lua_Integer n);
LUA_API int lua53_rawgetp(lua_State *L, int idx, const void *p);
LUA_API void lua53_rotate(lua_State *L, int idx, int n);
#endif

#if LUA_VERSION_NUM < 502
#define LUA_OK 0
#define lua_getuservalue lua_getfenv
#define lua_setuservalue lua_setfenv
#define lua_rawlen lua_objlen

#define luaL_newlibtable(L, l) lua_createtable(L, 0, sizeof(l) / sizeof((l)[0]) - 1)
#define luaL_newlib(L, l) (luaL_newlibtable(L, l), luaL_setfuncs(L, l, 0))

LUA_API lua_Integer(lua_tointegerx)(lua_State *L, int idx, int *valid);
LUA_API void(lua_rawsetp)(lua_State *L, int idx, const void *p);
LUA_API void(lua_rawgetp)(lua_State *L, int idx, const void *p);
LUALIB_API const char *(luaL_tolstring)(lua_State *L, int idx, size_t *len);
LUALIB_API void(luaL_setfuncs)(lua_State *L, const luaL_Reg *l, int nup);
#endif /* LUA_VERSION_NUM */

/* lbind internal max alignment */
#ifndef LBIND_MAXALIGN
#define LBIND_MAXALIGN \
    union {            \
        double u;      \
        void *s;       \
        long l;        \
    }
#endif

typedef LBIND_MAXALIGN lbind_MaxAlign;

/* lbind runtime */
#ifndef LBIND_NO_RUNTIME
int luaopen_lbind(lua_State *L);
#endif /* LBIND_NO_RUNTIME */

/* lbind utils functions */
int lbind_relindex(int idx, int onstack);
int lbind_argferror(lua_State *L, int idx, const char *fmt, ...);
int lbind_typeerror(lua_State *L, int idx, const char *tname);
int lbind_matcherror(lua_State *L, const char *extramsg);
int lbind_copystack(lua_State *from, lua_State *to, int nargs);
int lbind_hasfield(lua_State *L, int idx, const char *field);
int lbind_self(lua_State *L, const void *p, const char *method, int nargs, int *ptraceback);
int lbind_pcall(lua_State *L, int nargs, int nrets);

const char *lbind_dumpstack(lua_State *L, const char *extramsg);

#define lbind_returnself(L) \
    do {                    \
        lua_settop((L), 1); \
        return 1;           \
    } while (0)

#define lbind_printstack(L, msg) (printf("%s\n", lbind_dumpstack((L), (msg))), lua_pop((L), 1))

/* lbind lua module install */
typedef struct lbind_Reg {
    const char *name;        /* name of library */
    lua_CFunction open_func; /* luaopen_ function of library */
} lbind_Reg;

void lbind_install(lua_State *L, lbind_Reg *reg);
int lbind_requiref(lua_State *L, const char *name, lua_CFunction loader);
void lbind_requirelibs(lua_State *L, lbind_Reg *reg);
void lbind_requireinto(lua_State *L, const char *prefix, lbind_Reg *reg);

/* metatable maintain */
int lbind_setmetatable(lua_State *L, const void *t);
int lbind_getmetatable(lua_State *L, const void *t);
int lbind_setmetafield(lua_State *L, int idx, const char *field);
int lbind_setlibcall(lua_State *L, const char *method);

#define LBIND_INDEX 0x01
#define LBIND_NEWINDEX 0x02

void lbind_setaccessors(lua_State *L, int ntables, int field);
void lbind_setarrayf(lua_State *L, lua_CFunction f, int field);
void lbind_sethashf(lua_State *L, lua_CFunction f, int field);
void lbind_setmaptable(lua_State *L, luaL_Reg libs[], int field);

#define lbind_checkreadonly(L) ((void)(lua_gettop(L) != 2 && luaL_error((L), "field %s is read-only", lbind_tostring((L), 2))))

#define lbind_checkwriteonly(L) ((void)(lua_gettop(L) != 3 && luaL_error((L), "field %s is write-only", lbind_tostring((L), 2))))

/* light userdata utils */
int lbind_getudtypebox(lua_State *L);
int lbind_getlightuservalue(lua_State *L, const void *p);
void lbind_setlightuservalue(lua_State *L, const void *p);

/* lbind peer table support, define LBIND_NO_PEER to disable this.
 *
 * peer table is a normal table that has a field "__peer", t.__peer is
 * the real userdata, in this case, lbind use this table as it is
 * t.__peer. i.e. lbind treat that table as a native object.
 */
void *lbind_touserdata(lua_State *L, int idx);

/* lbind class runtime */

/*
 * NOTE: all types must have a global Type structure. its address used
 * to find the right informations about type. So all const lbind_Type *t
 * argument must be &var, where var is declared by , e.g.
 *  lbind_Type basetype;
 */
typedef struct lbind_Type lbind_Type;

typedef void *lbind_Cast(lua_State *L, int idx, const lbind_Type *to_type);

struct lbind_Type {
    const char *name;
    int flags;
    lbind_Cast *cast;
    lbind_Type **bases;
};

/* lbind type registry
 *
 * a lbind object can tracked and interned.
 *
 * If a object is tracked, when it collected (i.e. not used any more)
 * it's delete function in metatable will called. otherwise it won't
 * deleted by Lua, i.e. it's life-time is not associate with Lua.
 *
 * When a object is interned, You can use it's pointer to find the
 * object itself.
 *
 * if a type has flags LBIND_ACCESSOR, it has a non-trival __index and
 * __newindex, that means the object of this type has the ability to
 * save any value into it's uservalue and can have custom accessors.
 * if a type has base type, it also have LBIND_ACCESSOR flag.
 */
#define LBIND_TRACK 0x01
#define LBIND_INTERN 0x02
#define LBIND_ACCESSOR 0x04

#ifndef LBIND_DEFAULT_FLAG
#define LBIND_DEFAULT_FLAG (LBIND_TRACK)
#endif

#define LBIND_INIT(name) \
    { name, LBIND_DEFAULT_FLAG, NULL, NULL }
#define LBIND_TYPE(var, name) lbind_Type var = LBIND_INIT(name)

void lbind_inittype(lbind_Type *t, const char *name);
void lbind_setbase(lbind_Type *t, lbind_Type **bases, lbind_Cast *cast);
int lbind_settrack(lbind_Type *t, int autotrack);
int lbind_setintern(lbind_Type *t, int autointern);

/* lbind type metatable */
int lbind_newmetatable(lua_State *L, luaL_Reg *libs, const lbind_Type *t);
void lbind_setagency(lua_State *L);

/* get lbind_Type* from metatable */
lbind_Type *lbind_typeobject(lua_State *L, int idx);

/* lbind type system */
const char *lbind_tolstring(lua_State *L, int idx, size_t *plen);
const char *lbind_type(lua_State *L, int idx);

int lbind_isa(lua_State *L, int idx, const lbind_Type *t);
int lbind_copy(lua_State *L, const void *p, const lbind_Type *t);
void *lbind_cast(lua_State *L, int idx, const lbind_Type *t);
void *lbind_check(lua_State *L, int idx, const lbind_Type *t);
void *lbind_test(lua_State *L, int idx, const lbind_Type *t);

#define lbind_opt(L, idx, defs, t) (lua_isnoneornil((L), (idx)) ? (defs) : lbind_check((L), (idx), (t)))

#define lbind_tostring(L, idx) lbind_tolstring((L), (idx), NULL)

/* lbind object creation
 * `lbind_raw` create a raw lbind object, not associate with a
 * lbind_Type, if `intern` is non-zero, intern it.
 * `lbind_new` create a lbind object associated with a lbind_Type,
 * this type decide whether the object is signed up.
 * `lbind_wrap` wrap a pointer to lbind object associated with
 * lbind_Type, the type decide the signing.
 */
void *lbind_raw(lua_State *L, size_t objsize, int intern);
void *lbind_new(lua_State *L, size_t objsize, const lbind_Type *t);
void *lbind_wrap(lua_State *L, void *p, const lbind_Type *t);

/* delete a lbind object. unsign, clear and remove metatable of it.  */
void *lbind_delete(lua_State *L, int idx);

/* get pointer from a lbind object, or NULL. */
void *lbind_object(lua_State *L, int idx);

/* intern a object with a pointer p, object is on stack. */
void lbind_intern(lua_State *L, const void *p);

/* get lbind object userdata from object pointer.
 * require interned before */
int lbind_retrieve(lua_State *L, const void *p);

/* track/untrack object */
void lbind_track(lua_State *L, int idx);
void lbind_untrack(lua_State *L, int idx);
int lbind_hastrack(lua_State *L, int idx);

/* lbind enum runtime */
#ifndef LBIND_NO_ENUM

typedef struct lbind_EnumItem {
    const char *name;
    int value;
} lbind_EnumItem;

typedef struct lbind_Enum {
    const char *name;
    size_t nitem;
    lbind_EnumItem *items;
} lbind_Enum;

#define LBIND_INITENUM(name, es) \
    { name, sizeof(es) / sizeof((es)[0]), es }
#define LBIND_ENUM(var, name, es) lbind_Enum var = LBIND_INITENUM(name, es)

void lbind_initenum(lbind_Enum *et, const char *name);

lbind_EnumItem *lbind_findenum(lbind_Enum *et, const char *s, size_t len);

int lbind_pushenum(lua_State *L, const char *name, lbind_Enum *et);
int lbind_testenum(lua_State *L, int idx, lbind_Enum *et);
int lbind_checkenum(lua_State *L, int idx, lbind_Enum *et);

int lbind_pushmask(lua_State *L, int evalue, lbind_Enum *et);
int lbind_testmask(lua_State *L, int idx, lbind_Enum *et);
int lbind_checkmask(lua_State *L, int idx, lbind_Enum *et);

#define lbind_optenum(L, idx, defs, t) (lua_isnoneornil((L), (idx)) ? (defs) : lbind_checkenum((L), (idx), (t)))

#define lbind_optmask(L, idx, defs, t) (lua_isnoneornil((L), (idx)) ? (defs) : lbind_checkmask((L), (idx), (t)))

#endif /* LBIND_NO_ENUM */

int luaopen_lbind(lua_State *L);

#endif
