/**
* Copyright (c) 2006-2010 LOVE Development Team
* 
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgment in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
**/

#include "love_gl_wrap_ParticleSystem.h"

#include <cstring>

namespace love
{
namespace graphics
{
namespace opengl
{
	ParticleSystem * luax_checkparticlesystem(lua_State * L, int idx)
	{
		return luax_checktype<ParticleSystem>(L, idx, "ParticleSystem", GRAPHICS_PARTICLE_SYSTEM_T);
	}

	int w_ParticleSystem_setSprite(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		Image * i = luax_checkimage(L, 2);
		t->setSprite(i);
		return 0;
	}

	int w_ParticleSystem_setBufferSize(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		int arg1 = luaL_checkint(L, 2);
		t->setBufferSize((unsigned int)arg1);
		return 0;
	}

	int w_ParticleSystem_setEmissionRate(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		int arg1 = luaL_checkint(L, 2);
		t->setEmissionRate((unsigned int)arg1);
		return 0;
	}

	int w_ParticleSystem_setLifetime(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		float arg1 = (float)luaL_checknumber(L, 2);
		t->setLifetime(arg1);
		return 0;
	}

	int w_ParticleSystem_setParticleLife(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		float arg1 = (float)luaL_checknumber(L, 2);
		float arg2 = (float)luaL_optnumber(L, 3, arg1);
		t->setParticleLife(arg1, arg2);
		return 0;
	}

	int w_ParticleSystem_setPosition(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		float arg1 = (float)luaL_checknumber(L, 2);
		float arg2 = (float)luaL_checknumber(L, 3);
		t->setPosition(arg1, arg2);
		return 0;
	}

	int w_ParticleSystem_setDirection(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		float arg1 = (float)luaL_checknumber(L, 2);
		t->setDirection(arg1);
		return 0;
	}

	int w_ParticleSystem_setSpread(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		float arg1 = (float)luaL_checknumber(L, 2);
		t->setSpread(arg1);
		return 0;
	}

	int w_ParticleSystem_setRelativeDirection(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		bool arg1 = (bool)luax_toboolean(L, 2);
		t->setRelativeDirection(arg1);
		return 0;
	}

	int w_ParticleSystem_setSpeed(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		float arg1 = (float)luaL_checknumber(L, 2);
		float arg2 = (float)luaL_optnumber(L, 3, arg1);
		t->setSpeed(arg1, arg2);
		return 0;
	}

	int w_ParticleSystem_setGravity(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		float arg1 = (float)luaL_checknumber(L, 2);
		float arg2 = (float)luaL_optnumber(L, 3, arg1);
		t->setGravity(arg1, arg2);
		return 0;
	}

	int w_ParticleSystem_setRadialAcceleration(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		float arg1 = (float)luaL_checknumber(L, 2);
		float arg2 = (float)luaL_optnumber(L, 3, arg1);
		t->setRadialAcceleration(arg1, arg2);
		return 0;
	}

	int w_ParticleSystem_setTangentialAcceleration(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		float arg1 = (float)luaL_checknumber(L, 2);
		float arg2 = (float)luaL_optnumber(L, 3, arg1);
		t->setTangentialAcceleration(arg1, arg2);
		return 0;
	}

	int w_ParticleSystem_setSize(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		float arg1 = (float)luaL_checknumber(L, 2);
		float arg2 = (float)luaL_optnumber(L, 3, arg1);
		float arg3 = (float)luaL_optnumber(L, 3, 0);
		t->setSize(arg1, arg2, arg3);
		return 0;
	}

	int w_ParticleSystem_setSizeVariation(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		float arg1 = (float)luaL_checknumber(L, 2);
		t->setSizeVariation(arg1);
		return 0;
	}

	int w_ParticleSystem_setRotation(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		float arg1 = (float)luaL_checknumber(L, 2);
		float arg2 = (float)luaL_optnumber(L, 3, arg1);
		t->setRotation(arg1, arg2);
		return 0;
	}

	int w_ParticleSystem_setSpin(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		float arg1 = (float)luaL_checknumber(L, 2);
		float arg2 = (float)luaL_optnumber(L, 3, arg1);
		float arg3 = (float)luaL_optnumber(L, 3, 0);
		t->setSpin(arg1, arg2, arg3);
		return 0;
	}

	int w_ParticleSystem_setSpinVariation(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		float arg1 = (float)luaL_checknumber(L, 2);
		t->setSpinVariation(arg1);
		return 0;
	}

	int w_ParticleSystem_setColor(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		
		unsigned char start[4];

		start[0] = (unsigned char)luaL_checkint(L, 2);
		start[1] = (unsigned char)luaL_checkint(L, 3);
		start[2] = (unsigned char)luaL_checkint(L, 4);
		start[3] = (unsigned char)luaL_checkint(L, 5);

		if(lua_gettop(L) > 5)
		{
			unsigned char end[4];
			end[0] = (unsigned char)luaL_checkint(L, 6);
			end[1] = (unsigned char)luaL_checkint(L, 7);
			end[2] = (unsigned char)luaL_checkint(L, 8);
			end[3] = (unsigned char)luaL_checkint(L, 9);
			t->setColor(start, end);
		}
		else
			t->setColor(start);

		return 0;
	}
	
	int w_ParticleSystem_setOffset(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		float x = (float)luaL_checknumber(L, 2);
		float y = (float)luaL_checknumber(L, 3);
		t->setOffset(x, y);
		return 0;
	}

	int w_ParticleSystem_getX(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		lua_pushnumber(L, t->getX());
		return 1;
	}

	int w_ParticleSystem_getY(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		lua_pushnumber(L, t->getY());
		return 1;
	}

	int w_ParticleSystem_getDirection(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		lua_pushnumber(L, t->getDirection());
		return 1;
	}

	int w_ParticleSystem_getSpread(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		lua_pushnumber(L, t->getSpread());
		return 1;
	}
	
	int w_ParticleSystem_getOffsetX(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		lua_pushnumber(L, t->getOffsetX());
		return 1;
	}
	
	int w_ParticleSystem_getOffsetY(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		lua_pushnumber(L, t->getOffsetY());
		return 1;
	}
	
	int w_ParticleSystem_count(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		lua_pushnumber(L, t->count());
		return 1;
	}

	int w_ParticleSystem_start(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		t->start();
		return 0;
	}

	int w_ParticleSystem_stop(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		t->stop();
		return 0;
	}

	int w_ParticleSystem_pause(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		t->pause();
		return 0;
	}

	int w_ParticleSystem_reset(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		t->reset();
		return 0;
	}

	int w_ParticleSystem_isActive(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		luax_pushboolean(L, t->isActive());
		return 1;
	}

	int w_ParticleSystem_isEmpty(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		luax_pushboolean(L, t->isEmpty());
		return 1;
	}

	int w_ParticleSystem_isFull(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		luax_pushboolean(L, t->isFull());
		return 1;
	}

	int w_ParticleSystem_update(lua_State * L)
	{
		ParticleSystem * t = luax_checkparticlesystem(L, 1);
		float dt = (float)luaL_checknumber(L, 2);
		t->update(dt);
		return 0;
	}

	static const luaL_Reg functions[] = {
		{ "setSprite", w_ParticleSystem_setSprite },
		{ "setBufferSize", w_ParticleSystem_setBufferSize },
		{ "setEmissionRate", w_ParticleSystem_setEmissionRate },
		{ "setLifetime", w_ParticleSystem_setLifetime },
		{ "setParticleLife", w_ParticleSystem_setParticleLife },
		{ "setPosition", w_ParticleSystem_setPosition },
		{ "setDirection", w_ParticleSystem_setDirection },
		{ "setSpread", w_ParticleSystem_setSpread },
		{ "setRelativeDirection", w_ParticleSystem_setRelativeDirection },
		{ "setSpeed", w_ParticleSystem_setSpeed },
		{ "setGravity", w_ParticleSystem_setGravity },
		{ "setRadialAcceleration", w_ParticleSystem_setRadialAcceleration },
		{ "setTangentialAcceleration", w_ParticleSystem_setTangentialAcceleration },
		{ "setSize", w_ParticleSystem_setSize },
		{ "setSizeVariation", w_ParticleSystem_setSizeVariation },
		{ "setRotation", w_ParticleSystem_setRotation },
		{ "setSpin", w_ParticleSystem_setSpin },
		{ "setSpinVariation", w_ParticleSystem_setSpinVariation },
		{ "setColor", w_ParticleSystem_setColor },
		{ "setOffset", w_ParticleSystem_setOffset },
		{ "getX", w_ParticleSystem_getX },
		{ "getY", w_ParticleSystem_getY },
		{ "getDirection", w_ParticleSystem_getDirection },
		{ "getSpread", w_ParticleSystem_getSpread },
		{ "getOffsetX", w_ParticleSystem_getOffsetX },
		{ "getOffsetY", w_ParticleSystem_getOffsetY },
		{ "count", w_ParticleSystem_count },
		{ "start", w_ParticleSystem_start },
		{ "stop", w_ParticleSystem_stop },
		{ "pause", w_ParticleSystem_pause },
		{ "reset", w_ParticleSystem_reset },
		{ "isActive", w_ParticleSystem_isActive },
		{ "isEmpty", w_ParticleSystem_isEmpty },
		{ "isFull", w_ParticleSystem_isFull },
		{ "update", w_ParticleSystem_update },
		{ 0, 0 }
	};

	int luaopen_particlesystem(lua_State * L)
	{
		return luax_register_type(L, "ParticleSystem", functions);
	}

} // opengl
} // graphics
} // love
