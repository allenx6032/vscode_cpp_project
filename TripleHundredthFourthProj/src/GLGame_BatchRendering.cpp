#include "GLGame_BatchRendering.h" 

namespace GLGame
{
	using namespace std;

	// Sprite Batch Class functions

	SpriteBatcher::SpriteBatcher() : m_VBO(GL_ARRAY_BUFFER), m_ViewProjectionMatrix(glm::mat4(1.0f)), m_VerticesWritten(0), m_MaximumQuads(10000), m_VertexSize(40)
	{
		m_Info.m_MaximumQuads = m_MaximumQuads;
		m_ObjectsInitialized = false;
		m_LastElementVBuff = 0;
		m_AmbientLight = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_MaximumTextureSlots);;
		memset(m_SlottedTextures, -1, 32 * sizeof(GLuint));

		m_VertexBuffer = new GLfloat[m_VertexSize * (m_MaximumQuads + 2)];
		m_IndexBuffer = new GLuint[6 * (m_MaximumQuads + 2)];
		m_LastElementTex = 0;

		int index_offset = 0;
		size_t index_size = 6 * m_MaximumQuads;

		// Generate sampler array
		for (int i = 0; i < 32; i++)
		{
			m_SamplerArray[i] = i;
		}
		
		// Generating an index bufffer for the texture slots
		for (size_t i = 0; i < index_size; i += 6)
		{
			m_IndexBuffer[i] = 0 + index_offset;
			m_IndexBuffer[i + 1] = 1 + index_offset;
			m_IndexBuffer[i + 2] = 3 + index_offset;
			m_IndexBuffer[i + 3] = 1 + index_offset;
			m_IndexBuffer[i + 4] = 2 + index_offset;
			m_IndexBuffer[i + 5] = 3 + index_offset;

			index_offset = index_offset + 4;
		}
	}

	SpriteBatcher::~SpriteBatcher()
	{
		delete[] m_VertexBuffer;
		delete[] m_IndexBuffer;
	}

	void SpriteBatcher::StartSpriteBatch(Camera* scene_camera, const glm::vec4& ambient_light, Shader* custom_shader)
	{
		m_Info.m_DrawCalls = 0;
		m_Info.m_QuadCount = 0;

		glm::vec4 camera_projection = scene_camera->GetProjectionCoords();
		glm::vec3 camera_position = scene_camera->GetPosition();

		m_AmbientLight = ambient_light;
		m_ViewProjectionMatrix = scene_camera->GetViewProjectionMatrix();

		m_CameraCull.x = camera_position.x;
		m_CameraCull.y = camera_position.y;
		m_CameraCull.w = camera_projection.y;
		m_CameraCull.h = camera_projection.w;

		m_CameraCullGiven = true;

		if (custom_shader != nullptr)
		{
			m_BatchShader = custom_shader;
		}

		else
		{
			m_BatchShader = &m_DefaultShader;
		}
	}

	void SpriteBatcher::StartSpriteBatch()
	{
		m_Info.m_DrawCalls = 0;
		m_Info.m_QuadCount = 0;

		m_ViewProjectionMatrix = glm::mat4(1.0f);
		m_CameraCullGiven = false;
	}

	void SpriteBatcher::StartSpriteBatch(const glm::mat4& view_projection_matrix)
	{
		m_Info.m_DrawCalls = 0;
		m_Info.m_QuadCount = 0;

		m_ViewProjectionMatrix = view_projection_matrix;
		m_CameraCullGiven = false;
	}

	BatcherInfo& SpriteBatcher::EndSpriteBatch()
	{
		DrawFullBatch();

		// Reset the projection matrix
		m_ViewProjectionMatrix = glm::mat4(1.0f);

		return m_Info;
	}

	void SpriteBatcher::AddGLGameItemToBatch(SceneDataItem item, const glm::vec4& color)
	{
		AABB item_cull;
		const glm::vec3& camera_pos = m_CameraPos;
		glm::vec4 v1, v2, v3, v4;
	
		bool is_sprite = false;
		int tex_element = 0;
		bool repeated_texture = false;
		bool should_add = false;

		Object* object = nullptr;
		Texture* texture = nullptr;

		int width = 0, height = 0;

		if (item.ItemType == sitem_type_object)
		{
			object = item.ItemObjectInstance.m_Object;
			texture = item.ItemObjectInstance.m_Object->GetSprite()->GetCurrentTexture();

			width = texture->GetWidth() + item.ItemPos.x;
			height = texture->GetHeight() + item.ItemPos.y;
			is_sprite = false;

			v4.x = item.ItemPos.x;
			v4.y = item.ItemPos.y;
			v4.z = item.ItemPos.z;
			v4.w = 1.0f;

			v4 = object->GetModelMatrix() * v4;

			glm::vec4 cull_coordinates_wh;

			cull_coordinates_wh.x = texture->GetWidth();
			cull_coordinates_wh.y = texture->GetHeight();
			cull_coordinates_wh.z = item.ItemPos.z;
			cull_coordinates_wh.w = 1.0f;

			cull_coordinates_wh = object->GetModelMatrix() * cull_coordinates_wh ;

			item_cull.x = v4.x;
			item_cull.y = v4.y;
			item_cull.w = cull_coordinates_wh.x;
			item_cull.h = cull_coordinates_wh.y;

			if (object->Cull() == false || m_CameraCullGiven == false)
			{
				should_add = true;
			}

			else if (object->Cull() == true && ObjectInCameraView(m_CameraCull, item_cull) == true && m_CameraCullGiven == true)
			{
				should_add = true;
			}

			else
			{
				should_add = false;
			}
		}

		else if (item.ItemType == sitem_type_sprite)
		{
			object = nullptr;
			texture = item.ItemSpriteInstance.m_Sprite->GetCurrentTexture();
			is_sprite = true;
			should_add = true;
		}

		else
		{
			should_add = false;
			is_sprite = false;
			object = nullptr;
			texture = nullptr;

			return;
		}

		if (should_add)
		{
			if (m_VerticesWritten == m_MaximumQuads)
			{
				DrawFullBatch();
			}

			for (int i = 0; i < m_MaximumTextureSlots; i++)
			{
				if (m_SlottedTextures[i] == texture->GetTextureID())
				{
					tex_element = i;
					repeated_texture = true;
					break;
				}
			}

			if (repeated_texture == false)
			{
				if (m_LastElementTex == m_MaximumTextureSlots)
				{
					DrawFullBatch();
				}

				else
				{
					m_SlottedTextures[m_CurrSlottedTexElement] = texture->GetTextureID();
					m_CurrSlottedTexElement++;
					tex_element = m_LastElementTex;
					m_LastElementTex++;
				}
			}

			array<GLfloat, 8> TextureCoordinates = texture->GetTextureCoords();

			// V -> Vertices
			{
				v1.x = width;
				v1.y = item.ItemPos.y;
				v1.z = item.ItemPos.z;
				v1.w = 1.0f;

				v2.x = width;
				v2.y = height;
				v2.z = item.ItemPos.z;
				v2.w = 1.0f;
				
				v3.x = item.ItemPos.x;
				v3.y = height;
				v3.z = item.ItemPos.z;
				v3.w = 1.0f;

				v1 = object->GetModelMatrix() * v1;
				v2 = object->GetModelMatrix() * v2;
				v3 = object->GetModelMatrix() * v3;
			}

			m_VertexBuffer[m_LastElementVBuff + 0] = v1.x;
			m_VertexBuffer[m_LastElementVBuff + 1] = v1.y;
			m_VertexBuffer[m_LastElementVBuff + 2] = v1.z;
			m_VertexBuffer[m_LastElementVBuff + 3] = TextureCoordinates[0];
			m_VertexBuffer[m_LastElementVBuff + 4] = TextureCoordinates[1];
			m_VertexBuffer[m_LastElementVBuff + 5] = color.r;
			m_VertexBuffer[m_LastElementVBuff + 6] = color.g;
			m_VertexBuffer[m_LastElementVBuff + 7] = color.b;
			m_VertexBuffer[m_LastElementVBuff + 8] = color.a;
			m_VertexBuffer[m_LastElementVBuff + 9] = (float)tex_element;

			m_VertexBuffer[m_LastElementVBuff + 10] = v2.x;
			m_VertexBuffer[m_LastElementVBuff + 11] = v2.y;
			m_VertexBuffer[m_LastElementVBuff + 12] = v2.z;
			m_VertexBuffer[m_LastElementVBuff + 13] = TextureCoordinates[2];
			m_VertexBuffer[m_LastElementVBuff + 14] = TextureCoordinates[3];
			m_VertexBuffer[m_LastElementVBuff + 15] = color.r;
			m_VertexBuffer[m_LastElementVBuff + 16] = color.g;
			m_VertexBuffer[m_LastElementVBuff + 17] = color.b;
			m_VertexBuffer[m_LastElementVBuff + 18] = color.a;
			m_VertexBuffer[m_LastElementVBuff + 19] = (float)tex_element;

			m_VertexBuffer[m_LastElementVBuff + 20] = v3.x;
			m_VertexBuffer[m_LastElementVBuff + 21] = v3.y;
			m_VertexBuffer[m_LastElementVBuff + 22] = v3.z;
			m_VertexBuffer[m_LastElementVBuff + 23] = TextureCoordinates[4];
			m_VertexBuffer[m_LastElementVBuff + 24] = TextureCoordinates[5];
			m_VertexBuffer[m_LastElementVBuff + 25] = color.r;
			m_VertexBuffer[m_LastElementVBuff + 26] = color.g;
			m_VertexBuffer[m_LastElementVBuff + 27] = color.b;
			m_VertexBuffer[m_LastElementVBuff + 28] = color.a;
			m_VertexBuffer[m_LastElementVBuff + 29] = (float)tex_element;

			m_VertexBuffer[m_LastElementVBuff + 30] = v4.x;
			m_VertexBuffer[m_LastElementVBuff + 31] = v4.y;
			m_VertexBuffer[m_LastElementVBuff + 32] = v4.z;
			m_VertexBuffer[m_LastElementVBuff + 33] = TextureCoordinates[6];
			m_VertexBuffer[m_LastElementVBuff + 34] = TextureCoordinates[7];
			m_VertexBuffer[m_LastElementVBuff + 35] = color.r;
			m_VertexBuffer[m_LastElementVBuff + 36] = color.g;
			m_VertexBuffer[m_LastElementVBuff + 37] = color.b;
			m_VertexBuffer[m_LastElementVBuff + 38] = color.a;
			m_VertexBuffer[m_LastElementVBuff + 39] = (float)tex_element;

			m_LastElementVBuff += m_VertexSize;
			m_VerticesWritten++;
		}
	}

	void SpriteBatcher::AddGenericTextureToBatch(Texture* texture, const glm::vec3& pos, const glm::vec4& color)
	{
		if (m_VerticesWritten == m_MaximumQuads)
		{
			DrawFullBatch();
		}

		int tex_element = 0;
		bool repeated_texture = false;

		for (int i = 0; i < m_MaximumTextureSlots; i++)
		{
			if (m_SlottedTextures[i] == texture->GetTextureID())
			{
				tex_element = i;
				repeated_texture = true;
				break;
			}
		}

		if (repeated_texture == false)
		{
			if (m_LastElementTex == m_MaximumTextureSlots)
			{
				DrawFullBatch();
			}

			else
			{
				m_SlottedTextures[m_CurrSlottedTexElement] = texture->GetTextureID();
				m_CurrSlottedTexElement++;
				tex_element = m_LastElementTex;
				m_LastElementTex++;
			}
		}

		int width = 0, height = 0;
		array<GLfloat, 8> TextureCoordinates = texture->GetTextureCoords();

		width = texture->GetWidth() + pos.x;
		height = texture->GetHeight() + pos.y;

		m_VertexBuffer[m_LastElementVBuff + 0] = width;
		m_VertexBuffer[m_LastElementVBuff + 1] = pos.y;
		m_VertexBuffer[m_LastElementVBuff + 2] = pos.z;
		m_VertexBuffer[m_LastElementVBuff + 3] = TextureCoordinates[0];
		m_VertexBuffer[m_LastElementVBuff + 4] = TextureCoordinates[1];
		m_VertexBuffer[m_LastElementVBuff + 5] = color.r;
		m_VertexBuffer[m_LastElementVBuff + 6] = color.g;
		m_VertexBuffer[m_LastElementVBuff + 7] = color.b;
		m_VertexBuffer[m_LastElementVBuff + 8] = color.a;
		m_VertexBuffer[m_LastElementVBuff + 9] = (float)tex_element;

		m_VertexBuffer[m_LastElementVBuff + 10] = width;
		m_VertexBuffer[m_LastElementVBuff + 11] = height;
		m_VertexBuffer[m_LastElementVBuff + 12] = pos.z;
		m_VertexBuffer[m_LastElementVBuff + 13] = TextureCoordinates[2];
		m_VertexBuffer[m_LastElementVBuff + 14] = TextureCoordinates[3];
		m_VertexBuffer[m_LastElementVBuff + 15] = color.r;
		m_VertexBuffer[m_LastElementVBuff + 16] = color.g;
		m_VertexBuffer[m_LastElementVBuff + 17] = color.b;
		m_VertexBuffer[m_LastElementVBuff + 18] = color.a;
		m_VertexBuffer[m_LastElementVBuff + 19] = (float)tex_element;

		m_VertexBuffer[m_LastElementVBuff + 20] = pos.x;
		m_VertexBuffer[m_LastElementVBuff + 21] = height;
		m_VertexBuffer[m_LastElementVBuff + 22] = pos.z;
		m_VertexBuffer[m_LastElementVBuff + 23] = TextureCoordinates[4];
		m_VertexBuffer[m_LastElementVBuff + 24] = TextureCoordinates[5];
		m_VertexBuffer[m_LastElementVBuff + 25] = color.r;
		m_VertexBuffer[m_LastElementVBuff + 26] = color.g;
		m_VertexBuffer[m_LastElementVBuff + 27] = color.b;
		m_VertexBuffer[m_LastElementVBuff + 28] = color.a;
		m_VertexBuffer[m_LastElementVBuff + 29] = (float)tex_element;

		m_VertexBuffer[m_LastElementVBuff + 30] = pos.x;
		m_VertexBuffer[m_LastElementVBuff + 31] = pos.y;
		m_VertexBuffer[m_LastElementVBuff + 32] = pos.z;
		m_VertexBuffer[m_LastElementVBuff + 33] = TextureCoordinates[6];
		m_VertexBuffer[m_LastElementVBuff + 34] = TextureCoordinates[7];
		m_VertexBuffer[m_LastElementVBuff + 35] = color.r;
		m_VertexBuffer[m_LastElementVBuff + 36] = color.g;
		m_VertexBuffer[m_LastElementVBuff + 37] = color.b;
		m_VertexBuffer[m_LastElementVBuff + 38] = color.a;
		m_VertexBuffer[m_LastElementVBuff + 39] = (float)tex_element;

		m_LastElementVBuff += m_VertexSize;
		m_VerticesWritten++;
	}

	void SpriteBatcher::AddGenericTextureToBatchCustom(Texture* texture, const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color)
	{
		if (m_VerticesWritten == m_MaximumQuads)
		{
			DrawFullBatch();
		}

		int tex_element = 0;
		bool repeated_texture = false;

		for (int i = 0; i < m_MaximumTextureSlots; i++)
		{
			if (m_SlottedTextures[i] == texture->GetTextureID())
			{
				tex_element = i;
				repeated_texture = true;
				break;
			}
		}

		if (repeated_texture == false)
		{
			if (m_LastElementTex == m_MaximumTextureSlots)
			{
				DrawFullBatch();
			}

			else
			{
				m_SlottedTextures[m_CurrSlottedTexElement] = texture->GetTextureID();
				m_CurrSlottedTexElement++;
				tex_element = m_LastElementTex;
				m_LastElementTex++;
			}
		}

		int width = 0, height = 0;
		array<GLfloat, 8> TextureCoordinates = texture->GetTextureCoords();

		width = size.x + pos.x;
		height = size.y + pos.y;

		m_VertexBuffer[m_LastElementVBuff + 0] = width;
		m_VertexBuffer[m_LastElementVBuff + 1] = pos.y;
		m_VertexBuffer[m_LastElementVBuff + 2] = pos.z;
		m_VertexBuffer[m_LastElementVBuff + 3] = TextureCoordinates[0];
		m_VertexBuffer[m_LastElementVBuff + 4] = TextureCoordinates[1];
		m_VertexBuffer[m_LastElementVBuff + 5] = color.r;
		m_VertexBuffer[m_LastElementVBuff + 6] = color.g;
		m_VertexBuffer[m_LastElementVBuff + 7] = color.b;
		m_VertexBuffer[m_LastElementVBuff + 8] = color.a;
		m_VertexBuffer[m_LastElementVBuff + 9] = (float)tex_element;

		m_VertexBuffer[m_LastElementVBuff + 10] = width;
		m_VertexBuffer[m_LastElementVBuff + 11] = height;
		m_VertexBuffer[m_LastElementVBuff + 12] = pos.z;
		m_VertexBuffer[m_LastElementVBuff + 13] = TextureCoordinates[2];
		m_VertexBuffer[m_LastElementVBuff + 14] = TextureCoordinates[3];
		m_VertexBuffer[m_LastElementVBuff + 15] = color.r;
		m_VertexBuffer[m_LastElementVBuff + 16] = color.g;
		m_VertexBuffer[m_LastElementVBuff + 17] = color.b;
		m_VertexBuffer[m_LastElementVBuff + 18] = color.a;
		m_VertexBuffer[m_LastElementVBuff + 19] = (float)tex_element;

		m_VertexBuffer[m_LastElementVBuff + 20] = pos.x;
		m_VertexBuffer[m_LastElementVBuff + 21] = height;
		m_VertexBuffer[m_LastElementVBuff + 22] = pos.z;
		m_VertexBuffer[m_LastElementVBuff + 23] = TextureCoordinates[4];
		m_VertexBuffer[m_LastElementVBuff + 24] = TextureCoordinates[5];
		m_VertexBuffer[m_LastElementVBuff + 25] = color.r;
		m_VertexBuffer[m_LastElementVBuff + 26] = color.g;
		m_VertexBuffer[m_LastElementVBuff + 27] = color.b;
		m_VertexBuffer[m_LastElementVBuff + 28] = color.a;
		m_VertexBuffer[m_LastElementVBuff + 29] = (float)tex_element;

		m_VertexBuffer[m_LastElementVBuff + 30] = pos.x;
		m_VertexBuffer[m_LastElementVBuff + 31] = pos.y;
		m_VertexBuffer[m_LastElementVBuff + 32] = pos.z;
		m_VertexBuffer[m_LastElementVBuff + 33] = TextureCoordinates[6];
		m_VertexBuffer[m_LastElementVBuff + 34] = TextureCoordinates[7];
		m_VertexBuffer[m_LastElementVBuff + 35] = color.r;
		m_VertexBuffer[m_LastElementVBuff + 36] = color.g;
		m_VertexBuffer[m_LastElementVBuff + 37] = color.b;
		m_VertexBuffer[m_LastElementVBuff + 38] = color.a;
		m_VertexBuffer[m_LastElementVBuff + 39] = (float)tex_element;

		m_LastElementVBuff += m_VertexSize;
		m_VerticesWritten++;
	}

	void SpriteBatcher::AddGenericObjectToBatch(GenericObject object)
	{
		if (m_VerticesWritten == m_MaximumQuads)
		{
			DrawFullBatch();
		}

		int tex_element = 0;
		bool repeated_texture = false;

		for (int i = 0; i < m_MaximumTextureSlots; i++)
		{
			if (m_SlottedTextures[i] == object.texture->GetTextureID())
			{
				tex_element = i;
				repeated_texture = true;
				break;
			}
		}

		if (repeated_texture == false)
		{
			if (m_LastElementTex == m_MaximumTextureSlots)
			{
				DrawFullBatch();
			}

			else
			{
				m_SlottedTextures[m_CurrSlottedTexElement] = object.texture->GetTextureID();
				m_CurrSlottedTexElement++;
				tex_element = m_LastElementTex;
				m_LastElementTex++;
			}
		}

		GLfloat* TextureCoordinates = object.tex_coords;
		glm::vec4 color = object.color;

		m_VertexBuffer[m_LastElementVBuff + 0] = object.coords[0];
		m_VertexBuffer[m_LastElementVBuff + 1] = object.coords[1];
		m_VertexBuffer[m_LastElementVBuff + 2] = object.coords[2];
		m_VertexBuffer[m_LastElementVBuff + 3] = TextureCoordinates[0];
		m_VertexBuffer[m_LastElementVBuff + 4] = TextureCoordinates[1];
		m_VertexBuffer[m_LastElementVBuff + 5] = color.r;
		m_VertexBuffer[m_LastElementVBuff + 6] = color.g;
		m_VertexBuffer[m_LastElementVBuff + 7] = color.b;
		m_VertexBuffer[m_LastElementVBuff + 8] = color.a;
		m_VertexBuffer[m_LastElementVBuff + 9] = (float)tex_element;

		m_VertexBuffer[m_LastElementVBuff + 10] = object.coords[3];
		m_VertexBuffer[m_LastElementVBuff + 11] = object.coords[4];
		m_VertexBuffer[m_LastElementVBuff + 12] = object.coords[5];
		m_VertexBuffer[m_LastElementVBuff + 13] = TextureCoordinates[2];
		m_VertexBuffer[m_LastElementVBuff + 14] = TextureCoordinates[3];
		m_VertexBuffer[m_LastElementVBuff + 15] = color.r;
		m_VertexBuffer[m_LastElementVBuff + 16] = color.g;
		m_VertexBuffer[m_LastElementVBuff + 17] = color.b;
		m_VertexBuffer[m_LastElementVBuff + 18] = color.a;
		m_VertexBuffer[m_LastElementVBuff + 19] = (float)tex_element;

		m_VertexBuffer[m_LastElementVBuff + 20] = object.coords[6];
		m_VertexBuffer[m_LastElementVBuff + 21] = object.coords[7];
		m_VertexBuffer[m_LastElementVBuff + 22] = object.coords[8];
		m_VertexBuffer[m_LastElementVBuff + 23] = TextureCoordinates[4];
		m_VertexBuffer[m_LastElementVBuff + 24] = TextureCoordinates[5];
		m_VertexBuffer[m_LastElementVBuff + 25] = color.r;
		m_VertexBuffer[m_LastElementVBuff + 26] = color.g;
		m_VertexBuffer[m_LastElementVBuff + 27] = color.b;
		m_VertexBuffer[m_LastElementVBuff + 28] = color.a;
		m_VertexBuffer[m_LastElementVBuff + 29] = (float)tex_element;

		m_VertexBuffer[m_LastElementVBuff + 30] = object.coords[9];
		m_VertexBuffer[m_LastElementVBuff + 31] = object.coords[10];
		m_VertexBuffer[m_LastElementVBuff + 32] = object.coords[11];
		m_VertexBuffer[m_LastElementVBuff + 33] = TextureCoordinates[6];
		m_VertexBuffer[m_LastElementVBuff + 34] = TextureCoordinates[7];
		m_VertexBuffer[m_LastElementVBuff + 35] = color.r;
		m_VertexBuffer[m_LastElementVBuff + 36] = color.g;
		m_VertexBuffer[m_LastElementVBuff + 37] = color.b;
		m_VertexBuffer[m_LastElementVBuff + 38] = color.a;
		m_VertexBuffer[m_LastElementVBuff + 39] = (float)tex_element;

		m_LastElementVBuff += m_VertexSize;
		m_VerticesWritten++;
	}

	void SpriteBatcher::DrawFullBatch()
	{
		if (!m_ObjectsInitialized)
		{
			string vertex_data;
			stringstream vert_data_s;
			string frag_data;
			stringstream frag_data_s;
			ifstream vertex_file;
			ifstream fragment_file;

			vertex_file.open(GLGAME_DEFAULT_BATCH_VERTEX, ios::in);
			fragment_file.open(GLGAME_DEFAULT_BATCH_FRAGMENT, ios::in);

			if (vertex_file.good() && vertex_file.is_open() && fragment_file.good() && fragment_file.is_open())
			{
				// Set the max texture slot macro and add the version to the fragment shader

				vert_data_s << vertex_file.rdbuf();
				vertex_data = vert_data_s.str();
				frag_data_s << "#version 330 core" << endl << "#define MAX_TEX_SLOTS " << m_MaximumTextureSlots << endl;
				frag_data_s << fragment_file.rdbuf();
				frag_data = frag_data_s.str();

				m_DefaultShader.CreateShaderProgramFromString(vertex_data, frag_data);
				m_DefaultShader.CompileShaders();

				vert_data_s.flush();
				frag_data_s.flush();
				vertex_file.close();
				fragment_file.close();
			}

			else
			{
				vertex_file.close();
				fragment_file.close();

				Log::LogToFile("Invalid batch shader path..");
				Log::LogToConsole("Invalid batch shader path..");
			}

			// Setting up OpenGL Objects and writing the buffer data

			size_t index_size = 6 * m_MaximumQuads;

			m_VAO.Bind();
			m_IBO.Bind();
			m_IBO.BufferData(index_size * sizeof(GLuint), m_IndexBuffer, GL_STATIC_DRAW);

			// Attributes (4)
			// 0 : Position
			// 1 : Texture Coordinates
			// 2 : Color
			// 3 : Texture Element

			m_VBO.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)0);
			m_VBO.VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			m_VBO.VertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
			m_VBO.VertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(9 * sizeof(GLfloat)));
			m_VAO.Unbind();

			m_ObjectsInitialized = true;;
		}

		if (m_BatchShader == nullptr)
		{
			m_BatchShader = &m_DefaultShader;
		}

		m_BatchShader->Use();

		for (int i = 0; i < m_MaximumTextureSlots; i++)
		{
			if (m_SlottedTextures[i] < 0)
			{
				continue;
			}

			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, m_SlottedTextures[i]);
		}

		m_BatchShader->SetVector4f("u_AmbientColor", m_AmbientLight, 0);
		m_BatchShader->SetMatrix4("u_ViewProjectionMatrix", m_ViewProjectionMatrix, 0);
		m_BatchShader->SetIntegerArray("u_Textures", m_SamplerArray, 32, 0);
		
		m_VAO.Bind();
		m_VBO.BufferData((m_VerticesWritten * m_VertexSize) * sizeof(GLfloat), m_VertexBuffer, GL_STATIC_DRAW);
		glDrawElements(GL_TRIANGLES, 6 * m_VerticesWritten, GL_UNSIGNED_INT, (void*)0);
		m_VAO.Unbind();

		//m_ViewProjectionMatrix = glm::mat4(1.0f);
		//m_AmbientLight = glm::vec4(1.0f);
		m_Info.m_QuadCount += m_VerticesWritten;
		m_Info.m_DrawCalls += 1;
		m_VerticesWritten = 0;
		m_LastElementVBuff = 0;
		m_LastElementTex = 0;
		m_CameraCullGiven = false;
		
		for (int i = 0; i < m_MaximumTextureSlots; i++)
		{
			m_SlottedTextures[i] = -1;
		}

		m_CurrSlottedTexElement = 0;
	}

	// All of the below batch rendering functions are deprecated.
	// Use the SpriteBatcher() class for rendering anything
	// These functions were originally only used for testing
	// Deprecated
	bool temp_log_draw_calls = true;

	// Batch Renderer 
		//Renders max 10000 quads in one draw call. 
		//Set the max quad render size to the "max_size" variable"

	void BatchRenderObjectInstances(vector <SceneDataItem>& objects, Shader shader, const glm::mat4& model_matrix, const glm::mat4& view_matrix, const glm::mat4& view_projection_matrix, const glm::vec3& camera_scale, AABB camera_view_cull) // camera view is for culling
	{
		const unsigned int max_size = 10000;
		unsigned int index_offset = 0;
		uint32_t current_element = 0;
		uint32_t vertices_written = 0;
		uint32_t erase_factor = 0;
		static size_t vertex_size = (max_size * 20) * sizeof(GLfloat); // in bytes !
		static size_t index_size = (max_size * 6) * sizeof(GLuint); // in bytes !

		static GLfloat* vertex_buffer = new GLfloat[vertex_size];
		static GLuint* index_buffer = new GLuint[index_size];

		static bool first_run = true;

		// Initialize all the opengl object
		static VertexBuffer VBO(GL_ARRAY_BUFFER);
		static IndexBuffer IBO;
		static VertexArray VAO;

		unsigned int remaining_size = objects.size();

		array<GLfloat, 8> TextureCoordinates;
		uint32_t width;
		uint32_t height;
		// Generate index buffer

		if (first_run)
		{
			for (size_t i = 0; i < index_size; i += 6)
			{
				index_buffer[i] = 0 + index_offset;
				index_buffer[i + 1] = 1 + index_offset;
				index_buffer[i + 2] = 3 + index_offset;
				index_buffer[i + 3] = 1 + index_offset;
				index_buffer[i + 4] = 2 + index_offset;
				index_buffer[i + 5] = 3 + index_offset;

				index_offset = index_offset + 4;
			}
		}

		shader.Use();
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, objects[0].ItemObjectInstance.m_Object->GetSprite()->GetCurrentTexture()->GetTextureID());
		shader.SetInteger("u_Texture", 1, 0);
		shader.SetMatrix4("u_ViewProjectionMatrix", view_projection_matrix, 0);

		if (first_run)
		{
			Log::_LogRendererInit(glfwGetTime(), max_size, __FILE__, __LINE__);
			VAO.Bind();
			IBO.Bind();
			IBO.BufferData(index_size, index_buffer, GL_STATIC_DRAW);
			VBO.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
			VBO.VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			VAO.Unbind();
		}

		AABB object_cull = { 0.0f, 0.0f, 0.0f, 0.0f };
		glm::vec4 object_cull_vec(1.0f);
		bool should_draw = true;

		glm::vec4 transformed_position;

		while (remaining_size != 0)
		{
			for (size_t i = 0; i < remaining_size; i++)
			{
				TextureCoordinates = objects[i].ItemObjectInstance.m_Object->GetSprite()->GetCurrentTexture()->GetTextureCoords();
				width = objects[i].ItemObjectInstance.m_Object->GetSprite()->GetCurrentTextureWidth() + objects[i].ItemPos.x;
				height = objects[i].ItemObjectInstance.m_Object->GetSprite()->GetCurrentTextureHeight() + objects[i].ItemPos.y;

				cout << transformed_position.x << " " << transformed_position.y << " " << transformed_position.z << " " << transformed_position.w << "\n";

				object_cull.x = transformed_position.x;
				object_cull.y = (float)(transformed_position.y + objects[i].ItemObjectInstance.m_Object->GetSprite()->GetCurrentTextureHeight());
				object_cull.w = (float)objects[i].ItemObjectInstance.m_Object->GetSprite()->GetCurrentTextureWidth() * camera_scale.x;
				object_cull.h = (float)objects[i].ItemObjectInstance.m_Object->GetSprite()->GetCurrentTextureHeight() * camera_scale.y;

				if (objects[i].ItemObjectInstance.m_Object->Cull() == false)
				{
					should_draw = true;
				}

				else if (objects[i].ItemObjectInstance.m_Object->Cull() == true && ObjectInCameraView(camera_view_cull, object_cull) == true)
				{
					should_draw = true;
				}

				else
				{
					should_draw = false;
				}

				if (should_draw == true)
				{
					transformed_position.x = width;
					transformed_position.y = objects[i].ItemPos.y;
					transformed_position.z = objects[i].ItemPos.z;
					transformed_position.w = 1.0f;

					transformed_position = model_matrix * glm::vec4(transformed_position);

					vertex_buffer[current_element] = transformed_position.x;
					vertex_buffer[current_element + 1] = transformed_position.y;
					vertex_buffer[current_element + 2] = transformed_position.z;
					vertex_buffer[current_element + 3] = TextureCoordinates[0];
					vertex_buffer[current_element + 4] = TextureCoordinates[1];

					transformed_position.x = width;
					transformed_position.y = height;
					transformed_position.z = objects[i].ItemPos.z;
					transformed_position.w = 1.0f;

					transformed_position = model_matrix * glm::vec4(transformed_position);

					vertex_buffer[current_element + 5] = transformed_position.x;
					vertex_buffer[current_element + 6] = transformed_position.y;
					vertex_buffer[current_element + 7] = transformed_position.z;
					vertex_buffer[current_element + 8] = TextureCoordinates[2];
					vertex_buffer[current_element + 9] = TextureCoordinates[3];

					transformed_position.x = objects[i].ItemPos.x;
					transformed_position.y = height;
					transformed_position.z = objects[i].ItemPos.z;
					transformed_position.w = 1.0f;

					transformed_position = model_matrix * glm::vec4(transformed_position);

					vertex_buffer[current_element + 10] = transformed_position.x;
					vertex_buffer[current_element + 11] = transformed_position.y;
					vertex_buffer[current_element + 12] = transformed_position.z;
					vertex_buffer[current_element + 13] = TextureCoordinates[4];
					vertex_buffer[current_element + 14] = TextureCoordinates[5];

					transformed_position.x = objects[i].ItemPos.x;
					transformed_position.y = objects[i].ItemPos.y;
					transformed_position.z = objects[i].ItemPos.z;
					transformed_position.w = 1.0f;

					transformed_position = model_matrix * glm::vec4(transformed_position);

					vertex_buffer[current_element + 15] = transformed_position.x;
					vertex_buffer[current_element + 16] = transformed_position.y;
					vertex_buffer[current_element + 17] = transformed_position.z;
					vertex_buffer[current_element + 18] = TextureCoordinates[6];
					vertex_buffer[current_element + 19] = TextureCoordinates[7];

					current_element += 20;

					if (i == (max_size - 1))
					{
						vertices_written += 1;
						erase_factor += 1;
						break;
					}

					vertices_written += 1;
					erase_factor += 1;
				}

				else
				{
					erase_factor++;
				}
			}

			VAO.Bind();
			VBO.BufferData((vertices_written * 20) * sizeof(GLfloat), vertex_buffer, GL_STATIC_DRAW);
			glDrawElements(GL_TRIANGLES, 6 * vertices_written, GL_UNSIGNED_INT, (void*)0);

			if (temp_log_draw_calls)
			{
				//std::cout << "DRAW CALL. " << vertices_written << " Quads successfully drawn\n ";
			}

			VAO.Unbind();

			remaining_size = remaining_size - erase_factor;
			current_element = 0;
		}

		first_run = false;
	}

	// To test the batch rendering capabilities
	void BatchRenderGenericObjects(vector <GenericObject>& objects, unsigned int texture_id, Shader shader, glm::mat4 model)
	{
		const unsigned int max_size = 512;
		unsigned int index_offset = 0;
		uint32_t current_element = 0;
		uint32_t vertices_written = 0;
		static size_t vertex_size = (max_size * 20) * sizeof(GLfloat); // in bytes !
		static size_t index_size = (max_size * 6) * sizeof(GLuint); // in bytes !

		static GLfloat* vertex_buffer = new GLfloat[vertex_size];
		static GLuint* index_buffer = new GLuint[index_size];

		static bool first_run = true;

		// Initialize all the opengl object
		static VertexBuffer VBO(GL_ARRAY_BUFFER);
		static IndexBuffer IBO;
		static VertexArray VAO;

		unsigned int remaining_size = objects.size();

		// Generate index buffer

		if (first_run)
		{
			for (size_t i = 0; i < index_size; i += 6)
			{
				index_buffer[i] = 0 + index_offset;
				index_buffer[i + 1] = 1 + index_offset;
				index_buffer[i + 2] = 2 + index_offset;
				index_buffer[i + 3] = 2 + index_offset;
				index_buffer[i + 4] = 3 + index_offset;
				index_buffer[i + 5] = 0 + index_offset;

				index_offset = index_offset + 4;
			}
		}

		shader.Use();
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		shader.SetInteger("u_Texture", 1, 0);
		shader.SetMatrix4("u_Model", model, 0);

		if (first_run)
		{
			VAO.Bind();
			IBO.Bind();
			IBO.BufferData(index_size, index_buffer, GL_STATIC_DRAW);
			VBO.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
			VBO.VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			VAO.Unbind();
		}

		while (remaining_size != 0)
		{
			for (size_t i = 0; i < remaining_size; i++)
			{
				vertex_buffer[current_element] = objects[i].coords[0];
				vertex_buffer[current_element + 1] = objects[i].coords[1];
				vertex_buffer[current_element + 2] = objects[i].coords[2];
				vertex_buffer[current_element + 3] = objects[i].tex_coords[0];
				vertex_buffer[current_element + 4] = objects[i].tex_coords[1];

				vertex_buffer[current_element + 5] = objects[i].coords[3];
				vertex_buffer[current_element + 6] = objects[i].coords[4];
				vertex_buffer[current_element + 7] = objects[i].coords[5];
				vertex_buffer[current_element + 8] = objects[i].tex_coords[2];
				vertex_buffer[current_element + 9] = objects[i].tex_coords[3];

				vertex_buffer[current_element + 10] = objects[i].coords[6];
				vertex_buffer[current_element + 11] = objects[i].coords[7];
				vertex_buffer[current_element + 12] = objects[i].coords[8];
				vertex_buffer[current_element + 13] = objects[i].tex_coords[4];
				vertex_buffer[current_element + 14] = objects[i].tex_coords[5];

				vertex_buffer[current_element + 15] = objects[i].coords[9];
				vertex_buffer[current_element + 16] = objects[i].coords[10];
				vertex_buffer[current_element + 17] = objects[i].coords[11];
				vertex_buffer[current_element + 18] = objects[i].tex_coords[6];
				vertex_buffer[current_element + 19] = objects[i].tex_coords[7];

				current_element += 20;

				if (i == (max_size - 1))
				{
					vertices_written = i + 1;
					break;
				}

				vertices_written = i + 1;
			}

			VAO.Bind();
			VBO.BufferData((vertices_written * 20) * sizeof(GLfloat), vertex_buffer, GL_STATIC_DRAW);
			glDrawElements(GL_TRIANGLES, 6 * vertices_written, GL_UNSIGNED_INT, (void*)0);
			VAO.Unbind();

			remaining_size = remaining_size - vertices_written;
			current_element = 0;
		}

		first_run = false;
	}
}
