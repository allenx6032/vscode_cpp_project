#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <array>
#include <string>

#include "GLGame_GLDebug.h"
#include "GLGame_Background.h"
#include "GLGame_SceneData.h"
#include "GLGame_IndexBuffer.h"
#include "GLGame_VertexArray.h"
#include "GLGame_VertexBuffer.h"
#include "GLGame_Texture.h"
#include "GLGame_Shader.h"
#include "GLGame_Log.h"

namespace GLGame
{
	struct _RenderItem
	{
		float x;
		float y;
		float z;
		float tw;
		float th;
		Texture* tex;
	};

	
	void NormallyRenderBackgrounds(SceneBackground* bg, Shader* shader, float w, float h, const glm::vec4& color, const glm::mat4& model_matrix, const glm::mat4& view_matrix, const glm::mat4& view_projection_matrix);
	
	// This function was originally used to test opengl code for the scene editor
	void _NormallyRenderSEItems(_RenderItem item, Shader shader, const glm::mat4& projection_matrix);
}
