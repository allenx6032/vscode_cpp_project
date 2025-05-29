#pragma once

#include "GLGame_Macros.h"
#include "GLGame_IndexBuffer.h"
#include "GLGame_VertexArray.h"
#include "GLGame_VertexBuffer.h"
#include "GLGame_Texture.h"
#include "GLGame_Shader.h"

namespace GLGame
{
	class ParticleBatcher
	{
	public:

		ParticleBatcher();
		~ParticleBatcher();
		void AddParticleToBatch(const glm::mat4& transform, const glm::vec4& color);
		void StartParticleBatch(const glm::mat4& vp_matrix);

		// Returns the amount of quads that have been drawn
		unsigned int EndParticleBatch();

	private:

		// Returns the amount of quads that have been drawn
		unsigned int DrawFullBatch();

		int m_CurrentElement;
		int m_VerticesWritten;
		bool m_Initialized = false;
		const int m_MaxParticles;

		array<GLfloat, 8> m_UVCoords = { -1.0f, 2.0f, -1.0f, -1.0f, 2.0f, -1.0f, 2.0f, 2.0f };

		// Vertex and index data
		GLfloat* m_VertexBuffer;
		GLuint* m_IndexBuffer;

		// OpenGL Abstracted objects
		VertexBuffer* m_VBO;
		VertexArray m_VAO;
		IndexBuffer m_IBO;
		Shader m_Shader;

		glm::mat4 m_VPMatrix = glm::mat4(1.0f);
	};
}