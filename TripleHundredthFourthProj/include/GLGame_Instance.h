#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLGame_GLDebug.h"

#include "GLGame_AABB.h"
#include "GLGame_Object.h"
#include "GLGame_Sprite.h"
#include "GLGame_Texture.h"

namespace GLGame
{
	class ObjectInstance
	{
	public :

		Object* m_Object; 
		string m_ObjectID; 
	};

	class SpriteInstance
	{
	public:

		Sprite* m_Sprite;
		string m_SpriteID;
	};

	class TextureInstance
	{
	public:

		Texture* m_Tex;
		uint32_t m_TexID;
	};
}