#include "GLGame_SceneEditor.h"

// The GLGame Scene editor has to be called at runtime. 

namespace GLGame
{
	namespace SceneEditor
	{
		enum SceneEditorItemTypes
		{
			SE_ObjectType,
			SE_SpriteType,
			SE_BackgroundType
		};

		struct SceneEditorRenderItem
		{
			Object* obj = nullptr;
			Sprite* spr = nullptr;
			float x;
			float y;
			int layer;
			SceneEditorItemTypes item_type;
		};

		static enum CurrentItemTypeSelection
		{
			ObjectSelection = 0,
			SpriteSelection,
			Nothing
		};

		static enum Operations
		{
			PlaceItems = 0,
			ChangeBackground,
			SeeProperties,
			DebugGame,
			ViewScene,
		};

		// For undo and redo

		struct LastOperation
		{
			Operations last_operation;
			string id;
			int layer;
			int element;
			bool erase;
			SceneEditorRenderItem item;
		};

		LastOperation LastOperationCompleted;

		// Variables to initialize the window
		static bool SceneEditorInitialized = false;
		static GLFWwindow* SceneEditorWindow;
		static int SceneEditorWidth = 1366;
		static int SceneEditorHeight = 750;
		static bool SE_window_destroyed = false;

		// Scene Editor event variables
		static double StartPanX = 0;
		static double StartPanY = 0;

		// Data to display/store/show
		static unordered_map<string, Object*>* SceneEditorGlobalObjects = nullptr;
		static unordered_map<string, Sprite*>* SceneEditorGlobalSprites = nullptr;
		static unordered_map<string, Background*>* SceneEditorGlobalBackgrounds = nullptr;

		static vector<string>* ObjectIDList = nullptr;
		static vector<string>* SpriteIDList = nullptr;
		static vector<string>* BackgroundIDList = nullptr;
		static SpriteBatcher* SceneEditorBatcher;

		// Data used by the UI elements and widgets

		// To select the type of data
		static int RadioObjectSelected = -1;
		static int RadioSpriteSelected = -1;
	    static int ItemTypeSelected = Nothing;

		// For adding or removing backgrounds
		static map<int, vector<Background*>> SceneEditorBackgroundQueue;
		static int BackgroundSelected = 0;

		// Render queue and layering
		static int CurrentSceneEditorLayer = 0;
		static map<int, vector<SceneEditorRenderItem>> SceneEditorItemQueue;
		static SceneEditorRenderItem GhostObjectImage;

		static Shader SceneEditorRenderItemShader;
		static Shader SceneEditorBackgroundShader;

		// Saving file

		static char* SceneFilePath = nullptr;
		static bool SceneFilePathSet = false;
		static const size_t SceneFilePthSize = 512;
		static bool UnsavedChanges = true;

		// Mouse and window attributes
		static double MousePosX = 0;
		static double MousePosY = 0;
		static int WindowSizeX = 0;
		static int WindowSizeY = 0;
		static double MouseScrollOffsetX = 0;
		static double MouseScrollOffsetY = 0;

		static const char* ObjectInsertString = "@#$*#\0";
		static const char* SpriteInsertString = "$*$^(\0";

		// Modal windows

		// Close modal window
		static bool ShouldShowCloseModalWindow = false;

		static bool ShouldShowAboutTheAuthorWindow = false;
		static bool ShouldShowSupportMeWindow = false;
		static bool ShouldShowWITWindow = false; // What is this window
		static bool ShouldShowDependenciesWindow = false;

		// Saving modal window
		static bool ShouldShowSaveAsWindow = false;
		static bool ShouldShowFileErrWindow = false;

		// Input blocking
		static bool ShouldBlockInput = false;

		// IMGUI context => Scene editor window
		ImGuiContext* imcontext;

		// Scene editor camera
		Camera* SceneEditorCamera;

		// For the grid and snap
		static int GridX = 64;
		static int GridY = 64;
		static bool ViewGrid = true;
		static bool ShouldSnap = true;

		// Other needed structures
		int CurrentOperationSelected = 0; // Has to be an int for imgui. Used as an enum class "Operations" 

		// Debug Window
		GameDebugInfo* DebugInfo;

		GLFWwindow* _Init(GLFWwindow* share_window, ImGuiContext* context);
		void Undo();
		void ExtendString(string& str, int ex_amt, const string& ex_c);
		void FlushSceneFile();
		void DrawModalWindows();
		void RenderSceneEditorItems();
		void RenderSceneEditorBackgrounds();
		void DrawFloatingWidgets();
		void DrawPlaceItemWindowWidget();
		void DrawMenuBar();
		float SnapToGrid(int value, int size);
		void RenderBackgroundAddWidget();
		void DrawGrid();
		void RenderDebugWindow();
		
		// Event callbacks

		void SEWindowResizeCallback(GLFWwindow* window, int width, int height);
		void SEKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		void SECursorPosCallback(GLFWwindow* window, double xpos, double ypos);
		void SEMouseCallback(GLFWwindow* window, int button, int action, int mods);
		void SEScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
		void SEWindowCloseCallback(GLFWwindow* window);
		void _SetSceneEditorCloseFlag(bool val);

		GLFWwindow* InitSceneEditor(GameDebugInfo* debug_info, unordered_map<string, Object*>* global_objects, unordered_map<string, Sprite*>* global_sprites, unordered_map<string, Background*>* global_bgs, vector<string>* objid_list, vector<string>* sprid_list, vector<string>* bgid_list, GLFWwindow* window, ImGuiContext* context)
		{
			DebugInfo = debug_info;
			SceneEditorGlobalObjects = global_objects;
			SceneEditorGlobalSprites = global_sprites;
			SceneEditorGlobalBackgrounds = global_bgs;

			ObjectIDList = objid_list;
			SpriteIDList = sprid_list;
			BackgroundIDList = bgid_list;

			return (_Init(window, context));
		}

		GLFWwindow* _Init(GLFWwindow* share_window, ImGuiContext* context)
		{
			glfwInit();

			SceneEditorWindow = glfwCreateWindow(SceneEditorWidth, SceneEditorHeight, "GLGame Scene Editor", nullptr, share_window);

			if (SceneEditorWindow == 0)
			{
				Log::LogToConsole("Scene Editor window could not be created!");
				return nullptr;
			}

			glfwMakeContextCurrent(SceneEditorWindow);

			// Set all the GLFW Event Callbacks required

			//glfwSetCursorPosCallback(m_GameWindow, GLGameCursorPosCallback);
			glfwSetKeyCallback(SceneEditorWindow, SEKeyCallback);
			glfwSetMouseButtonCallback(SceneEditorWindow, SEMouseCallback);
			glfwSetScrollCallback(SceneEditorWindow, SEScrollCallback);
			glfwSetFramebufferSizeCallback(SceneEditorWindow, SEWindowResizeCallback);
			glfwSetWindowCloseCallback(SceneEditorWindow, SEWindowCloseCallback);
			glfwSetCursorPosCallback(SceneEditorWindow, SECursorPosCallback);
			                                                                   
			SceneEditorInitialized = true;

			// Imgui initialization

			ImGui::SetCurrentContext(ImGui::CreateContext());
			imcontext = ImGui::GetCurrentContext();
			ImGui_ImplGlfw_InitForOpenGL(SceneEditorWindow, true);
			ImGui_ImplOpenGL3_Init((const char*)"#version 130");
			ImGui::StyleColorsDark();

			// Build the font atlas
			ImGuiIO& io = ImGui::GetIO();
			io.Fonts->AddFontDefault();
			io.Fonts->Build();

			// Imgui style
			ImGuiStyle& style = ImGui::GetStyle();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				style.WindowRounding = 0.0f;
				style.Colors[ImGuiCol_WindowBg].w = 1.0f;
			}

			// Set up character buffers
			SceneFilePath = new char[SceneFilePthSize];
			memset(SceneFilePath, '\0', SceneFilePthSize);

			// Setup camera, batchers and shaders
			SceneEditorCamera = new Camera(0.0f, (float)SceneEditorWidth, 0.0f, (float)SceneEditorHeight);
			SceneEditorBatcher = new SpriteBatcher();
			
			SceneEditorRenderItemShader.CreateShaderProgramFromFile(GLGAME_DEFAULT_SE_VERTEX, GLGAME_DEFAULT_SE_FRAGMENT);
			SceneEditorBackgroundShader.CreateShaderProgramFromFile(GLGAME_DEFAULT_BGSHADER_VERTEX, GLGAME_DEFAULT_BGSHADER_FRAGMENT);
			SceneEditorRenderItemShader.CompileShaders();
			SceneEditorBackgroundShader.CompileShaders();

			return SceneEditorWindow;
		}

		void Undo()
		{
			if (LastOperationCompleted.erase)
			{
				if (LastOperationCompleted.last_operation == PlaceItems)
				{
					SceneEditorItemQueue.at(LastOperationCompleted.layer).erase(
						SceneEditorItemQueue.at(LastOperationCompleted.layer).begin()
						+ LastOperationCompleted.element);
				}

				else if (LastOperationCompleted.last_operation == ChangeBackground)
				{
					SceneEditorBackgroundQueue.at(LastOperationCompleted.layer).erase(
						SceneEditorBackgroundQueue.at(LastOperationCompleted.layer).begin() + LastOperationCompleted.element);
				}

				LastOperationCompleted.erase = false;
			}

			else if (LastOperationCompleted.erase == false)
			{
				if (LastOperationCompleted.last_operation == PlaceItems)
				{
					SceneEditorItemQueue[LastOperationCompleted.layer].push_back(LastOperationCompleted.item);
					LastOperationCompleted.erase = true;
				}

				else if (LastOperationCompleted.last_operation == ChangeBackground)
				{
					SceneEditorBackgroundQueue[LastOperationCompleted.layer].push_back(SceneEditorGlobalBackgrounds->at(LastOperationCompleted.id));
					LastOperationCompleted.erase = true;
				}
			}
		}

		void OpenSceneFile()
		{

		}

		void ExtendString(string& str, int ex_amt, const string& ex_c)
		{
			int temp = ex_amt - str.size();
			ex_amt = abs(temp);

			for (int i = 0; i < ex_amt; i++)
			{
				str.insert(0, ex_c);
			}
		}

		void FlushSceneFile()
		{
			static string scene_file_header = string(GLGAME_SCENE_FILE_HEADER);
			char* scene_garbage_str = new char[12];
			fstream scene_file;

			// item write strings
			string item_type_str;
			string layer;
			string x, y;
			string id;
			string id_size;

			memset(scene_garbage_str, '\0', 12);
			scene_file.open(string(SceneFilePath), ios::out | ios::binary);

			if (scene_file.good() && scene_file.is_open())
			{
				// Write the scene header
				scene_file.write(scene_file_header.c_str(), scene_file_header.size());

				// Write the backgrounds to the scene file
				for (auto e = SceneEditorBackgroundQueue.begin(); e != SceneEditorBackgroundQueue.end(); e++)
				{
					for (int i = 0; i < e->second.size(); i++)
					{
						layer = to_string(e->first);
						x = to_string(0);
						y = to_string(0);

						item_type_str = "BG_";
						id = e->second.at(i)->GetBackgroundID();
						id_size = to_string(id.size());

						ExtendString(layer, 8, "!");
						ExtendString(x, 12, "@");
						ExtendString(y, 12, "#");
						ExtendString(id_size, 8, "$");

						scene_file.write(item_type_str.c_str(), item_type_str.size());
						scene_file.write(layer.c_str(), 8);
						scene_file.write(x.c_str(), 12);
						scene_file.write(y.c_str(), 12);
						scene_file.write(id_size.c_str(), 8);
						scene_file.write(id.c_str(), id.size());

						// Generate a garbage string of 8 characters and write it to the file stream
						// To further limit readability of the outputted scene file
						for (int i = 0; i < 8; i++)
						{
							scene_garbage_str[i] = (char)rand() % 200;
						}

						scene_garbage_str[9] = '\0';
						scene_file.write(scene_garbage_str, 8);
					}
				}

				// Write the scene items to the scene file
				for (auto e = SceneEditorItemQueue.begin(); e != SceneEditorItemQueue.end(); e++)
				{
					for (int i = 0; i < e->second.size(); i++)
					{
						SceneEditorRenderItem item;

						item = e->second.at(i);
						layer = to_string(item.layer);
						x = to_string((int)item.x);
						y = to_string((int)item.y);

						if (item.item_type == SE_ObjectType)
						{
							item_type_str = "OBJ";
							id = item.obj->GetObjectID();
						}

						else if (item.item_type == SE_SpriteType)
						{
							item_type_str = "SPR";
							id = item.spr->GetSpriteID();
						}

						else
						{
							// BAD TYPE
						}

						id_size = to_string(id.size());

						ExtendString(layer, 8, "!");
						ExtendString(x, 12, "@");
						ExtendString(y, 12, "#");
						ExtendString(id_size, 8, "$");

						scene_file.write(item_type_str.c_str(), item_type_str.size());
						scene_file.write(layer.c_str(), 8);
						scene_file.write(x.c_str(), 12);
						scene_file.write(y.c_str(), 12);
						scene_file.write(id_size.c_str(), 8);
						scene_file.write(id.c_str(), id.size());

						// Generate a garbage string of 8 characters and write it to the file stream
						// To further limit readability of the outputted scene file
						for (int i = 0; i < 8; i++)
						{
							scene_garbage_str[i] = (char)rand() % 200;
						}

						scene_garbage_str[9] = '\0';
						scene_file.write(scene_garbage_str, 8);
					}
				}

				UnsavedChanges = false;
			}

			else
			{
				ShouldShowFileErrWindow = true;
			}

			scene_file.close();
		}

		void DrawGrid()
		{
			static SpriteBatcher grid_batcher;
			static Texture grid_texture("Core\\Resources\\Scene Editor\\grid_tile.png");
			
			if (ViewGrid)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

				GenericObject obj;
				float x = 0, y = 0, w = GridX, h = GridX;

				glfwGetFramebufferSize(SceneEditorWindow, &SceneEditorWidth, &SceneEditorHeight);
				grid_batcher.StartSpriteBatch(SceneEditorCamera->GetProjectionMatrix());

				obj.texture = &grid_texture;

				int rows = SceneEditorWidth / GridX + 4;
				int cols = SceneEditorHeight / GridY + 4;

				for (int i = 0; i < rows; i++)
				{
					for (int j = 0; j < cols; j++)
					{
						x = i * GridX;
						y = j * GridY;
						w = x + GridX;
						h = y + GridY;

						obj.coords[0] = w;
						obj.coords[1] = y;
						obj.coords[2] = 1.0f;
						obj.coords[3] = w;
						obj.coords[4] = h;
						obj.coords[5] = 1.0f;
						obj.coords[6] = x;
						obj.coords[7] = h;
						obj.coords[8] = 1.0f;
						obj.coords[9] = x;
						obj.coords[10] = y;
						obj.coords[11] = 1.0f;

						grid_batcher.AddGenericObjectToBatch(obj);
					}
				}

				grid_batcher.EndSpriteBatch();

				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}

		void DrawModalWindows()
		{
			// Shows all the modal windows that are flagged true.
			// (Windows such as About me, Support me etc..

			if (ShouldShowAboutTheAuthorWindow)
			{
				ImGui::OpenPopup("About the author");

				if (ImGui::BeginPopupModal("About the author", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ShouldBlockInput = true;

					ImGui::Text("Programmed by Samuel Rasquinha\n");
					ImGui::Text("GitHub : Samuel Rasquinha");
					ImGui::Text("Gmail : samuelrasquinha@gmail.com");
					ImGui::Text("Instagram : Samuel Rasquinha");
					ImGui::Text("Programmed using C++ and OpenGL.");
					ImGui::Text("If you like this program, please consider starring this on github.");

					ImGui::SetItemDefaultFocus();

					if (ImGui::Button("OK", ImVec2(120, 0)))
					{
						ShouldBlockInput = false;
						ShouldShowAboutTheAuthorWindow = false;
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}
			}

			if (ShouldShowSupportMeWindow)
			{
				ImGui::OpenPopup("Support me");

				if (ImGui::BeginPopupModal("Support me", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ShouldBlockInput = true;

					ImGui::Text("If you want to report bugs or request features, send me an email (samuelrasquinha@gmail.com)\n");
					ImGui::Text("The entire GLGame project was made by Samuel Rasquinha (samuelrasquinha@gmail.com)");
					ImGui::Text("Since I am only 14 years old, I don't have paypal. But you can show your support by starring this project on GitHub.");

					ImGui::SetItemDefaultFocus();

					if (ImGui::Button("OK", ImVec2(120, 0)))
					{
						ShouldBlockInput = false;
						ShouldShowSupportMeWindow = false;
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}
			}

			if (ShouldShowWITWindow)
			{
				ImGui::OpenPopup("What is this?");

				if (ImGui::BeginPopupModal("What is this?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ShouldBlockInput = true;

					ImGui::Text("This is a debug tool and scene editor I made to run along with GLGame");
					ImGui::Text("This is to be run during runtime.");

					ImGui::SetItemDefaultFocus();

					if (ImGui::Button("OK", ImVec2(120, 0)))
					{
						ShouldBlockInput = false;
						ShouldShowWITWindow = false;
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}

			}

			if (ShouldShowDependenciesWindow)
			{
				ImGui::OpenPopup("Dependencies");

				if (ImGui::BeginPopupModal("Dependencies", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ShouldBlockInput = true;

					ImGui::Text("GLGame the minimal open-source dependencies. All of the below are open-source and free");
					ImGui::Text("GLGame uses the following dependencies : ");
					ImGui::Text("C++17 (For std::filesystem)");
					ImGui::Text("GLEW (To access OpenGL functions)");
					ImGui::Text("GLFW (Windowing environment)");
					ImGui::Text("ImGui (Bloat free immediate mode GUI interface by Omar Ocurnut)");
					ImGui::Text("stb_image (To read texture files)\n");

					ImGui::SetItemDefaultFocus();

					if (ImGui::Button("OK", ImVec2(120, 0)))
					{
						ShouldBlockInput = false;
						ShouldShowDependenciesWindow = false;
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}
			}

			if (ShouldShowSaveAsWindow)
			{
				ImGui::OpenPopup("Save As");

				if (ImGui::BeginPopupModal("Save As", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ShouldBlockInput = true;

					ImGui::InputText("Path", SceneFilePath, SceneFilePthSize);

					ImGui::SetItemDefaultFocus();

					if (ImGui::Button("OK", ImVec2(120, 0)))
					{
						ShouldBlockInput = false;
						ShouldShowSaveAsWindow = false;
						SceneFilePathSet = true;

						FlushSceneFile();
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}
			}

			if (ShouldShowFileErrWindow)
			{
				ImGui::OpenPopup("File error");

				if (ImGui::BeginPopupModal("File error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ShouldBlockInput = true;

					ImGui::SetItemDefaultFocus();

					ImGui::Text("Error saving file..");
					ImGui::Text("Click OK to continue..");

					if (ImGui::Button("OK", ImVec2(120, 0)))
					{
						ShouldBlockInput = false;
						ShouldShowFileErrWindow = false;
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}
			}
		}

		void RenderSceneEditorItems()
		{
			stbi_set_flip_vertically_on_load(true);

			// Draw all the Scene editor objects that are written in the map
			SceneEditorBatcher->StartSpriteBatch(SceneEditorCamera, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

			for (auto e = SceneEditorItemQueue.begin(); e != SceneEditorItemQueue.end(); e++)
			{
				for (int i = 0; i < e->second.size(); i++)
				{
					if (e->second.at(i).item_type == SE_ObjectType && e->second.at(i).obj != nullptr)
					{
						SceneEditorBatcher->AddGenericTextureToBatch(e->second.at(i).obj->GetSprite()->GetCurrentTexture(), glm::vec3(e->second.at(i).x, e->second.at(i).y, 1.0f));
					}

					else if (e->second.at(i).item_type == SE_SpriteType && e->second.at(i).spr != nullptr)
					{
						SceneEditorBatcher->AddGenericTextureToBatch(e->second.at(i).spr->GetCurrentTexture(), glm::vec3(e->second.at(i).x, e->second.at(i).y, 1.0f));
					}
				}
			}

			SceneEditorBatcher->EndSpriteBatch();

			// Object Drawing ends here..

			// Draw the ghost mouse image

			if (CurrentOperationSelected == PlaceItems)
			{
				if (GhostObjectImage.item_type == SE_ObjectType && GhostObjectImage.obj != nullptr
					&& ItemTypeSelected == ObjectSelection
					&& ObjectIDList != nullptr && RadioObjectSelected != -1)
				{
					string item_id = (ObjectIDList->at(RadioObjectSelected));
					unordered_map<string, Object*>::iterator chk = SceneEditorGlobalObjects->find(item_id);

					if (chk != SceneEditorGlobalObjects->end() && SceneEditorGlobalObjects->at(item_id)->HasSprite())
					{
						SceneEditorBatcher->StartSpriteBatch(SceneEditorCamera, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
						SceneEditorBatcher->AddGenericTextureToBatch(GhostObjectImage.obj->GetSprite()->GetCurrentTexture(), glm::vec3(GhostObjectImage.x, GhostObjectImage.y, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
						SceneEditorBatcher->EndSpriteBatch();
					}
				}

				else if (GhostObjectImage.item_type == SE_SpriteType && GhostObjectImage.spr != nullptr
					&& ItemTypeSelected == SpriteSelection
					&& SpriteIDList != nullptr && RadioSpriteSelected != -1)
				{
					string item_id = (SpriteIDList->at(RadioSpriteSelected));
					unordered_map<string, Sprite*>::iterator chk = SceneEditorGlobalSprites->find(item_id);

					if (chk != SceneEditorGlobalSprites->end())
					{
						SceneEditorBatcher->StartSpriteBatch(SceneEditorCamera, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
						SceneEditorBatcher->AddGenericTextureToBatch(GhostObjectImage.spr->GetCurrentTexture(), glm::vec3(GhostObjectImage.x, GhostObjectImage.y, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
						SceneEditorBatcher->EndSpriteBatch();
					}
				}
			}

			// Drawing the ghost mouse image ends..
		}

		void RenderSceneEditorBackgrounds()
		{
			GenericObject obj;

			glfwGetFramebufferSize(SceneEditorWindow, &SceneEditorWidth, &SceneEditorHeight);
			SceneEditorBatcher->StartSpriteBatch(SceneEditorCamera->GetViewProjectionMatrix());

			for (auto e = SceneEditorBackgroundQueue.begin(); e != SceneEditorBackgroundQueue.end(); e++)
			{
				for (int i = 0; i < e->second.size(); i++)
				{
					if (e->second.at(i)->ShouldStretchToWindow())
					{
						SceneEditorBatcher->AddGenericTextureToBatchCustom((Texture*)&e->second.at(i)->GetTexture(), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2((float)SceneEditorWidth - 10, (float) SceneEditorHeight - 10), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					}

					else if (e->second.at(i)->ShouldStretchToWindow() == false)
					{
						SceneEditorBatcher->AddGenericTextureToBatchCustom((Texture*)&e->second.at(i)->GetTexture(), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2((float)e->second.at(i)->GetTexture().GetWidth() - 10, (float)e->second.at(i)->GetTexture().GetHeight() - 10), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					}
				}
			}

			SceneEditorBatcher->EndSpriteBatch();
		}

		void RenderDebugWindow()
		{
			if (CurrentOperationSelected == DebugGame)
			{
				if (ImGui::Begin("Debug.."))
				{
					ImGui::Text("Current Frame : %ld", DebugInfo->CurrentFrame);
					ImGui::Text("Current Time Step : %lf", DebugInfo->CurrentTS);
					ImGui::Text("Render Time : %lf\n", DebugInfo->RenderTime);
					ImGui::Text("Vertex Count : %d", DebugInfo->VerticesCount);
					ImGui::Text("Indices Count : %d", DebugInfo->IndicesCount);
					ImGui::Text("Quad Count : %d", DebugInfo->QuadCount);
					ImGui::Text("Objects Drawn : %d", DebugInfo->ObjectsDrawn);
					ImGui::Text("Sprites Drawn : %d", DebugInfo->SpritesDrawn);
					ImGui::Text("Lights Drawn : %d", DebugInfo->LightsDrawn);
					ImGui::Text("Total Quad Count : %d", DebugInfo->TotalQuads);
					ImGui::Text("Total Vertices : %d", DebugInfo->TotalVertices);
					ImGui::Text("Total Indices : %d", DebugInfo->TotalIndices);
					ImGui::Text("Total Draw Calls (Backgrounds + Items) : %d", DebugInfo->TotalDrawCalls);
				}

				ImGui::End();
			}
		}

		void DrawFloatingWidgets()
		{
			static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
			ImGui::SetNextWindowPos(ImVec2(SceneEditorWidth * 0.75, 20), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);

			if (ImGui::Begin("Operation select."))
			{
				ImGui::Text("What would you like to do ?");
				ImGui::RadioButton("Place GLGame::Items", &CurrentOperationSelected, PlaceItems);
				ImGui::RadioButton("Change/Add Backgrounds", &CurrentOperationSelected, ChangeBackground);
				ImGui::RadioButton("See properties of a GLGame::Object or GLGame::Sprite", &CurrentOperationSelected, SeeProperties);
				ImGui::RadioButton("Debug", &CurrentOperationSelected, DebugGame);
				ImGui::RadioButton("View or Look at the scene.", &CurrentOperationSelected, ViewScene);
				ImGui::Text("\n\nEditor settings : \n\n"); 
				ImGui::Separator();
				ImGui::Checkbox("View Grid", &ViewGrid);
				ImGui::Checkbox("Snap to grid", &ShouldSnap);

				if (ViewGrid)
				{
					ImGui::InputInt("Grid Size X (in pixels)", &GridX);
					ImGui::InputInt("Grid Size Y (in pixels)", &GridY); 

					if (GridX < 16)
					{
						GridX = 16;
					}

					else if (GridX > 256)
					{
						GridX = 256;
					}

					if (GridY < 16)
					{
						GridY = 16;
					}

					else if (GridY > 256)
					{
						GridY = 256;
					}
				}
				
				if (GridX < 0)
				{
					GridX = 0;
				}

				if (GridY < 0)
				{
					GridY = 0;
				}

				ImGui::Text("\n");
				ImGui::Separator();
				ImGui::Text("\n%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}

			ImGui::End();
		}

		void RenderBackgroundAddWidget()
		{
			struct background_map_data_element
			{
				string id;
				int layer;
				int vector_element;
			};

			static int bgl;
			static int allocation_size = BackgroundIDList->size();
			static char** bg_items = new char* [2048];
			static char* bg_button = new char [2048];
			static char* bg_remove_label = new char[256];
			static char* bg_display_label = new char[512];
			static int erase_element = 0;
			static bool first_run = true;

			if (first_run == true)
			{
				first_run = false;
				memset(bg_display_label, '\0', 512);
				memset(bg_remove_label, '\0', 256);
				memset(bg_button, '\0', 2048);
			}
			
			vector<background_map_data_element> background_map_contents;

			if (CurrentOperationSelected == ChangeBackground)
			{
				if (ImGui::Begin("Add Backgrounds"))
				{
					if (ImGui::CollapsingHeader("Add Backgrounds"))
					{
						for (int i = 0; i < BackgroundIDList->size(); i++)
						{
							bg_items[i] = (char*)BackgroundIDList->at(i).c_str();
						}

						ImGui::Text("Backgrounds : \n\n");
						ImGui::ListBox("", &BackgroundSelected, bg_items, BackgroundIDList->size(), 10);
				
						ImGui::Separator();
						ImGui::Text("Current Background Selected : %s", BackgroundIDList->at(BackgroundSelected).c_str());
						ImGui::NewLine();
						
						sprintf(bg_button, "Add \"%s\" at layer (%d)", BackgroundIDList->at(BackgroundSelected).c_str(), bgl);

						if (ImGui::Button((const char*)bg_button))
						{
							if (BackgroundSelected >= 0 && BackgroundSelected < BackgroundIDList->size())
							{
								// Write the data for undo'ing the background operation

								LastOperationCompleted.last_operation = ChangeBackground;
								LastOperationCompleted.layer = bgl;
								LastOperationCompleted.id = BackgroundIDList->at(BackgroundSelected);
								
								// The location of the pushed element will be at the end of the vector 
								LastOperationCompleted.element = SceneEditorBackgroundQueue[bgl].size();
								LastOperationCompleted.erase = true;

								SceneEditorBackgroundQueue[bgl].push_back(SceneEditorGlobalBackgrounds->at(BackgroundIDList->at(BackgroundSelected)));
								
								sprintf(bg_display_label, "Added \"%s\" successfully to Layer (%d)", BackgroundIDList->at(BackgroundSelected).c_str(), bgl);
							}

							else
							{
								sprintf(bg_display_label, "Error adding background!");
							}
						}

						ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.4f, 1.0f), bg_display_label);
						ImGui::Text("\n\n");

						ImGui::Separator();
						ImGui::InputInt("Background Layer", &bgl);	
					}

					ImGui::Text("\n");
					ImGui::Separator();
					ImGui::Text("\n");

					if (ImGui::CollapsingHeader("Remove Backgrounds"))
					{
						// Write the map contents to the vector so it becomes easier to display it

						for (auto e = SceneEditorBackgroundQueue.begin(); e != SceneEditorBackgroundQueue.end(); e++)
						{
							background_map_data_element element_data;

							for (int i = 0; i < e->second.size(); i++)
							{
								element_data.id = e->second.at(i)->GetBackgroundID();
								element_data.layer = e->first;
								element_data.vector_element = i;
								background_map_contents.push_back(element_data);
							}
						}

						for (int i = 0; i < background_map_contents.size(); i++)
						{
							ImGui::RadioButton(background_map_contents.at(i).id.c_str(), &erase_element, i);
						}

						ImGui::Text("\n");

						if (background_map_contents.size() > 0 && erase_element >= 0 && erase_element < background_map_contents.size())
						{
							sprintf(bg_remove_label, "Erase \"%s\" from Layer(%d)", background_map_contents.at(erase_element).id.c_str(), background_map_contents.at(erase_element).layer);

							if (ImGui::Button(bg_remove_label))
							{
								LastOperationCompleted.last_operation = ChangeBackground;
								LastOperationCompleted.layer = background_map_contents.at(erase_element).layer;
								LastOperationCompleted.id = background_map_contents.at(erase_element).id;

								// The location of the pushed element will be at the end of the vector 
								LastOperationCompleted.element = SceneEditorBackgroundQueue[bgl].size();
								LastOperationCompleted.erase = false;

								SceneEditorBackgroundQueue.at(background_map_contents.at(erase_element).layer).erase(
									SceneEditorBackgroundQueue.at(background_map_contents.at(erase_element).layer).begin() + background_map_contents.at(erase_element).vector_element);
							}
						}

						else
						{
							ImGui::Button("No Backgrounds to erase!");
						}
					}
				}

				ImGui::End();
			}
		}

		void DrawPlaceItemWindowWidget()
		{
			static bool show_window = true;

			if (CurrentOperationSelected == PlaceItems)
			{
				// Reset the scene editor :
				SceneEditorCamera->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
				SceneEditorCamera->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

				if (ImGui::Begin("Place GLGame::SceneItems", &show_window))
				{
					if (ImGui::CollapsingHeader("Place Scene Items"))
					{
						if (ItemTypeSelected == ObjectSelection)
						{
							if (ImGui::TreeNode("Objects"))
							{
								string obj_name_holder;

								for (int i = 0; i < ObjectIDList->size(); i++)
								{
									obj_name_holder = ObjectIDList->at(i);
									obj_name_holder.erase(obj_name_holder.begin(), obj_name_holder.begin() + 5);
									ImGui::RadioButton(obj_name_holder.c_str(), &RadioObjectSelected, i);
								}

								ImGui::TreePop();
							}
						}

						else if (ItemTypeSelected == SpriteSelection)
						{
							if (ImGui::TreeNode("Sprites"))
							{
								string spr_name_holder;

								for (int i = 0; i < SpriteIDList->size(); i++)
								{
									spr_name_holder = SpriteIDList->at(i);
									spr_name_holder.erase(spr_name_holder.begin(), spr_name_holder.begin() + 5);
									ImGui::RadioButton(spr_name_holder.c_str(), &RadioSpriteSelected, i);
								}

								ImGui::TreePop();
							}
						}

						ImGui::NewLine();
						ImGui::Separator();

						ImGui::Text("\nTYPE OF SCENE ITEM THAT YOU WOULD LIKE TO PLACE : \n\n");
						ImGui::RadioButton("GLGame::Object", &ItemTypeSelected, ObjectSelection);
						ImGui::RadioButton("GLGame::Sprite", &ItemTypeSelected, SpriteSelection);
						// Draw the "Selected Item" ImGui window
					}

					ImGui::Text("\n\n");
					ImGui::InputInt("Layer/Depth", &CurrentSceneEditorLayer);
				}

				ImGui::End();

				if (CurrentOperationSelected == PlaceItems)
				{
					ImGuiWindowFlags selected_item_window_flags = 0;
					bool selected_item_show_window = true;

					selected_item_window_flags |= ImGuiWindowFlags_NoScrollbar;
					selected_item_window_flags |= ImGuiWindowFlags_MenuBar;
					selected_item_window_flags |= ImGuiWindowFlags_NoResize;
					selected_item_window_flags |= ImGuiWindowFlags_NoCollapse;
					selected_item_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
					selected_item_window_flags |= ImGuiWindowFlags_NoDocking;

					ImGui::SetNextWindowPos(ImVec2(SceneEditorWidth - 270, SceneEditorHeight - 270), ImGuiCond_FirstUseEver);
					ImGui::SetNextWindowSize(ImVec2(250, 250), ImGuiCond_Always);

					if (ImGui::Begin("Selected Item", &selected_item_show_window, selected_item_window_flags))
					{
						if (ItemTypeSelected == ObjectSelection && RadioObjectSelected > 0 && RadioObjectSelected < ObjectIDList->size())
						{
							Texture* selected_item_texture;

							if (SceneEditorGlobalObjects->at(ObjectIDList->at(RadioObjectSelected))->HasSprite())
							{
								selected_item_texture = SceneEditorGlobalObjects->at(ObjectIDList->at(RadioObjectSelected))->GetSprite()->GetCurrentTexture();
							
								// Add the item to the imgui draw list
								ImGui::GetWindowDrawList()->AddImage(
									(void*)selected_item_texture->GetTextureID(),
									ImVec2(ImGui::GetCursorScreenPos()),
									ImVec2(ImGui::GetCursorScreenPos().x + selected_item_texture->GetWidth(),
									ImGui::GetCursorScreenPos().y + selected_item_texture->GetHeight()),
									ImVec2(0, 0),
									ImVec2(1, 1));
							}
						}

						if (ItemTypeSelected == SpriteSelection && RadioSpriteSelected > 0 && RadioSpriteSelected < SpriteIDList->size())
						{
							Texture* selected_item_texture;

							if (SceneEditorGlobalSprites->at(SpriteIDList->at(RadioSpriteSelected))->GetCurrentTexture() != nullptr)
							{
								selected_item_texture = SceneEditorGlobalSprites->at(SpriteIDList->at(RadioSpriteSelected))->GetCurrentTexture();
								
								// Add the item to the imgui draw list
								ImGui::GetWindowDrawList()->AddImage(
									(void*)selected_item_texture->GetTextureID(),
									ImVec2(ImGui::GetCursorScreenPos()),
									ImVec2(ImGui::GetCursorScreenPos().x + selected_item_texture->GetWidth(),
										ImGui::GetCursorScreenPos().y + selected_item_texture->GetHeight()),
									ImVec2(0, 0),
									ImVec2(1, 1));
							}
						}
					}

					ImGui::End();

				}
			}
		}

		void DrawMenuBar()
		{
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Save As", ""))
					{
						ShouldShowSaveAsWindow = true;
					}

					if (ImGui::MenuItem("Save", ""))
					{
						if (SceneFilePathSet == false)
						{
							ShouldShowSaveAsWindow = true;
						}

						else
						{
							fstream scene_file;

							scene_file.open(string(SceneFilePath), ios::out | ios::binary);

							if (scene_file.good() && scene_file.is_open())
							{
								scene_file.close();
								FlushSceneFile();
							}

							else
							{
								ShouldShowFileErrWindow = true;
							}
						}
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Edit"))
				{
					if (ImGui::MenuItem("Undo", "CTRL+Z"))
					{
						// Undo
					}

					if (ImGui::MenuItem("Redo", "CTRL+Y", false, false))
					{

					}

					ImGui::Separator();

					if (ImGui::MenuItem("Cut", "CTRL+X"))
					{

					}

					if (ImGui::MenuItem("Copy", "CTRL+C"))
					{

					}

					if (ImGui::MenuItem("Paste", "CTRL+V"))
					{

					}


					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Help"))
				{
					if (ImGui::MenuItem("About the author"))
					{
						ShouldShowAboutTheAuthorWindow = true;
					}

					else
					{
						ShouldShowAboutTheAuthorWindow = false;
					}

					if (ImGui::MenuItem("What is this?"))
					{
						ShouldShowWITWindow = true;
					}

					else
					{
						ShouldShowWITWindow = false;
					}


					if (ImGui::MenuItem("Dependencies"))
					{
						ShouldShowDependenciesWindow = true;
					}

					else
					{
						ShouldShowDependenciesWindow = false;
					}

					if (ImGui::MenuItem("Support"))
					{
						ShouldShowSupportMeWindow = true;
					}

					else
					{
						ShouldShowSupportMeWindow = false;
					}

					ImGui::EndMenu();
				}

				ImGui::EndMainMenuBar();
			}
		}

		void _SetSceneEditorCloseFlag(bool val)
		{
			SE_window_destroyed = true;
		}

		bool RenderSceneEditor()
		{
			if (SE_window_destroyed == false)
			{
				glfwMakeContextCurrent(SceneEditorWindow);
				glfwGetFramebufferSize(SceneEditorWindow, &SceneEditorWidth, &SceneEditorWidth);
				glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);

				// Draw the ImGui items
				ImGui::SetCurrentContext(imcontext);

				if (ImGui::IsAnyWindowHovered())
				{
					ShouldBlockInput = true;
				}

				else
				{
					ShouldBlockInput = false;
				}

				RenderSceneEditorBackgrounds();
				RenderSceneEditorItems();

				bool display_title_place_item = true;
				bool display_title_selected_item = true;

				ImGui::SetNextWindowPos(ImVec2(SceneEditorWidth * 0.75, 20), ImGuiCond_FirstUseEver);
				ImGui::SetNextWindowSize(ImVec2(555, 760), ImGuiCond_FirstUseEver);

				ImGui::SetCurrentContext(imcontext);
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();

				glfwGetFramebufferSize(SceneEditorWindow, &SceneEditorWidth, &SceneEditorHeight);

				DrawMenuBar();
				DrawGrid();
				DrawModalWindows();
				DrawFloatingWidgets();
				DrawPlaceItemWindowWidget();
				RenderBackgroundAddWidget();
				RenderDebugWindow();

				// Scope : show modal window when you click on the close button
				{
					if (ShouldShowCloseModalWindow)
					{
						if (UnsavedChanges)
						{
							ImGui::SetNextWindowSize(ImVec2(600, 100), ImGuiCond_Always);
							ImGui::OpenPopup("Delete?");

							if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
							{
								ImGui::Text("The Scene you just created will be lost forever! This action cannot be undone!\n");
								ImGui::Separator();
								ShouldBlockInput = true;

								if (ImGui::Button("OK", ImVec2(120, 0)))
								{
									ShouldBlockInput = false;
									ImGui::CloseCurrentPopup();

									if (SE_window_destroyed == false)
									{
										SE_window_destroyed = true;
										glfwDestroyWindow(SceneEditorWindow);
										Log::LogToConsole("The Scene Editor was destroyed!");
									}

									return 0;
								}

								ImGui::SetItemDefaultFocus();
								ImGui::SameLine();

								if (ImGui::Button("Cancel", ImVec2(120, 0)))
								{
									ShouldBlockInput = false;
									ShouldShowCloseModalWindow = false;
									ImGui::CloseCurrentPopup();
								}

								ImGui::EndPopup();
							}
						}

						else
						{
							ShouldBlockInput = false;

							if (SE_window_destroyed == false)
							{
								SE_window_destroyed = true;
								glfwDestroyWindow(SceneEditorWindow);
								Log::LogToConsole("The Scene Editor was destroyed!");
							}

							return 0;
						}
					}
				}


				ImGui::Render();
				ImGui::EndFrame();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

				glfwSwapBuffers(SceneEditorWindow);
				return true;
			}

			else
			{
				return false;
			}

			return false;
		}

		bool SceneEditorAlive()
		{
			return SE_window_destroyed;
		}

		void SEWindowResizeCallback(GLFWwindow* window, int width, int height)
		{
			if (window != SceneEditorWindow)
			{
				return;
			}

			glfwGetFramebufferSize(window, &WindowSizeX, &WindowSizeY);
			glfwGetCursorPos(window, &MousePosX, &MousePosY);

			// Set the view port
			glViewport(0, 0, WindowSizeX, WindowSizeY);
		}

		// function to snap an object to a particular grid
		float SnapToGrid(int value, int size)
		{
			if (!ShouldSnap)
			{
				size = 1;
			}

			if (size == 0)
			{
				size = 1;
			}

			int temp = value % size;

			if (temp < (size / 2))
			{
				return (value - temp);
			}

			else
			{
				return (value + size) - temp;
			}
		}

		void SEKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			if (window != SceneEditorWindow)
			{
				return;
			}

			if (ShouldBlockInput)
			{
				return;
			}

			glfwGetCursorPos(window, &MousePosX, &MousePosY);

			if (action == GLFW_PRESS)
			{
				if (key == GLFW_KEY_S && mods & GLFW_MOD_CONTROL)
				{
					if (SceneFilePathSet == false)
					{
						ShouldShowSaveAsWindow = true;

					}

					else
					{
						fstream scene_file;

						scene_file.open(string(SceneFilePath), ios::out | ios::binary);

						if (scene_file.good() && scene_file.is_open())
						{
							scene_file.close();
							FlushSceneFile();
						}

						else
						{
							ShouldShowFileErrWindow = true;
						}
					}
				}

				if (key == GLFW_KEY_Z && mods & GLFW_MOD_CONTROL)
				{
					Undo();
				}
			}

			else if (action == GLFW_RELEASE)
			{

			}

			else
			{

			}
		}

		void SECursorPosCallback(GLFWwindow* window, double xpos, double ypos)
		{
			if (window != SceneEditorWindow)
			{
				return;
			}

			if (ShouldBlockInput)
			{
				return;
			}

			if (CurrentOperationSelected == PlaceItems)
			{
				// Draw a ghost image of the current selected item

				if (ItemTypeSelected == ObjectSelection)
				{
					if (ObjectIDList != nullptr && RadioObjectSelected != -1 && RadioObjectSelected < ObjectIDList->size() && RadioObjectSelected >= 0)
					{
						int width, height;
						string item_id;

						item_id = (ObjectIDList->at(RadioObjectSelected));
						unordered_map<string, Object*>::iterator chk = SceneEditorGlobalObjects->find(item_id);

						// If the object exists in the map
						if (chk != SceneEditorGlobalObjects->end())
						{
							if (SceneEditorGlobalObjects->at(item_id)->HasSprite())
							{
								GhostObjectImage.item_type = SE_ObjectType;
								GhostObjectImage.obj = SceneEditorGlobalObjects->at(item_id);

								// Get the width and height of the textures
								width = GhostObjectImage.obj->GetSprite()->GetCurrentTextureWidth();
								height = GhostObjectImage.obj->GetSprite()->GetCurrentTextureWidth();

								// Subtract width/2 and height/2 from MouseX and MouseY so that the origin of the object is in the center

								GhostObjectImage.x = (float)xpos - ((int)width / 2);
								GhostObjectImage.y = (float)(SceneEditorHeight - ypos);
								GhostObjectImage.y -= ((int)height / 2);

								// Snap the Ghost object to the grid
								GhostObjectImage.x = SnapToGrid(GhostObjectImage.x, GridX);
								GhostObjectImage.y = SnapToGrid(GhostObjectImage.y, GridY);
							}
						}
					}
				}

				else if (ItemTypeSelected == SpriteSelection)
				{
					if (SpriteIDList != nullptr && RadioSpriteSelected != -1 && RadioSpriteSelected < SpriteIDList->size() && RadioSpriteSelected >= 0)
					{
						int width, height;
						string item_id;

						item_id = (SpriteIDList->at(RadioSpriteSelected));
						unordered_map<string, Sprite*>::iterator chk = SceneEditorGlobalSprites->find(item_id);

						// If the object exists in the map
						if (chk != SceneEditorGlobalSprites->end())
						{
							GhostObjectImage.item_type = SE_SpriteType;
							GhostObjectImage.spr = SceneEditorGlobalSprites->at(item_id);

							width = GhostObjectImage.spr->GetCurrentTextureWidth();
							height = GhostObjectImage.spr->GetCurrentTextureWidth();

							// Subtract width/2 and height/2 from MouseX and MouseY so that the origin of the object is in the center

							GhostObjectImage.x = (float)xpos - ((int)width / 2);
							GhostObjectImage.y = (float)(SceneEditorHeight - ypos);
							GhostObjectImage.y -= ((int)height / 2);

							// Snap the Ghost object to the grid
							GhostObjectImage.x = SnapToGrid(GhostObjectImage.x, GridX);
							GhostObjectImage.y = SnapToGrid(GhostObjectImage.y, GridY);
						}
					}
				}
			}

			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
			{
				return;
			}

			// Sneaky way to handle mouse drag events
			if (CurrentOperationSelected == ViewScene)
			{
				SceneEditorCamera->SetPosition(glm::vec3(StartPanX - xpos, ypos - StartPanY, 1.0f));
			}
		}

		void SEMouseCallback(GLFWwindow* window, int button, int action, int mods)
		{
			if (window != SceneEditorWindow)
			{
				return;
			}

			if (ShouldBlockInput)
			{
				return;
			}

			glfwGetCursorPos(window, &MousePosX, &MousePosY);

			if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT)
			{
				// Set the unsaved changes flag
				UnsavedChanges = true;

				StartPanX = MousePosX;
				StartPanY = MousePosY;

				if (CurrentOperationSelected == PlaceItems)
				{
					if (ItemTypeSelected == ObjectSelection &&
						RadioObjectSelected < ObjectIDList->size() && RadioObjectSelected > -1)
					{
						if (ObjectIDList != nullptr)
						{
							SceneEditorRenderItem item;
							int width, height;
							string item_id;

							item_id = (ObjectIDList->at(RadioObjectSelected));

							unordered_map<string, Object*>::iterator chk = SceneEditorGlobalObjects->find(item_id);

							// If the object exists in the map
							if (chk != SceneEditorGlobalObjects->end())
							{
								if (SceneEditorGlobalObjects->at(item_id)->HasSprite())
								{
									item.item_type = SE_ObjectType;
									item.obj = SceneEditorGlobalObjects->at(item_id);

									// Get the width and height of the textures
									width = item.obj->GetSprite()->GetCurrentTextureWidth();
									height = item.obj->GetSprite()->GetCurrentTextureHeight();

									// Subtract width/2 and height/2 from MouseX and MouseY so that the origin of the object is in the center

									item.x = (float)MousePosX - ((int)width / 2);
									item.y = (float)(SceneEditorHeight - MousePosY);
									item.y -= ((int)height / 2);

									// Snap the X and Y to the grid
									item.x = SnapToGrid(item.x, GridX);
									item.y = SnapToGrid(item.y, GridY);

									item.layer = CurrentSceneEditorLayer;
									SceneEditorItemQueue[item.layer].push_back(item);

									// Writing the required data in the struct
									// For undo/redo
									LastOperationCompleted.erase = true;
									LastOperationCompleted.element = SceneEditorItemQueue[item.layer].size() - 1;
									LastOperationCompleted.last_operation = PlaceItems;
									LastOperationCompleted.id = "";
									LastOperationCompleted.item = item;
								}
							}
						}
					}

					else if (ItemTypeSelected == SpriteSelection &&
						RadioSpriteSelected < SpriteIDList->size() && RadioSpriteSelected > -1)
					{
						if (SpriteIDList != nullptr)
						{
							SceneEditorRenderItem item;
							int width, height;
							string item_id;

							item_id = (SpriteIDList->at(RadioSpriteSelected));

							unordered_map<string, Sprite*>::iterator chk = SceneEditorGlobalSprites->find(item_id);

							// If the object exists in the map
							if (chk != SceneEditorGlobalSprites->end())
							{
								if (SceneEditorGlobalSprites->at(item_id))
								{
									item.item_type = SE_SpriteType;
									item.spr = SceneEditorGlobalSprites->at(item_id);

									// Get the width and height of the textures
									width = item.spr->GetCurrentTextureWidth();
									height = item.spr->GetCurrentTextureHeight();

									// Subtract width/2 and height/2 from MouseX and MouseY so that the origin of the object is in the center

									item.x = (float)MousePosX - ((int)width / 2);
									item.y = (float)(SceneEditorHeight - MousePosY);
									item.y -= ((int)height / 2);

									// Snap the X and Y to the grid
									item.x = SnapToGrid(item.x, GridX);
									item.y = SnapToGrid(item.y, GridY);

									item.layer = CurrentSceneEditorLayer;
									SceneEditorItemQueue[item.layer].push_back(item);
									LastOperationCompleted.erase = true;
									LastOperationCompleted.element = SceneEditorItemQueue[item.layer].size() - 1;
									LastOperationCompleted.last_operation = PlaceItems;
									LastOperationCompleted.id = "";
									LastOperationCompleted.item = item;
								}
							}
						}
					}
				}
			}

			if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT)
			{
				// Set the unsaved changes flag
				UnsavedChanges = true;

				AABB o1, o2;

				o1.x = (float)MousePosX;
				o1.y = (float)(SceneEditorHeight - MousePosY);
				o1.w = 4;
				o1.h = 4;

				for (int i = 0; i < SceneEditorItemQueue[CurrentSceneEditorLayer].size(); i++)
				{
					o2.x = SceneEditorItemQueue[CurrentSceneEditorLayer].at(i).x;
					o2.y = SceneEditorItemQueue[CurrentSceneEditorLayer].at(i).y;

					if (SceneEditorItemQueue[CurrentSceneEditorLayer].at(i).item_type == SE_ObjectType)
					{
						o2.w = SceneEditorItemQueue[CurrentSceneEditorLayer].at(i).obj->GetSprite()->GetCurrentTextureWidth();
						o2.h = SceneEditorItemQueue[CurrentSceneEditorLayer].at(i).obj->GetSprite()->GetCurrentTextureHeight();

						// If the mouse collided with the object, erase it from the editor queue
						if (CheckAABBCollision(o1, o2))
						{
							// Write some data in the undo structure
							// For undo/redo
							LastOperationCompleted.erase = false;
							LastOperationCompleted.element = i;
							LastOperationCompleted.last_operation = PlaceItems;
							LastOperationCompleted.id = "";
							LastOperationCompleted.item = SceneEditorItemQueue[CurrentSceneEditorLayer].at(i);
							LastOperationCompleted.layer = CurrentSceneEditorLayer;

							SceneEditorItemQueue[CurrentSceneEditorLayer].erase(SceneEditorItemQueue[CurrentSceneEditorLayer].begin() + i);
							
							break;
						}
					}

					else if (SceneEditorItemQueue[CurrentSceneEditorLayer].at(i).item_type == SE_SpriteType)
					{
						o2.w = SceneEditorItemQueue[CurrentSceneEditorLayer].at(i).spr->GetCurrentTextureWidth();
						o2.h = SceneEditorItemQueue[CurrentSceneEditorLayer].at(i).spr->GetCurrentTextureHeight();

						if (CheckAABBCollision(o1, o2))
						{
							// Write some data in the undo structure
							// For undo/redo
							LastOperationCompleted.erase = false;
							LastOperationCompleted.element = i;
							LastOperationCompleted.last_operation = PlaceItems;
							LastOperationCompleted.id = "";
							LastOperationCompleted.item = SceneEditorItemQueue[CurrentSceneEditorLayer].at(i);
							LastOperationCompleted.layer = CurrentSceneEditorLayer;

							SceneEditorItemQueue[CurrentSceneEditorLayer].erase(SceneEditorItemQueue[CurrentSceneEditorLayer].begin() + i);

							break;
						}
					}
				}
			}

			else if (action == GLFW_RELEASE)
			{

			}

			else
			{

			}
		}

		void SEScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
		{
			if (window != SceneEditorWindow)
			{
				return;
			}

			if (ShouldBlockInput)
			{
				return;
			}

			glfwGetCursorPos(window, &MousePosX, &MousePosY);

			if (CurrentOperationSelected == ViewScene)
			{
				if (yoffset < 0)
				{
					const glm::vec3 scale = SceneEditorCamera->GetScale();
					SceneEditorCamera->SetScale(glm::vec3(scale.x - 0.1, scale.y - 0.1, 1.0f));
				}

				else if (yoffset > 0)
				{
					const glm::vec3 scale = SceneEditorCamera->GetScale();
					SceneEditorCamera->SetScale(glm::vec3(scale.x + 0.1, scale.y + 0.1, 1.0f));
				}
			}
		}

		void SEWindowCloseCallback(GLFWwindow* window)
		{
			if (window != SceneEditorWindow)
			{
				return;
			}

			ShouldShowCloseModalWindow = true;
		}
	}
}

// Scene Editor End..