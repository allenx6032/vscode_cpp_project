#pragma once

#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <array>
#include <string>

#include "GLGame_Structures.h"
#include "GLGame_GLDebug.h"
#include "imgui.h"
#include "GLGame_AABB.h"
#include "GLGame_Background.h"
#include "GLGame_Instance.h"
#include "GLGame_SceneData.h"
#include "GLGame_IndexBuffer.h"
#include "GLGame_VertexArray.h"
#include "GLGame_VertexBuffer.h"
#include "GLGame_Texture.h"
#include "GLGame_Shader.h"
#include "GLGame_Log.h"

namespace GLGame
{
	struct GenericObject
	{
		GLfloat coords[12];
		GLfloat tex_coords[8] = { 1.0f, 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
		glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		Texture* texture = nullptr;
	};

	// Reliable batch renderer. Can render upto 10000 quads

	class SpriteBatcher
	{
	public : 

		SpriteBatcher();
		~SpriteBatcher();
		void StartSpriteBatch(Camera* scene_camera, const glm::vec4& ambient_light, Shader* custom_shader = nullptr);
		void StartSpriteBatch(const glm::mat4& view_projection_matrix);
		void StartSpriteBatch();

		// Returns the amount of vertices that were written
		BatcherInfo& EndSpriteBatch();
		void AddGenericTextureToBatch(Texture* texture, const glm::vec3 &pos, const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		void AddGenericTextureToBatchCustom(Texture* texture, const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		void AddGenericObjectToBatch(GenericObject object);
		void AddGLGameItemToBatch(SceneDataItem item, const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	private : 

		void DrawFullBatch();

		glm::mat4 m_ViewProjectionMatrix;
		AABB m_CameraCull;
		glm::vec4 m_AmbientLight;

		BatcherInfo m_Info;

		int m_MaximumTextureSlots;
		const int m_MaximumQuads;
		const int m_VertexSize;
		int m_LastElementTex;
		int m_LastElementVBuff;
		unsigned int m_VerticesWritten;
		int m_SamplerArray[32];
		GLfloat *m_VertexBuffer;
		GLuint *m_IndexBuffer;
		int m_SlottedTextures[32];
		int m_CurrSlottedTexElement = 0;
		bool m_CameraCullGiven = false;
		bool m_ObjectsInitialized = false;

		// OpenGL Specific objects
		VertexBuffer m_VBO;
		VertexArray m_VAO;
		IndexBuffer m_IBO;
		Shader m_DefaultShader;
		Shader* m_BatchShader = nullptr;

		glm::vec3 m_CameraPos;
	};

	// Quad Batch Renderer

	//class QuadBatcher
	//{
	//public:

	//	QuadBatcher();
	//	~QuadBatcher();
	//	void StartQuadBatch(glm::mat4& view_projection);
	//	void AddQuadToBatch(const glm::vec4& position, const glm::vec4 color, const glm::mat4 model_matrix = glm::mat4(1.0f));
	//	void EndQuadBatch();

	//private:

	//	void DrawFullBatch();

	//	glm::mat4 m_ViewProjectionMatrix;

	//	const int m_MaximumQuads;
	//	int m_LastElementTex;
	//	int m_LastElementVBuff;
	//	unsigned int m_VerticesWritten;
	//	
	//	GLfloat* m_VertexBuffer;
	//	GLuint* m_IndexBuffer;

	//	// OpenGL Specific objects
	//	VertexBuffer m_VBO;
	//	VertexArray m_VAO;
	//	IndexBuffer m_IBO;
	//	Shader m_Shader;

	//	Camera* m_Camera;
	//};

	// This is new type of batch rendering that I wanted to experiment with
	// Do not use these functions. Use SpriteBatch instead
	void BatchRenderGenericObjects(vector <GenericObject> &objects, unsigned int texture_id, Shader shader, glm::mat4 model);
	void BatchRenderObjectInstances(vector <SceneDataItem>& objects, Shader shader, const glm::mat4& model_matrix, const glm::mat4& view_matrix, const glm::mat4& view_projection_matrix, const glm::vec3& camera_scale, AABB camera_view_cull);
	void BatchRenderSpriteInstances(vector <SceneDataItem>& sprites);
}