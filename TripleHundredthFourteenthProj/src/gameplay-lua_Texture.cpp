// Autogenerated by gameplay-luagen
#include "gameplay-Base.h"
#include "gameplay-ScriptController.h"
#include "gameplay-lua_Texture.h"
#include "gameplay-Base.h"
#include "gameplay-FileSystem.h"
#include "gameplay-Game.h"
#include "gameplay-Image.h"
#include "gameplay-Ref.h"
#include "gameplay-Texture.h"
#include "gameplay-Ref.h"

namespace gameplay
{

extern void luaGlobal_Register_Conversion_Function(const char* className, void*(*func)(void*, const char*));

static Texture* getInstance(lua_State* state)
{
    void* userdata = luaL_checkudata(state, 1, "Texture");
    luaL_argcheck(state, userdata != NULL, 1, "'Texture' expected.");
    return (Texture*)((gameplay::ScriptUtil::LuaObject*)userdata)->instance;
}

static int lua_Texture__gc(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                void* userdata = luaL_checkudata(state, 1, "Texture");
                luaL_argcheck(state, userdata != NULL, 1, "'Texture' expected.");
                gameplay::ScriptUtil::LuaObject* object = (gameplay::ScriptUtil::LuaObject*)userdata;
                if (object->owns)
                {
                    Texture* instance = (Texture*)object->instance;
                    SAFE_RELEASE(instance);
                }
                
                return 0;
            }

            lua_pushstring(state, "lua_Texture__gc - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

static int lua_Texture_addRef(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Texture* instance = getInstance(state);
                instance->addRef();
                
                return 0;
            }

            lua_pushstring(state, "lua_Texture_addRef - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

static int lua_Texture_generateMipmaps(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Texture* instance = getInstance(state);
                instance->generateMipmaps();
                
                return 0;
            }

            lua_pushstring(state, "lua_Texture_generateMipmaps - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

static int lua_Texture_getFormat(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Texture* instance = getInstance(state);
                Texture::Format result = instance->getFormat();

                // Push the return value onto the stack.
                lua_pushnumber(state, (int)result);

                return 1;
            }

            lua_pushstring(state, "lua_Texture_getFormat - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

static int lua_Texture_getHandle(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Texture* instance = getInstance(state);
                void* returnPtr = (void*)new GLuint(instance->getHandle());
                if (returnPtr)
                {
                    gameplay::ScriptUtil::LuaObject* object = (gameplay::ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(gameplay::ScriptUtil::LuaObject));
                    object->instance = returnPtr;
                    object->owns = true;
                    luaL_getmetatable(state, "GLuint");
                    lua_setmetatable(state, -2);
                }
                else
                {
                    lua_pushnil(state);
                }

                return 1;
            }

            lua_pushstring(state, "lua_Texture_getHandle - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

static int lua_Texture_getHeight(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Texture* instance = getInstance(state);
                unsigned int result = instance->getHeight();

                // Push the return value onto the stack.
                lua_pushunsigned(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_Texture_getHeight - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

static int lua_Texture_getPath(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Texture* instance = getInstance(state);
                const char* result = instance->getPath();

                // Push the return value onto the stack.
                lua_pushstring(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_Texture_getPath - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

static int lua_Texture_getRefCount(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Texture* instance = getInstance(state);
                unsigned int result = instance->getRefCount();

                // Push the return value onto the stack.
                lua_pushunsigned(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_Texture_getRefCount - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

static int lua_Texture_getType(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Texture* instance = getInstance(state);
                Texture::Type result = instance->getType();

                // Push the return value onto the stack.
                lua_pushnumber(state, (int)result);

                return 1;
            }

            lua_pushstring(state, "lua_Texture_getType - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

static int lua_Texture_getWidth(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Texture* instance = getInstance(state);
                unsigned int result = instance->getWidth();

                // Push the return value onto the stack.
                lua_pushunsigned(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_Texture_getWidth - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

static int lua_Texture_isCompressed(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Texture* instance = getInstance(state);
                bool result = instance->isCompressed();

                // Push the return value onto the stack.
                lua_pushboolean(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_Texture_isCompressed - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

static int lua_Texture_isMipmapped(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Texture* instance = getInstance(state);
                bool result = instance->isMipmapped();

                // Push the return value onto the stack.
                lua_pushboolean(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_Texture_isMipmapped - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

static int lua_Texture_release(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA))
            {
                Texture* instance = getInstance(state);
                instance->release();
                
                return 0;
            }

            lua_pushstring(state, "lua_Texture_release - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

static int lua_Texture_setData(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                (lua_type(state, 2) == LUA_TTABLE || lua_type(state, 2) == LUA_TLIGHTUSERDATA))
            {
                // Get parameter 1 off the stack.
                gameplay::ScriptUtil::LuaArray<unsigned char> param1 = gameplay::ScriptUtil::getUnsignedCharPointer(2);

                Texture* instance = getInstance(state);
                instance->setData(param1);
                
                return 0;
            }

            lua_pushstring(state, "lua_Texture_setData - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 2).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

static int lua_Texture_static_create(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 1:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TSTRING || lua_type(state, 1) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    const char* param1 = gameplay::ScriptUtil::getString(1, false);

                    void* returnPtr = ((void*)Texture::create(param1));
                    if (returnPtr)
                    {
                        gameplay::ScriptUtil::LuaObject* object = (gameplay::ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(gameplay::ScriptUtil::LuaObject));
                        object->instance = returnPtr;
                        object->owns = true;
                        luaL_getmetatable(state, "Texture");
                        lua_setmetatable(state, -2);
                    }
                    else
                    {
                        lua_pushnil(state);
                    }

                    return 1;
                }
            } while (0);

            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA || lua_type(state, 1) == LUA_TTABLE || lua_type(state, 1) == LUA_TNIL))
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    gameplay::ScriptUtil::LuaArray<Image> param1 = gameplay::ScriptUtil::getObjectPointer<Image>(1, "Image", false, &param1Valid);
                    if (!param1Valid)
                        break;

                    void* returnPtr = ((void*)Texture::create(param1));
                    if (returnPtr)
                    {
                        gameplay::ScriptUtil::LuaObject* object = (gameplay::ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(gameplay::ScriptUtil::LuaObject));
                        object->instance = returnPtr;
                        object->owns = true;
                        luaL_getmetatable(state, "Texture");
                        lua_setmetatable(state, -2);
                    }
                    else
                    {
                        lua_pushnil(state);
                    }

                    return 1;
                }
            } while (0);

            lua_pushstring(state, "lua_Texture_static_create - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 2:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TSTRING || lua_type(state, 1) == LUA_TNIL) &&
                    lua_type(state, 2) == LUA_TBOOLEAN)
                {
                    // Get parameter 1 off the stack.
                    const char* param1 = gameplay::ScriptUtil::getString(1, false);

                    // Get parameter 2 off the stack.
                    bool param2 = gameplay::ScriptUtil::luaCheckBool(state, 2);

                    void* returnPtr = ((void*)Texture::create(param1, param2));
                    if (returnPtr)
                    {
                        gameplay::ScriptUtil::LuaObject* object = (gameplay::ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(gameplay::ScriptUtil::LuaObject));
                        object->instance = returnPtr;
                        object->owns = true;
                        luaL_getmetatable(state, "Texture");
                        lua_setmetatable(state, -2);
                    }
                    else
                    {
                        lua_pushnil(state);
                    }

                    return 1;
                }
            } while (0);

            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA || lua_type(state, 1) == LUA_TTABLE || lua_type(state, 1) == LUA_TNIL) &&
                    lua_type(state, 2) == LUA_TBOOLEAN)
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    gameplay::ScriptUtil::LuaArray<Image> param1 = gameplay::ScriptUtil::getObjectPointer<Image>(1, "Image", false, &param1Valid);
                    if (!param1Valid)
                        break;

                    // Get parameter 2 off the stack.
                    bool param2 = gameplay::ScriptUtil::luaCheckBool(state, 2);

                    void* returnPtr = ((void*)Texture::create(param1, param2));
                    if (returnPtr)
                    {
                        gameplay::ScriptUtil::LuaObject* object = (gameplay::ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(gameplay::ScriptUtil::LuaObject));
                        object->instance = returnPtr;
                        object->owns = true;
                        luaL_getmetatable(state, "Texture");
                        lua_setmetatable(state, -2);
                    }
                    else
                    {
                        lua_pushnil(state);
                    }

                    return 1;
                }
            } while (0);

            lua_pushstring(state, "lua_Texture_static_create - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 3:
        {
            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA || lua_type(state, 1) == LUA_TNIL) &&
                    lua_type(state, 2) == LUA_TNUMBER &&
                    lua_type(state, 3) == LUA_TNUMBER)
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    gameplay::ScriptUtil::LuaArray<GLuint> param1 = gameplay::ScriptUtil::getObjectPointer<GLuint>(1, "GLuint", true, &param1Valid);
                    if (!param1Valid)
                        break;

                    // Get parameter 2 off the stack.
                    int param2 = (int)luaL_checkint(state, 2);

                    // Get parameter 3 off the stack.
                    int param3 = (int)luaL_checkint(state, 3);

                    void* returnPtr = ((void*)Texture::create(*param1, param2, param3));
                    if (returnPtr)
                    {
                        gameplay::ScriptUtil::LuaObject* object = (gameplay::ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(gameplay::ScriptUtil::LuaObject));
                        object->instance = returnPtr;
                        object->owns = true;
                        luaL_getmetatable(state, "Texture");
                        lua_setmetatable(state, -2);
                    }
                    else
                    {
                        lua_pushnil(state);
                    }

                    return 1;
                }
            } while (0);

            lua_pushstring(state, "lua_Texture_static_create - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 4:
        {
            do
            {
                if (lua_type(state, 1) == LUA_TNUMBER &&
                    lua_type(state, 2) == LUA_TNUMBER &&
                    lua_type(state, 3) == LUA_TNUMBER &&
                    (lua_type(state, 4) == LUA_TTABLE || lua_type(state, 4) == LUA_TLIGHTUSERDATA))
                {
                    // Get parameter 1 off the stack.
                    Texture::Format param1 = (Texture::Format)luaL_checkint(state, 1);

                    // Get parameter 2 off the stack.
                    unsigned int param2 = (unsigned int)luaL_checkunsigned(state, 2);

                    // Get parameter 3 off the stack.
                    unsigned int param3 = (unsigned int)luaL_checkunsigned(state, 3);

                    // Get parameter 4 off the stack.
                    gameplay::ScriptUtil::LuaArray<unsigned char> param4 = gameplay::ScriptUtil::getUnsignedCharPointer(4);

                    void* returnPtr = ((void*)Texture::create(param1, param2, param3, param4));
                    if (returnPtr)
                    {
                        gameplay::ScriptUtil::LuaObject* object = (gameplay::ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(gameplay::ScriptUtil::LuaObject));
                        object->instance = returnPtr;
                        object->owns = true;
                        luaL_getmetatable(state, "Texture");
                        lua_setmetatable(state, -2);
                    }
                    else
                    {
                        lua_pushnil(state);
                    }

                    return 1;
                }
            } while (0);

            do
            {
                if ((lua_type(state, 1) == LUA_TUSERDATA || lua_type(state, 1) == LUA_TNIL) &&
                    lua_type(state, 2) == LUA_TNUMBER &&
                    lua_type(state, 3) == LUA_TNUMBER &&
                    lua_type(state, 4) == LUA_TNUMBER)
                {
                    // Get parameter 1 off the stack.
                    bool param1Valid;
                    gameplay::ScriptUtil::LuaArray<GLuint> param1 = gameplay::ScriptUtil::getObjectPointer<GLuint>(1, "GLuint", true, &param1Valid);
                    if (!param1Valid)
                        break;

                    // Get parameter 2 off the stack.
                    int param2 = (int)luaL_checkint(state, 2);

                    // Get parameter 3 off the stack.
                    int param3 = (int)luaL_checkint(state, 3);

                    // Get parameter 4 off the stack.
                    Texture::Format param4 = (Texture::Format)luaL_checkint(state, 4);

                    void* returnPtr = ((void*)Texture::create(*param1, param2, param3, param4));
                    if (returnPtr)
                    {
                        gameplay::ScriptUtil::LuaObject* object = (gameplay::ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(gameplay::ScriptUtil::LuaObject));
                        object->instance = returnPtr;
                        object->owns = true;
                        luaL_getmetatable(state, "Texture");
                        lua_setmetatable(state, -2);
                    }
                    else
                    {
                        lua_pushnil(state);
                    }

                    return 1;
                }
            } while (0);

            lua_pushstring(state, "lua_Texture_static_create - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 5:
        {
            do
            {
                if (lua_type(state, 1) == LUA_TNUMBER &&
                    lua_type(state, 2) == LUA_TNUMBER &&
                    lua_type(state, 3) == LUA_TNUMBER &&
                    (lua_type(state, 4) == LUA_TTABLE || lua_type(state, 4) == LUA_TLIGHTUSERDATA) &&
                    lua_type(state, 5) == LUA_TBOOLEAN)
                {
                    // Get parameter 1 off the stack.
                    Texture::Format param1 = (Texture::Format)luaL_checkint(state, 1);

                    // Get parameter 2 off the stack.
                    unsigned int param2 = (unsigned int)luaL_checkunsigned(state, 2);

                    // Get parameter 3 off the stack.
                    unsigned int param3 = (unsigned int)luaL_checkunsigned(state, 3);

                    // Get parameter 4 off the stack.
                    gameplay::ScriptUtil::LuaArray<unsigned char> param4 = gameplay::ScriptUtil::getUnsignedCharPointer(4);

                    // Get parameter 5 off the stack.
                    bool param5 = gameplay::ScriptUtil::luaCheckBool(state, 5);

                    void* returnPtr = ((void*)Texture::create(param1, param2, param3, param4, param5));
                    if (returnPtr)
                    {
                        gameplay::ScriptUtil::LuaObject* object = (gameplay::ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(gameplay::ScriptUtil::LuaObject));
                        object->instance = returnPtr;
                        object->owns = true;
                        luaL_getmetatable(state, "Texture");
                        lua_setmetatable(state, -2);
                    }
                    else
                    {
                        lua_pushnil(state);
                    }

                    return 1;
                }
            } while (0);

            lua_pushstring(state, "lua_Texture_static_create - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        case 6:
        {
            do
            {
                if (lua_type(state, 1) == LUA_TNUMBER &&
                    lua_type(state, 2) == LUA_TNUMBER &&
                    lua_type(state, 3) == LUA_TNUMBER &&
                    (lua_type(state, 4) == LUA_TTABLE || lua_type(state, 4) == LUA_TLIGHTUSERDATA) &&
                    lua_type(state, 5) == LUA_TBOOLEAN &&
                    lua_type(state, 6) == LUA_TNUMBER)
                {
                    // Get parameter 1 off the stack.
                    Texture::Format param1 = (Texture::Format)luaL_checkint(state, 1);

                    // Get parameter 2 off the stack.
                    unsigned int param2 = (unsigned int)luaL_checkunsigned(state, 2);

                    // Get parameter 3 off the stack.
                    unsigned int param3 = (unsigned int)luaL_checkunsigned(state, 3);

                    // Get parameter 4 off the stack.
                    gameplay::ScriptUtil::LuaArray<unsigned char> param4 = gameplay::ScriptUtil::getUnsignedCharPointer(4);

                    // Get parameter 5 off the stack.
                    bool param5 = gameplay::ScriptUtil::luaCheckBool(state, 5);

                    // Get parameter 6 off the stack.
                    Texture::Type param6 = (Texture::Type)luaL_checkint(state, 6);

                    void* returnPtr = ((void*)Texture::create(param1, param2, param3, param4, param5, param6));
                    if (returnPtr)
                    {
                        gameplay::ScriptUtil::LuaObject* object = (gameplay::ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(gameplay::ScriptUtil::LuaObject));
                        object->instance = returnPtr;
                        object->owns = true;
                        luaL_getmetatable(state, "Texture");
                        lua_setmetatable(state, -2);
                    }
                    else
                    {
                        lua_pushnil(state);
                    }

                    return 1;
                }
            } while (0);

            lua_pushstring(state, "lua_Texture_static_create - Failed to match the given parameters to a valid function signature.");
            lua_error(state);
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 1, 2, 3, 4, 5 or 6).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

// Provides support for conversion to all known relative types of Texture
static void* __convertTo(void* ptr, const char* typeName)
{
    Texture* ptrObject = reinterpret_cast<Texture*>(ptr);

    if (strcmp(typeName, "Ref") == 0)
    {
        return reinterpret_cast<void*>(static_cast<Ref*>(ptrObject));
    }

    // No conversion available for 'typeName'
    return NULL;
}

static int lua_Texture_to(lua_State* state)
{
    // There should be only a single parameter (this instance)
    if (lua_gettop(state) != 2 || lua_type(state, 1) != LUA_TUSERDATA || lua_type(state, 2) != LUA_TSTRING)
    {
        lua_pushstring(state, "lua_Texture_to - Invalid number of parameters (expected 2).");
        lua_error(state);
        return 0;
    }

    Texture* instance = getInstance(state);
    const char* typeName = gameplay::ScriptUtil::getString(2, false);
    void* result = __convertTo((void*)instance, typeName);

    if (result)
    {
        gameplay::ScriptUtil::LuaObject* object = (gameplay::ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(gameplay::ScriptUtil::LuaObject));
        object->instance = (void*)result;
        object->owns = false;
        luaL_getmetatable(state, typeName);
        lua_setmetatable(state, -2);
    }
    else
    {
        lua_pushnil(state);
    }

    return 1;
}

void luaRegister_Texture()
{
    const luaL_Reg lua_members[] = 
    {
        {"addRef", lua_Texture_addRef},
        {"generateMipmaps", lua_Texture_generateMipmaps},
        {"getFormat", lua_Texture_getFormat},
        {"getHandle", lua_Texture_getHandle},
        {"getHeight", lua_Texture_getHeight},
        {"getPath", lua_Texture_getPath},
        {"getRefCount", lua_Texture_getRefCount},
        {"getType", lua_Texture_getType},
        {"getWidth", lua_Texture_getWidth},
        {"isCompressed", lua_Texture_isCompressed},
        {"isMipmapped", lua_Texture_isMipmapped},
        {"release", lua_Texture_release},
        {"setData", lua_Texture_setData},
        {"to", lua_Texture_to},
        {NULL, NULL}
    };
    const luaL_Reg lua_statics[] = 
    {
        {"create", lua_Texture_static_create},
        {NULL, NULL}
    };
    std::vector<std::string> scopePath;

    gameplay::ScriptUtil::registerClass("Texture", lua_members, NULL, lua_Texture__gc, lua_statics, scopePath);

    luaGlobal_Register_Conversion_Function("Texture", __convertTo);
}

}
