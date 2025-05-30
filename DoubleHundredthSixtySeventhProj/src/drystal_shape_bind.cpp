#include <cassert>
#include <lua.hpp>

#include "drystal_macro.h"

BEGIN_DISABLE_WARNINGS
DISABLE_WARNING_EFFCPP
DISABLE_WARNING_STRICT_ALIASING
#include <b2Box2D.h>
END_DISABLE_WARNINGS

#include "drystal_shape_bind.hpp"
#include "drystal_world_bind.hpp"
#include "drystal_log.h"
#include "drystal_lua_util.h"
#include "drystal_util.h"

log_category("shape");

IMPLEMENT_PUSHPOP(Shape, shape)

int mlua_new_shape(lua_State* L)
{
	assert(L);

	const char* type = luaL_checkstring(L, 1);

	b2FixtureDef* fixtureDef = new b2FixtureDef;
	fixtureDef->density = 1.0f;

	if (streq(type, "box")) {
		b2PolygonShape* polygon = new b2PolygonShape;
		lua_Number w = luaL_checknumber(L, 2) / 2 / pixels_per_meter;
		lua_Number h = luaL_checknumber(L, 3) / 2 / pixels_per_meter;
		lua_Number centerx = 0;
		lua_Number centery = 0;
		if (lua_gettop(L) > 3) {
			centerx = luaL_checknumber(L, 4) / pixels_per_meter;
			centery = luaL_checknumber(L, 5) / pixels_per_meter;
		}
		polygon->SetAsBox(w, h, b2Vec2(centerx + w, centery + h), 0);
		fixtureDef->shape = polygon;
	} else if (streq(type, "circle")) {
		b2CircleShape* circle = new b2CircleShape;
		circle->m_radius = luaL_checknumber(L, 2) / pixels_per_meter;
		if (lua_gettop(L) > 2) {
			lua_Number dx = luaL_checknumber(L, 3) / pixels_per_meter;
			lua_Number dy = luaL_checknumber(L, 4) / pixels_per_meter;
			circle->m_p.Set(dx, dy);
		}
		fixtureDef->shape = circle;
	} else if (streq(type, "chain")) {
		b2ChainShape* chain = new b2ChainShape;
		int number = (lua_gettop(L) - 1) / 2;
		b2Vec2* vecs = new b2Vec2[number];
		for (int i = 0; i < number; i++) {
			vecs[i].x = luaL_checknumber(L, (i + 1) * 2) / pixels_per_meter;
			vecs[i].y = luaL_checknumber(L, (i + 1) * 2 + 1) / pixels_per_meter;
		}
		chain->CreateLoop(vecs, number);
		delete[] vecs;
		fixtureDef->shape = chain;
	} else {
		assert(false);
		return 0;
	}

	Shape* shape = new Shape;
	shape->fixtureDef = fixtureDef;
	shape->ref = 0;
	push_shape(L, shape);
	return 1;
}

#define SHAPE_GETSET_SOME_VALUE(value) \
	int mlua_set_##value##_shape(lua_State* L) \
	{ \
		b2FixtureDef* fixtureDef = pop_shape(L, 1)->fixtureDef; \
		lua_Number value = luaL_checknumber(L, 2); \
		fixtureDef->value = value; \
		return 0; \
	} \
	int mlua_get_##value##_shape(lua_State* L) \
	{ \
		b2FixtureDef* fixtureDef = pop_shape(L, 1)->fixtureDef; \
		lua_pushnumber(L, fixtureDef->value); \
		return 1; \
	}
SHAPE_GETSET_SOME_VALUE(density)
SHAPE_GETSET_SOME_VALUE(restitution)
SHAPE_GETSET_SOME_VALUE(friction)

int mlua_set_sensor_shape(lua_State* L)
{
	assert(L);

	b2FixtureDef* fixtureDef = pop_shape(L, 1)->fixtureDef;
	bool sensor = lua_toboolean(L, 2);
	fixtureDef->isSensor = sensor;
	return 0;
}

int mlua_gc_shape(lua_State* L)
{
	assert(L);
	log_debug();

	Shape* shape = pop_shape(L, 1);
	b2FixtureDef* fixtureDef = shape->fixtureDef;
	delete fixtureDef->shape;
	delete fixtureDef;
	delete shape;
	return 0;
}

