// Autogenerated by gameplay-luagen
#include "gameplay-Base.h"
#include "gameplay-ScriptController.h"
#include "gameplay-lua_VertexAttributeBinding.h"
#include "gameplay-Base.h"
#include "gameplay-Effect.h"
#include "gameplay-Game.h"
#include "gameplay-Mesh.h"
#include "gameplay-Ref.h"
#include "gameplay-VertexAttributeBinding.h"
#include "gameplay-Ref.h"

namespace gameplay
{

extern void luaGlobal_Register_Conversion_Function(const char* className, void*(*func)(void*, const char*));

static VertexAttributeBinding* getInstance(lua_State* state)
{
    void* userdata = luaL_checkudata(state, 1, "VertexAttributeBinding");
    luaL_argcheck(state, userdata != NULL, 1, "'VertexAttributeBinding' expected.");
    return (VertexAttributeBinding*)((gameplay::ScriptUtil::LuaObject*)userdata)->instance;
}

static int lua_VertexAttributeBinding__gc(lua_State* state)
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
                void* userdata = luaL_checkudata(state, 1, "VertexAttributeBinding");
                luaL_argcheck(state, userdata != NULL, 1, "'VertexAttributeBinding' expected.");
                gameplay::ScriptUtil::LuaObject* object = (gameplay::ScriptUtil::LuaObject*)userdata;
                if (object->owns)
                {
                    VertexAttributeBinding* instance = (VertexAttributeBinding*)object->instance;
                    SAFE_RELEASE(instance);
                }
                
                return 0;
            }

            lua_pushstring(state, "lua_VertexAttributeBinding__gc - Failed to match the given parameters to a valid function signature.");
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

static int lua_VertexAttributeBinding_addRef(lua_State* state)
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
                VertexAttributeBinding* instance = getInstance(state);
                instance->addRef();
                
                return 0;
            }

            lua_pushstring(state, "lua_VertexAttributeBinding_addRef - Failed to match the given parameters to a valid function signature.");
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

static int lua_VertexAttributeBinding_bind(lua_State* state)
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
                VertexAttributeBinding* instance = getInstance(state);
                instance->bind();
                
                return 0;
            }

            lua_pushstring(state, "lua_VertexAttributeBinding_bind - Failed to match the given parameters to a valid function signature.");
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

static int lua_VertexAttributeBinding_getRefCount(lua_State* state)
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
                VertexAttributeBinding* instance = getInstance(state);
                unsigned int result = instance->getRefCount();

                // Push the return value onto the stack.
                lua_pushunsigned(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_VertexAttributeBinding_getRefCount - Failed to match the given parameters to a valid function signature.");
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

static int lua_VertexAttributeBinding_release(lua_State* state)
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
                VertexAttributeBinding* instance = getInstance(state);
                instance->release();
                
                return 0;
            }

            lua_pushstring(state, "lua_VertexAttributeBinding_release - Failed to match the given parameters to a valid function signature.");
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

static int lua_VertexAttributeBinding_static_create(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA || lua_type(state, 1) == LUA_TTABLE || lua_type(state, 1) == LUA_TNIL) &&
                (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TTABLE || lua_type(state, 2) == LUA_TNIL))
            {
                // Get parameter 1 off the stack.
                bool param1Valid;
                gameplay::ScriptUtil::LuaArray<Mesh> param1 = gameplay::ScriptUtil::getObjectPointer<Mesh>(1, "Mesh", false, &param1Valid);
                if (!param1Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 1 to type 'Mesh'.");
                    lua_error(state);
                }

                // Get parameter 2 off the stack.
                bool param2Valid;
                gameplay::ScriptUtil::LuaArray<Effect> param2 = gameplay::ScriptUtil::getObjectPointer<Effect>(2, "Effect", false, &param2Valid);
                if (!param2Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 2 to type 'Effect'.");
                    lua_error(state);
                }

                void* returnPtr = ((void*)VertexAttributeBinding::create(param1, param2));
                if (returnPtr)
                {
                    gameplay::ScriptUtil::LuaObject* object = (gameplay::ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(gameplay::ScriptUtil::LuaObject));
                    object->instance = returnPtr;
                    object->owns = true;
                    luaL_getmetatable(state, "VertexAttributeBinding");
                    lua_setmetatable(state, -2);
                }
                else
                {
                    lua_pushnil(state);
                }

                return 1;
            }

            lua_pushstring(state, "lua_VertexAttributeBinding_static_create - Failed to match the given parameters to a valid function signature.");
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

static int lua_VertexAttributeBinding_unbind(lua_State* state)
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
                VertexAttributeBinding* instance = getInstance(state);
                instance->unbind();
                
                return 0;
            }

            lua_pushstring(state, "lua_VertexAttributeBinding_unbind - Failed to match the given parameters to a valid function signature.");
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

// Provides support for conversion to all known relative types of VertexAttributeBinding
static void* __convertTo(void* ptr, const char* typeName)
{
    VertexAttributeBinding* ptrObject = reinterpret_cast<VertexAttributeBinding*>(ptr);

    if (strcmp(typeName, "Ref") == 0)
    {
        return reinterpret_cast<void*>(static_cast<Ref*>(ptrObject));
    }

    // No conversion available for 'typeName'
    return NULL;
}

static int lua_VertexAttributeBinding_to(lua_State* state)
{
    // There should be only a single parameter (this instance)
    if (lua_gettop(state) != 2 || lua_type(state, 1) != LUA_TUSERDATA || lua_type(state, 2) != LUA_TSTRING)
    {
        lua_pushstring(state, "lua_VertexAttributeBinding_to - Invalid number of parameters (expected 2).");
        lua_error(state);
        return 0;
    }

    VertexAttributeBinding* instance = getInstance(state);
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

void luaRegister_VertexAttributeBinding()
{
    const luaL_Reg lua_members[] = 
    {
        {"addRef", lua_VertexAttributeBinding_addRef},
        {"bind", lua_VertexAttributeBinding_bind},
        {"getRefCount", lua_VertexAttributeBinding_getRefCount},
        {"release", lua_VertexAttributeBinding_release},
        {"unbind", lua_VertexAttributeBinding_unbind},
        {"to", lua_VertexAttributeBinding_to},
        {NULL, NULL}
    };
    const luaL_Reg lua_statics[] = 
    {
        {"create", lua_VertexAttributeBinding_static_create},
        {NULL, NULL}
    };
    std::vector<std::string> scopePath;

    gameplay::ScriptUtil::registerClass("VertexAttributeBinding", lua_members, NULL, lua_VertexAttributeBinding__gc, lua_statics, scopePath);

    luaGlobal_Register_Conversion_Function("VertexAttributeBinding", __convertTo);
}

}
