#ifndef DIM_DIMENSION3D_HPP
#define DIM_DIMENSION3D_HPP

#include "dimension3D_libraries.hpp"
#include "dimension3D_Mesh.hpp"
#include "dimension3D_Light.hpp"
#include "dimension3D_AmbientLight.hpp"
#include "dimension3D_DirectionalLight.hpp"
#include "dimension3D_PointLight.hpp"
#include "dimension3D_Material.hpp"
#include "dimension3D_Object.hpp"
#include "dimension3D_Shader.hpp"
#include "dimension3D_Texture.hpp"
#include "dimension3D_Vector2.hpp"
#include "dimension3D_Vector3.hpp"
#include "dimension3D_Vector4.hpp"
#include "dimension3D_Vector2int.hpp"
#include "dimension3D_Vector3int.hpp"
#include "dimension3D_Vector4int.hpp"
#include "dimension3D_vectors.hpp"
#include "dimension3D_VertexBuffer.hpp"
#include "dimension3D_Window.hpp"
#include "dimension3D_Camera.hpp"
#include "dimension3D_utils.hpp"
#include "dimension3D_FrameBuffer.hpp"
#include "dimension3D_Scene.hpp"
#include "dimension3D_Color.hpp"
#include "dimension3D_Controller.hpp"
#include "dimension3D_OrbitController.hpp"
#include "dimension3D_FlyController.hpp"
#include "dimension3D_DragController.hpp"
#include "dimension3D_Camera2D.hpp"
#include "dimension3D_OrthographicCamera.hpp"
#include "dimension3D_PerspectiveCamera.hpp"

namespace dim
{
	/**
	 * @brief Initialize the OpenGL library.
	 */
	void init_opengl();

	/**
	 * @brief Initialize the ImGui library.
	 */
	void init_imgui();

	/**
	 * @brief Initialize the Dimension3D library.
	 */
	void init();

	/**
	 * @brief Shut down the Dimension3D library.
	 */
	void shut_down();
}

#endif
