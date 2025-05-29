#pragma once

#define _CRT_SECURE_NO_WARNINGS 
#define GLEW_STATIC

#include <unordered_map>
#include <map>
#include <vector>
#include <string>
#include <thread>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GLEW
#include <GL/glew.h> 

#include <GLFW/glfw3.h>

#include "GLGame_GLDebug.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "GLGame_Structures.h"
#include "GLGame_Log.h"
#include "GLGame_Macros.h"
#include "GLGame_AABB.h"
#include "GLGame_Texture.h"
#include "GLGame_BatchRendering.h"
#include "GLGame_Rendering.h"
#include "GLGame_Object.h"
#include "GLGame_Sprite.h"
#include "GLGame_SceneData.h"
#include "GLGame_SceneFileParser.h"

namespace GLGame
{
	namespace SceneEditor
	{
		using namespace std;
		
		GLFWwindow* InitSceneEditor(GameDebugInfo* debug_info, unordered_map<string, Object*>* global_objects, unordered_map<string, Sprite*>* global_sprites, unordered_map<string, Background*>* global_bgs, vector<string>* objid_list, vector<string>* sprid_list, vector<string>* bgid_list, GLFWwindow* window, ImGuiContext* context);
		void _SetSceneEditorCloseFlag(bool val);
		bool RenderSceneEditor();
		bool SceneEditorAlive();
	}
}