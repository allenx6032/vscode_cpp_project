#include "luaw.hh"

#include <cassert>
#include <cstring>
#include <cstdio>

#include <string>
#include <tuple>
#include <map>
#include <set>
#include <variant>
#include <vector>

using namespace std::string_literals;

extern "C" {
#if LUAW == JIT
# include "lua_jit.hpp"
// # include "luaw/test-jit.hh"
#else
# include "lua.hpp"
// # include "luaw/test-54.hh"
#endif
}

int hello_f(lua_State*) {
    printf("Lua function!\n");
    return 0;
}

struct Wrappeable {
    [[nodiscard]] std::string test() const { return "hello world"; }

    static constexpr const char* mt_identifier = "WRP";
};

int main()
{
    lua_State* L = luaw_newstate();

    luaw_ensure(L);

    // luaw_do

    const char* hello = "print('luaw_do const char* ok!')";
    size_t sz = strlen(hello);
    luaw_do(L, (uint8_t *) hello, sz, 0, "hello");

    luaw_do(L, "print('luaw_do string ok!')");

    /*
    luaw_do(L, "print(");
    luaw_do(L, "print(a[1])");
     */

    // luaw_dump

    luaw_ensure(L);

    printf("---------------------\n");

    auto dump = [&](auto const& v) {
        luaw_do(L, "return "s + v, 1); printf("%s\n", luaw_dump(L, -1).c_str()); lua_pop(L, 1);
    };
    dump("nil");
    dump("42");
    dump("42.8");
    dump("true");
    dump("false");
    dump("'string'");
    dump("function() end");
    dump("{ a=4, 'b', { 'c', 'd' } }");

    lua_pushlightuserdata(L, (void *) hello);
    printf("%s\n", luaw_dump(L, -1).c_str());
    lua_pop(L, 1);

    luaw_push(L, true);
    printf("%s\n", luaw_dump(L, -1).c_str());
    lua_pop(L, 1);

    // luaw_dump_stack

    printf("---------------------\n");

    luaw_ensure(L);

    lua_pushstring(L, "abc");
    lua_pushinteger(L, 42);
    printf("%s\n", luaw_dump_stack(L).c_str());
    lua_pop(L, 2);

    printf("---------------------\n");

    // stack management

    luaw_ensure(L);

    luaw_push(L, true); assert(luaw_pop<bool>(L));
    luaw_push(L, 42); assert(luaw_pop<int>(L) == 42);
    luaw_push(L, 42.8); assert(luaw_pop<double>(L) == 42.8);
    luaw_push(L, "hello"s); assert(luaw_pop<std::string>(L) == "hello");
    luaw_push(L, "hello"); assert(std::string(luaw_pop<const char*>(L)) == "hello");

    int* x = (int *) malloc(sizeof(int));
    *x = 40;
    luaw_push<int *>(L, x);
    assert(*luaw_pop<int*>(L) == 40);
    free(x);

    std::vector<int> v { 10, 20, 30 };
    luaw_push(L, v);
    assert(luaw_pop<std::vector<int>>(L) == v);

    std::optional<int> o = 42;
    luaw_push(L, o); assert(luaw_pop<std::optional<int>>(L) == 42);

    std::pair<int, std::string> p = { 42, "hello" };
    luaw_push(L, p);
    assert(luaw_pop<decltype(p)>(L) == p);

    std::tuple<bool, int, std::string> tp = { false, 48, "str" };
    luaw_push(L, tp);
    printf("%s\n", luaw_dump(L, -1).c_str());
    assert(luaw_pop<decltype(tp)>(L) == tp);

    std::map<std::string, int> mp { { "hello", 42 }, { "world", 18 } };
    luaw_push(L, mp);
    printf("%s\n", luaw_dump(L, -1).c_str());
    luaw_is<std::map<std::string, int>>(L, -1);
    assert(luaw_pop<decltype(mp)>(L) == mp);

    /*
    std::variant<int, double> vv { 42 };
    luaw_push(L, vv);
    assert(luaw_pop<decltype(vv)>(L) == vv);
     */

    printf("---------------------\n");

    // globals

    luaw_ensure(L);

    luaw_setglobal(L, "word", "hello world");
    assert(luaw_getglobal<std::string>(L, "word") == "hello world");

    printf("---------------------\n");

    // iterations

    luaw_ensure(L);

    luaw_do(L, "return { 'd', 'e', a=1, b=2, c=3, [8]='f' }", 1);
    luaw_ipairs(L, -1, [](lua_State* L, int i) {
        printf("%d: %s   ", i, lua_tostring(L, -1));
    });
    printf("\n");
    luaw_spairs(L, -1, [](lua_State* L, std::string const& key) {
        printf("%s: %s   ", key.c_str(), luaw_dump(L, -1).c_str());
    });
    printf("\n");
    luaw_pairs(L, -1, [](lua_State* L) {
        printf("%s: %s   ", luaw_dump(L, -2).c_str(), luaw_dump(L, -1).c_str());
    });
    printf("\n");
    lua_pop(L, 1);

    printf("---------------------\n");

    // fields

    luaw_ensure(L);

    luaw_do(L, "return { a = { b = { c = 84 } } }", 1);
    assert(luaw_hasfield(L, -1, "a.b"));
    assert(luaw_hasfield(L, -1, "a.b.c"));
    assert(luaw_hasfield(L, -1, "a.b.c.d") == false);

    luaw_getfield(L, -1, "a.b.c");
    assert(luaw_pop<int>(L) == 84);
    assert(luaw_getfield<int>(L, -1, "a.b.c") == 84);

    luaw_push(L, 65);
    luaw_setfield(L, -2, "a.b.d");
    assert(luaw_getfield<int>(L, -1, "a.b.d") == 65);

    luaw_setfield(L, -1, "a.b.e", "hello");
    assert(luaw_getfield<std::string>(L, -1, "a.b.e") == "hello");

    lua_pop(L, 1);

    printf("---------------------\n");

    // methods

    luaw_do(L, "function dbl(x) return x * 2 end");
    assert(luaw_call_global<int>(L, "dbl", 24) == 48);

    luaw_do(L, "function hello(str) print('Hello '..str..'!') end");
    luaw_call_global(L, "hello", "world");

    luaw_ensure(L);

    // table types

    struct Point {
        int x, y;

        void to_lua(lua_State* L) const {
            lua_newtable(L);
            luaw_setfield(L, -1, "x", x);
            luaw_setfield(L, -1, "y", y);
        }

        static Point from_lua(lua_State* L, int index) {
            return {
                .x = luaw_getfield<int>(L, index, "x"),
                .y = luaw_getfield<int>(L, index, "y"),
            };
        }

        static bool lua_is(lua_State* L, int index) {
            return luaw_hasfield(L, index, "x") && luaw_hasfield(L, index, "y");
        }

        [[nodiscard]] std::string to_str() const { return "<"s + std::to_string(x) + "," + std::to_string(y) + ">"; }
    };

    luaw_set_metatable<Point>(L, {
            { "__tostring", [](lua_State *L) {
                luaw_push(L, luaw_to<Point>(L, 1).to_str());
                return 1;
            } },
    });

    luaw_setglobal(L, "pt1", Point { 3, 4 });
    assert(luaw_do<int>(L, "return pt1.x") == 3);
    Point pp = luaw_getglobal<Point>(L, "pt1");
    assert(pp.x == 3 && pp.y == 4);

    luaw_do(L, "return pt1", 1);
    assert(luaw_is<Point>(L, -1));
    printf("%s\n", luaw_to_string(L, -1).c_str());
    lua_pop(L, 1);

    luaw_do(L, "print(pt1)");

    // userdata
    struct Hello {
        Hello(std::string s) { printf("HELLO %s!\n", s.c_str()); }
        ~Hello() { printf("HELLO destroyed.\n"); }

        int x = 8;
    };

    Hello* hh = luaw_push_new_userdata<Hello>(L, "WORLD");
    printf("%d\n", hh->x);
    hh->x = 7;

    Hello* h2 = luaw_to<Hello*>(L, -1);
    printf("%d\n", h2->x);
    lua_pop(L, 1);

    luaw_ensure(L);

    // userdata override GC
    luaw_set_metatable<Hello>(L, {
          { "__tostring", [](lua_State *L) { luaw_push(L, "<HELLO>"); return 1; } }
    });

    luaw_push_new_userdata<Hello>(L, "H3");
    luaw_print_stack(L);
    lua_pop(L, 1);

    // wrapped metadata

    printf("Metatable: '%s'\n", luaw_set_metatable<Wrappeable>(L, {
            { "test", [](lua_State *L) {
                luaw_push(L, luaw_to<Wrappeable*>(L, 1)->test());
                return 1;
            }},
    }).c_str());

    auto wptr = std::make_unique<Wrappeable>();
    luaw_push(L, wptr.get());

    lua_setglobal(L, "wptr");
    luaw_do(L, "print(wptr:test())");

    luaw_do(L, "function test(obj) print(obj:test()) end");
    luaw_call_global(L, "test", wptr.get());

    // odds & ends

    printf("---------------------\n");

    luaw_push(L, hello_f);
    luaw_call(L);

    luaw_do_z(L, test);

    luaw_do(L, "return { a={ b={ c={ 48, 13 }, d=12 } }, j=4 }", 1);
    printf("%s\n", luaw_dump(L, -1, false).c_str());
    printf("%s\n", luaw_dump(L, -1, true, 10).c_str());
    lua_pop(L, 1);

    luaw_do(L, "return { 10, 20, 30, 'a' }", 1);
    printf("%s\n", luaw_dump(L, -1, false).c_str());
    printf("%s\n", luaw_dump(L, -1, true, 10).c_str());
    lua_pop(L, 1);

    lua_close(L);
}
