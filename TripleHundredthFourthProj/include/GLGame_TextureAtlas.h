#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

#include "GLGame_GLDebug.h"
#include "GLGame_Texture.h"
#include "GLGame_Log.h"

namespace GLGame
{
	using namespace std;

	class TextureAtlas
	{
	public : 

		TextureAtlas(Texture* atlas_texture, int tx, int ty)
		{
			m_Atlas = atlas_texture;
			m_TileX = tx;
			m_TileY = ty;
		}

		TextureAtlas(const string& atlas_path, int tx, int ty)
		{
			m_Atlas = new Texture(atlas_path);
			m_TileX = tx;
			m_TileY = ty;
		}

		Texture* Sample(const glm::vec2& start_coords, const glm::vec2& end_coords)
		{
			float width, height;
			float x2, y2;
			float x1, y1;

			width = m_TileX * (end_coords.x - start_coords.x);
			height = m_TileY * (end_coords.y - start_coords.y);

			array<GLfloat, 8> TextureCoordinates;

			x1 = start_coords.x * m_TileX;
			y1 = start_coords.y * m_TileY;
			x2 = end_coords.x * m_TileX;
			y2 = end_coords.y * m_TileY;

			x1 = x1 / m_Atlas->GetWidth();
			y1 = y1 / m_Atlas->GetHeight();
			x2 = x2 / m_Atlas->GetWidth();
			y2 = y2 / m_Atlas->GetHeight();

			TextureCoordinates[0] = x2;
			TextureCoordinates[1] = y2;
			TextureCoordinates[2] = x2;
			TextureCoordinates[3] = y1;
			TextureCoordinates[4] = x1;
			TextureCoordinates[5] = y1;
			TextureCoordinates[6] = x1;
			TextureCoordinates[7] = y2;

			Texture *tex = new Texture;

			*tex = *m_Atlas;
			tex->IntCreateTexture(m_Atlas->GetTextureID(), TextureCoordinates, width, height, false);

			return tex;
		}

		Texture* SampleCustom(const glm::vec2& start_coords, const glm::vec2& end_coords)
		{
			float width, height;
			float x2, y2;
			float x1, y1;

			width = end_coords.x - start_coords.x;
			height = end_coords.y - start_coords.y;

			array<GLfloat, 8> TextureCoordinates;

			x1 = start_coords.x;
			y1 = start_coords.y;
			x2 = end_coords.x;
			y2 = end_coords.y;

			x1 = x1 / m_Atlas->GetWidth();
			y1 = y1 / m_Atlas->GetHeight();
			x2 = x2 / m_Atlas->GetWidth();
			y2 = y2 / m_Atlas->GetHeight();

			TextureCoordinates[0] = x2;
			TextureCoordinates[1] = y2;
			TextureCoordinates[2] = x2;
			TextureCoordinates[3] = y1;
			TextureCoordinates[4] = x1;
			TextureCoordinates[5] = y1;
			TextureCoordinates[6] = x1;
			TextureCoordinates[7] = y2;

			Texture* tex = new Texture;

			*tex = *m_Atlas;
			tex->IntCreateTexture(m_Atlas->GetTextureID(), TextureCoordinates, width, height, false);

			return tex;
		}

	private : 

		int m_TileX, m_TileY;
		Texture* m_Atlas;
	};
}