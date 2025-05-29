#pragma once

#include <vector>
#include <iostream>

#include "GLGame_GLDebug.h"

#include "GLGame_Texture.h"
#include "GLGame_Shader.h"

namespace GLGame
{
	class Animation
	{
	public : 
		Animation(vector<Texture*> Textures);
		Animation(vector<std::string> TexturePaths);
		pair<int, Texture*> GetFrame(int curr_frame); 
		void AddFrame(Texture& Tex);

	private :

		vector<Texture*> m_Textures;    
	};

}