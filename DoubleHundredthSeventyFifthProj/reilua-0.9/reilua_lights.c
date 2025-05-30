#include "reilua_main.h"
#include "reilua_state.h"
#include "reilua_lua_core.h"
#include "reilua_core.h"
#include "reilua_lights.h"

#define RLIGHTS_IMPLEMENTATION
#include "reilua_rlights.h"

/*
## Lights - Light management functions
*/

/*
> light = RL.CreateLight( int type, Vector3 position, Vector3 target, Color color, Shader shader )

Create a light and get shader locations

- Success return Light
*/
int llightsCreateLight( lua_State* L ) {
	int type = luaL_checkinteger( L, 1 );
	Vector3 position = uluaGetVector3( L, 2 );
	Vector3 target = uluaGetVector3( L, 3 );
	Color color = uluaGetColor( L, 4 );
	Shader* shader = uluaGetShader( L, 5 );

	uluaPushLight( L, CreateLight( type, position, target, color, *shader ) );

	return 1;
}

/*
> RL.UpdateLightValues( Shader shader, Light light )

Send light properties to shader
*/
int llightsUpdateLightValues( lua_State* L ) {
	Shader* shader = uluaGetShader( L, 1 );
	Light* light = uluaGetLight( L, 2 );

	UpdateLightValues( *shader, *light );

	return 0;
}

/*
> RL.SetLightType( Light light, int type )

Set light type
*/
int llightsSetLightType( lua_State* L ) {
	Light* light = uluaGetLight( L, 1 );
	int type = luaL_checkinteger( L, 2 );

	light->type = type;

	return 0;
}

/*
> RL.SetLightPosition( Light light, Vector3 position )

Set light position
*/
int llightsSetLightPosition( lua_State* L ) {
	Light* light = uluaGetLight( L, 1 );
	Vector3 position = uluaGetVector3( L, 2 );

	light->position = position;

	return 0;
}

/*
> RL.SetLightTarget( Light light, Vector3 target )

Set light target
*/
int llightsSetLightTarget( lua_State* L ) {
	Light* light = uluaGetLight( L, 1 );
	Vector3 target = uluaGetVector3( L, 2 );

	light->target = target;

	return 0;
}

/*
> RL.SetLightColor( Light light, Color color )

Set light color
*/
int llightsSetLightColor( lua_State* L ) {
	Light* light = uluaGetLight( L, 1 );
	Color color = uluaGetColor( L, 2 );

	light->color = color;

	return 0;
}

/*
> RL.SetLightEnabled( Light light, bool enabled )

Set light enabled
*/
int llightsSetLightEnabled( lua_State* L ) {
	Light* light = uluaGetLight( L, 1 );
	bool enabled = uluaGetBoolean( L, 2 );

	light->enabled = enabled;

	return 0;
}

/*
> type = RL.GetLightType( Light light )

Get light type

- Success return int
*/
int llightsGetLightType( lua_State* L ) {
	Light* light = uluaGetLight( L, 1 );

	lua_pushinteger( L, light->type );

	return 1;
}

/*
> position = RL.GetLightPosition( Light light )

Get light position

- Success return Vector3
*/
int llightsGetLightPosition( lua_State* L ) {
	Light* light = uluaGetLight( L, 1 );

	uluaPushVector3( L, light->position );

	return 1;
}

/*
> target = RL.GetLightTarget( Light light )

Get light target

- Success return Vector3
*/
int llightsGetLightTarget( lua_State* L ) {
	Light* light = uluaGetLight( L, 1 );

	uluaPushVector3( L, light->target );

	return 1;
}

/*
> color = RL.GetLightColor( Light light )

Get light color

- Success return Color
*/
int llightsGetLightColor( lua_State* L ) {
	Light* light = uluaGetLight( L, 1 );

	uluaPushColor( L, light->color );

	return 1;
}

/*
> enabled = RL.IsLightEnabled( Light light )

Get light enabled

- Success return bool
*/
int llightsIsLightEnabled( lua_State* L ) {
	Light* light = uluaGetLight( L, 1 );

	lua_pushboolean( L, light->enabled );

	return 1;
}
