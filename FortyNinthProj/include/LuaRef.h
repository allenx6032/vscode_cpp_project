//------------------------------------------------------------------------------
/*

   Copyright 2017, Nigel Atkinson

License: The MIT License (http://www.opensource.org/licenses/mit-license.php)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
//==============================================================================

#ifndef __LUAREF_H
#define __LUAREF_H

#include <lua_includes.h>
#include <string>
#include <tuple> // For std::ignore
#include "LuaStack.h"
#include "LuaException.h"

namespace ManualBind {

struct LuaNil
{
};

template<>
struct LuaStack<LuaNil>
{
    static inline void push( lua_State* L, LuaNil const& nil )
    {
        lua_pushnil(L);
    }
};

class LuaRef;

class LuaRefBase
{
protected:
    lua_State* m_L;
    int m_ref;

    class StackPopper
    {
        lua_State* m_L;
        int m_count;
    public:
        StackPopper( lua_State* L, int count = 1 ) : m_L(L), m_count(count)
        {}
        ~StackPopper()
        {
            lua_pop( m_L, m_count );
        }
    };

    struct FromStack {};

    // These constructors as destructor are protected as this
    // class should not be used directly.

    LuaRefBase( lua_State* L, FromStack ) : m_L( L )
    {
        m_ref = luaL_ref( m_L, LUA_REGISTRYINDEX );
    }

    LuaRefBase( lua_State* L, int ref ) : m_L( L ), m_ref( ref )
    {
    }

    ~LuaRefBase()
    {
        luaL_unref( m_L, LUA_REGISTRYINDEX, m_ref );
    }

public:
    virtual void push() const
    {
        lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_ref );
    }

    std::string tostring() const
    {
        lua_getglobal( m_L, "tostring" );
        push();
        lua_call( m_L, 1, 1 );
        const char* str = lua_tostring( m_L, 1 );
        lua_pop( m_L, 1 );
        return std::string(str);
    }

    int type () const
    {
        int result;
        push();
        result = lua_type (m_L, -1);
        lua_pop (m_L, 1);
        return result;
    }

    inline bool isNil () const {
        return type () == LUA_TNIL;
    }
    inline bool isNumber () const {
        return type () == LUA_TNUMBER;
    }
    inline bool isString () const {
        return type () == LUA_TSTRING;
    }
    inline bool isTable () const {
        return type () == LUA_TTABLE;
    }
    inline bool isFunction () const {
        return type () == LUA_TFUNCTION;
    }
    inline bool isUserdata () const {
        return type () == LUA_TUSERDATA;
    }
    inline bool isThread () const {
        return type () == LUA_TTHREAD;
    }
    inline bool isLightUserdata () const {
        return type () == LUA_TLIGHTUSERDATA;
    }

    template<typename... Args>
    inline LuaRef const operator()( Args... args ) const;

    template<typename... Args>
    inline void call( int ret, Args... args ) const;

    template<typename T>
    void append( T v ) const
    {
        push();
        LuaStack<T>::push( m_L, v );
        luaL_ref( m_L, -2 );
        lua_pop( m_L, 1 );
    }

    template<typename T>
    T cast()
    {
        push();
        return LuaStack<T>::get( m_L, -1 );
    }

    template<typename T>
    operator T()
    {
        return cast<T>();
    }
};

template<typename K>
class LuaTableElement : public LuaRefBase
{
    friend class LuaRef;
private:
    K m_key;

    // This constructor has to be public, so that the operator[]
    // with a differing template type can call it.
    // I could not find a way to 'friend' it.
public:
    // Expects on the Lua stack
    // 1 - The table
    LuaTableElement( lua_State* L, K key )
        : LuaRefBase( L, FromStack() )
        , m_key( key )
    {
    }

    void push() const override
    {
        lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_ref );
        LuaStack<K>::push( m_L, m_key );
        lua_gettable( m_L, -2 );
        lua_remove( m_L, -2 );
    }

    // Assign a new value to this table/key.
    template<typename T>
    LuaTableElement& operator= ( T v )
    {
        StackPopper p( m_L );
        lua_rawgeti( m_L, LUA_REGISTRYINDEX, m_ref );
        LuaStack<K>::push( m_L, m_key );
        LuaStack<T>::push( m_L, v );
        lua_settable( m_L, -3 );
        return *this;
    }

    template<typename NK>
    LuaTableElement<NK> operator[]( NK key ) const
    {
        push();
        return LuaTableElement<NK>( m_L, key );
    }
};

template<typename K>
struct LuaStack<LuaTableElement<K> >
{
    static inline void push( lua_State* L, LuaTableElement<K> const& e )
    {
        e.push();
    }
};

class LuaRef : public LuaRefBase
{
    friend LuaRefBase;
private:
    LuaRef( lua_State* L, FromStack fs ) : LuaRefBase( L, fs )
    {}

public:
    LuaRef( lua_State* L ) : LuaRefBase( L, LUA_REFNIL )
    {
    }

    LuaRef( lua_State* L, const std::string& global ) : LuaRefBase( L, LUA_REFNIL )
    {
        lua_getglobal( m_L, global.c_str() );
        m_ref = luaL_ref( m_L, LUA_REGISTRYINDEX );
    }

    LuaRef( LuaRef const& other ) : LuaRefBase( other.m_L, LUA_REFNIL )
    {
        other.push();
        m_ref = luaL_ref( m_L, LUA_REGISTRYINDEX );
    }

    LuaRef( LuaRef&& other ) noexcept : LuaRefBase( other.m_L, other.m_ref )
    {
        other.m_ref = LUA_REFNIL;
    }

    LuaRef& operator=( LuaRef&& other ) noexcept
    {
        if( this == &other ) return *this;

        std::swap( m_L, other.m_L);
        std::swap( m_ref, other.m_ref);

        return *this;
    }

    LuaRef& operator=( LuaRef const& other )
    {
        if( this == &other ) return *this;
        luaL_unref( m_L, LUA_REGISTRYINDEX, m_ref );
        other.push();
        m_L = other.m_L;
        m_ref = luaL_ref( m_L, LUA_REGISTRYINDEX );
        return *this;
    }

    template<typename K>
    LuaRef& operator=( LuaTableElement<K>&& other ) noexcept
    {
        luaL_unref( m_L, LUA_REGISTRYINDEX, m_ref );
        other.push();
        m_L = other.m_L;
        m_ref = luaL_ref( m_L, LUA_REGISTRYINDEX );
        return *this;
    }

    template<typename K>
    LuaRef& operator=( LuaTableElement<K> const& other )
    {
        luaL_unref( m_L, LUA_REGISTRYINDEX, m_ref );
        other.push();
        m_L = other.m_L;
        m_ref = luaL_ref( m_L, LUA_REGISTRYINDEX );
        return *this;
    }

    template<typename K>
    LuaTableElement<K> operator[]( K key ) const
    {
        push();
        return LuaTableElement<K>( m_L, key );
    }

    static LuaRef fromStack( lua_State* L, int index = -1 )
    {
        lua_pushvalue( L, index );
        return LuaRef( L, FromStack() );
    }

    static LuaRef newTable (lua_State* L)
    {
        lua_newtable (L);
        return LuaRef (L, FromStack ());
    }

    static LuaRef getGlobal (lua_State *L, char const* name)
    {
        lua_getglobal (L, name);
        return LuaRef (L, FromStack ());
    }
};

template<>
struct LuaStack<LuaRef>
{
    static inline void push( lua_State* L, LuaRef const& r )
    {
        r.push();
    }
};

template<>
inline LuaRef const LuaRefBase::operator() () const
{
    push();
    LuaException::pcall (m_L, 0, 1);
    return LuaRef (m_L, FromStack ());
}

template<typename... Args>
inline LuaRef const LuaRefBase::operator()( Args... args ) const
{
    const int n = sizeof...(Args);
    push();
    // Initializer expansion trick to call push for each arg.
    // https://stackoverflow.com/questions/25680461/variadic-template-pack-expansion
    int dummy[] = { 0, ( (void) LuaStack<Args>::push( m_L, std::forward<Args>(args) ), 0 ) ... };
    std::ignore = dummy;
    LuaException::pcall( m_L, n, 1 );
    return LuaRef (m_L, FromStack ());
}

template<>
inline void LuaRefBase::call( int ret ) const
{
    push();
    LuaException::pcall (m_L, 0, ret);
    return; // Return values, if any, are left on the Lua stack.
}

template<typename... Args>
inline void LuaRefBase::call( int ret, Args... args ) const
{
    const int n = sizeof...(Args);
    push();
    // Initializer expansion trick to call push for each arg.
    // https://stackoverflow.com/questions/25680461/variadic-template-pack-expansion
    int dummy[] = { 0, ( (void) LuaStack<Args>::push( m_L, std::forward<Args>(args) ), 0 ) ... };
    std::ignore = dummy;
    LuaException::pcall( m_L, n, ret );
    return; // Return values, if any, are left on the Lua stack.
}

template<>
inline LuaRef LuaRefBase::cast()
{
    push();
    return LuaRef( m_L, FromStack() );
}

}; // namespace ManualBind

#endif // __LUAREF_H
