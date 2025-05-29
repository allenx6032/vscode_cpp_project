#pragma once

#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLGame_Structures.h"
#include "GLGame_IndexBuffer.h"
#include "GLGame_VertexArray.h"
#include "GLGame_VertexBuffer.h"
#include "GLGame_Texture.h"
#include "GLGame_Shader.h"
#include "GLGame_GLDebug.h"
#include "GLGame_Macros.h"
#include "GLGame_Lighting.h"

namespace GLGame
{
	class LightBatcher
	{
	public :

		LightBatcher();
		~LightBatcher();
		void AddLightToBatch(Light light);
		void StartLightBatch(const glm::mat4& vp_matrix);

		// Returns the amount of lights that have been drawn
		BatcherInfo& EndLightBatch();

	private : 

		void DrawFullBatch();

		int m_CurrentElement;
		unsigned int m_VerticesWritten = 0;
		const int m_MaxLights;
		bool m_ObjectsInitialized = false;

		array<GLfloat, 8> m_UVCoords = { -1.0f, 2.0f, -1.0f, -1.0f, 2.0f, -1.0f, 2.0f, 2.0f };

		// Vertex and index data
		GLfloat* m_VertexBuffer;
		GLuint* m_IndexBuffer;

		// OpenGL Abstracted objects
		VertexBuffer m_VBO;
		VertexArray m_VAO;
		IndexBuffer m_IBO;
		Shader m_Shader;

		BatcherInfo m_Info;

		glm::mat4 m_VPMatrix = glm::mat4(1.0f);
	};

	// This function is INCREDIBLY slow. It was originally only used to test my lighting concept. Please use LightBatcher as it much more fast and reliable
	void DrawLight(Light light);

}