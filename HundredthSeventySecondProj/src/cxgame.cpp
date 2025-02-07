#include "cxgame.h"	
#include <cxscript_system.h>
#include "cxwindow.h"
#include "cxscene_manager.h"
#include "cxbase_scene.h"
#include "cxfile_system.h"
#include "cxaction.h"
#include "cxgame_map.h"
#include "cxresource_manager.h"
#include "cxutils.h"


Game::Game()
{
	
}

Game::~Game()
{

}

Pos Game::MapPosToScreenPos(Pos mapPos)
{
	if (IsRunning())
	{
		GameMap* map = SCENE_MANAGER_INSTANCE->GetCurrentScene()->GetGameMap();
		return { mapPos.x + map->GetMapOffsetX(),mapPos.y + map->GetMapOffsetY() };
	}
	else
	{
		return mapPos;
	}
}

Pos Game::ScreenPosToMapPos(Pos screenPos)
{
	if (IsRunning())
	{
		GameMap* map = SCENE_MANAGER_INSTANCE->GetCurrentScene()->GetGameMap();
		return { screenPos.x - map->GetMapOffsetX(),screenPos.y - map->GetMapOffsetY() };
	}
	else
	{
		return screenPos;
	}
}

bool Game::IsRunning()
{
	auto* curScene = SCENE_MANAGER_INSTANCE->GetCurrentScene();
	return curScene->GetGameMap() != nullptr;
}

int util_screen_pos_to_map_pos(lua_State* L) {
	float mx = (float)lua_tonumber(L, 1);
	float my = (float)lua_tonumber(L, 2);
	Pos pos = GAME_INSTANCE->ScreenPosToMapPos(Pos(mx, my));
	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);
	return 2;
}


 
void util_save_image_file(const char* filename, int width, int height, int pixelDepth, char* data)
{
	TGA_FILE_HEADER TgaHeader;
	memset(&TgaHeader, 0, 18);
	TgaHeader.IdLength = 0;
	TgaHeader.ColorMapType = 0;
	TgaHeader.ImageType = 0x02;
	TgaHeader.ColorMapFirstIndex = 0;
	TgaHeader.ColorMapLength = 0;
	TgaHeader.ColorMapEntrySize = 0;
	TgaHeader.XOrigin = 0;
	TgaHeader.YOrigin = 0;
	TgaHeader.ImageWidth = width;
	TgaHeader.ImageHeight = height;
	TgaHeader.PixelDepth = pixelDepth;
	TgaHeader.ImageDescruptor = 8;

	std::fstream ofile;
	ofile.open(filename, std::ios::out | std::ios::trunc | std::ios::binary);
	ofile.write((char*)(&TgaHeader), sizeof(TGA_FILE_HEADER));
	ofile.write((char*)data, width*height*pixelDepth / 8);
	ofile.close();
}

void luaopen_game(lua_State* L) {
	script_system_register_luac_function(L, util_screen_pos_to_map_pos);
}