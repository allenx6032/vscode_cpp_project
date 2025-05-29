#pragma once

#include <iostream>
#include <fstream>

#include "GLGame_GLDebug.h"
#include "GLGame_Scene.h"
#include "GLGame_Sprite.h"
#include "GLGame_SceneData.h"
#include "GLGame_Macros.h"

namespace GLGame
{
	namespace GameInternal
	{
		Object* _GetObjectFromGlobalArray(const string& id);
		Sprite* _GetSpriteFromGlobalArray(const string& id);
		Background* _GetBackgroundFromGlobalArray(const string& id);
	}

	namespace SceneParser
	{
		enum SceneParsedItemType
		{
			ItemTypeObject,
			ItemTypeSprite,
			ItemTypeTex,
			ItemTypeBackground
		};

		struct SceneParsedDataItem
		{
			SceneParsedItemType type;
			string id;
			float x;
			float y;
			int sx;
			int sy;
			int layer;
		};

		static std::ifstream::pos_type GetFileSize(const string& filename);
		void ParseSceneData(const string& scene_file_pth, Scene* scene);
		void _FillInParsedSceneData(vector<SceneParsedDataItem>& items, Scene* scene);
		void _ParseSceneFileData(Scene* scene, const string& scene_file);
	}
}