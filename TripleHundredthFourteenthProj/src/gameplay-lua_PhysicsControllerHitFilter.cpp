// Autogenerated by gameplay-luagen
#include "gameplay-Base.h"
#include "gameplay-ScriptController.h"
#include "gameplay-lua_PhysicsControllerHitFilter.h"
#include "gameplay-Base.h"
#include "gameplay-Bundle.h"
#include "gameplay-Game.h"
#include "gameplay-MeshPart.h"
#include "gameplay-PhysicsCharacter.h"
#include "gameplay-PhysicsController.h"
#include "gameplay-PhysicsRigidBody.h"
#include "gameplay-ScriptController.h"
#include "gameplay-ScriptTarget.h"
#include "gameplay-Terrain.h"

namespace gameplay
{

static PhysicsController::HitFilter* getInstance(lua_State* state)
{
    void* userdata = luaL_checkudata(state, 1, "PhysicsControllerHitFilter");
    luaL_argcheck(state, userdata != NULL, 1, "'PhysicsControllerHitFilter' expected.");
    return (PhysicsController::HitFilter*)((gameplay::ScriptUtil::LuaObject*)userdata)->instance;
}

static int lua_PhysicsControllerHitFilter__gc(lua_State* state)
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
                void* userdata = luaL_checkudata(state, 1, "PhysicsControllerHitFilter");
                luaL_argcheck(state, userdata != NULL, 1, "'PhysicsControllerHitFilter' expected.");
                gameplay::ScriptUtil::LuaObject* object = (gameplay::ScriptUtil::LuaObject*)userdata;
                if (object->owns)
                {
                    PhysicsController::HitFilter* instance = (PhysicsController::HitFilter*)object->instance;
                    SAFE_DELETE(instance);
                }
                
                return 0;
            }

            lua_pushstring(state, "lua_PhysicsControllerHitFilter__gc - Failed to match the given parameters to a valid function signature.");
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

static int lua_PhysicsControllerHitFilter__init(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 0:
        {
            void* returnPtr = ((void*)new PhysicsController::HitFilter());
            if (returnPtr)
            {
                gameplay::ScriptUtil::LuaObject* object = (gameplay::ScriptUtil::LuaObject*)lua_newuserdata(state, sizeof(gameplay::ScriptUtil::LuaObject));
                object->instance = returnPtr;
                object->owns = true;
                luaL_getmetatable(state, "PhysicsControllerHitFilter");
                lua_setmetatable(state, -2);
            }
            else
            {
                lua_pushnil(state);
            }

            return 1;
            break;
        }
        default:
        {
            lua_pushstring(state, "Invalid number of parameters (expected 0).");
            lua_error(state);
            break;
        }
    }
    return 0;
}

static int lua_PhysicsControllerHitFilter_filter(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TTABLE || lua_type(state, 2) == LUA_TNIL))
            {
                // Get parameter 1 off the stack.
                bool param1Valid;
                gameplay::ScriptUtil::LuaArray<PhysicsCollisionObject> param1 = gameplay::ScriptUtil::getObjectPointer<PhysicsCollisionObject>(2, "PhysicsCollisionObject", false, &param1Valid);
                if (!param1Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 1 to type 'PhysicsCollisionObject'.");
                    lua_error(state);
                }

                PhysicsController::HitFilter* instance = getInstance(state);
                bool result = instance->filter(param1);

                // Push the return value onto the stack.
                lua_pushboolean(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_PhysicsControllerHitFilter_filter - Failed to match the given parameters to a valid function signature.");
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

static int lua_PhysicsControllerHitFilter_hit(lua_State* state)
{
    // Get the number of parameters.
    int paramCount = lua_gettop(state);

    // Attempt to match the parameters to a valid binding.
    switch (paramCount)
    {
        case 2:
        {
            if ((lua_type(state, 1) == LUA_TUSERDATA) &&
                (lua_type(state, 2) == LUA_TUSERDATA || lua_type(state, 2) == LUA_TNIL))
            {
                // Get parameter 1 off the stack.
                bool param1Valid;
                gameplay::ScriptUtil::LuaArray<PhysicsController::HitResult> param1 = gameplay::ScriptUtil::getObjectPointer<PhysicsController::HitResult>(2, "PhysicsControllerHitResult", true, &param1Valid);
                if (!param1Valid)
                {
                    lua_pushstring(state, "Failed to convert parameter 1 to type 'PhysicsController::HitResult'.");
                    lua_error(state);
                }

                PhysicsController::HitFilter* instance = getInstance(state);
                bool result = instance->hit(*param1);

                // Push the return value onto the stack.
                lua_pushboolean(state, result);

                return 1;
            }

            lua_pushstring(state, "lua_PhysicsControllerHitFilter_hit - Failed to match the given parameters to a valid function signature.");
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

void luaRegister_PhysicsControllerHitFilter()
{
    const luaL_Reg lua_members[] = 
    {
        {"filter", lua_PhysicsControllerHitFilter_filter},
        {"hit", lua_PhysicsControllerHitFilter_hit},
        {NULL, NULL}
    };
    const luaL_Reg* lua_statics = NULL;
    std::vector<std::string> scopePath;
    scopePath.push_back("PhysicsController");

    gameplay::ScriptUtil::registerClass("PhysicsControllerHitFilter", lua_members, lua_PhysicsControllerHitFilter__init, lua_PhysicsControllerHitFilter__gc, lua_statics, scopePath);

}

}
