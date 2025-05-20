#include "GE_pch.h"

#include "GE_Game.h"

#include "GE_Logger.h"
#include "GE_LevelLoader.h"

#include "GE_MovementSystem.h"
#include "GE_RenderSystem.h"
#include "GE_AnimationSystem.h"
#include "GE_CollisionSystem.h"
#include "GE_RenderColliderSystem.h"
#include "GE_DamageSystem.h"
#include "GE_KeyboardControlSystem.h"
#include "GE_CameraMovementSystem.h"
#include "GE_ProjectileEmitSystem.h"
#include "GE_ProjectileLifeCycleSystem.h"
#include "GE_RenderTextSystem.h"
#include "GE_RenderHealthBarSystem.h"
#include "GE_RenderGUISystem.h"
#include "GE_ScriptSystem.h"

#include "GE_LeftMouseButtonDownEvent.h"
#include "GE_LeftMouseButtonUpEvent.h"

int Game::m_windowWidth = 1680;
int Game::m_windowHeight = 1050;
int Game::m_mapWidth = 0;
int Game::m_mapHeight = 0;

Game::Game()
    : m_IsRunning(false)
    , m_camera(std::make_unique<SDL_Rect>())
    , m_assetStore(std::make_unique<AssetStore>())
    , m_registry(std::make_unique<Registry>())
    , m_eventBus(std::make_unique<EventBus>())
    , m_backgroundColor(std::make_unique<SDL_Color>())
    , m_lua()
{
    Logger::Log("game created");
}

void Game::Initialize()
{
    // init SDL
    const bool errorInitializingSDL = SDL_Init(SDL_INIT_EVERYTHING) != 0;
    if (errorInitializingSDL)
    {
        Logger::Error("Error initializing SDL" );
        return;
    }

    const bool errorInitializingSDL_TTF = TTF_Init() != 0;
    if (errorInitializingSDL_TTF)
    {
        Logger::Error("Error initializing SDL TTF");
        return;
    }
    // init window
    // setting the w and h here before using SDL_SetWindowFullscreen()
    // will set the resolution of the window
    m_window = SDL_CreateWindow(
        "2D Game Engine - Tiny Steps Game Dev",
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        m_windowWidth,
        m_windowHeight,
        NULL
        );
    if(!m_window)
    {
        Logger::Error("Error creating SDL window");
        return;
    }

    // init renderer
    m_renderer = SDL_CreateRenderer(m_window, -1, 0);
    if (!m_renderer)
    {
        Logger::Error("Error creating SDL renderer");
        return;
    }

    // initialize Imgui
    ImGui::CreateContext();
    ImGuiSDL::Initialize(m_renderer, m_windowWidth, m_windowHeight);

    // initialize the camera view with the entire screen area
    SDL_Rect dsd{ 0,0, m_windowWidth, m_windowHeight };
    m_camera = std::make_unique<SDL_Rect>(dsd);

    //SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
    m_IsRunning = true;
}

void Game::ProcessInput()
{
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent))
    {
        // imgui sdl input
        ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
        ImGuiIO& io = ImGui::GetIO();
        {
            // imgui mouse needs
            int mouseX, mouseY;
            const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
            io.MousePos = ImVec2(mouseX, mouseY);
            io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
            io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
        }

        switch (sdlEvent.type)
        {
        case SDL_QUIT:
            m_IsRunning = false;
            break;
        case SDL_KEYDOWN:
            if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
            {
                m_IsRunning = false;
            }
            else if (sdlEvent.key.keysym.sym == SDL_KeyCode::SDLK_QUOTE)
            {
                m_shouldRenderDebug = !m_shouldRenderDebug;
            }
            m_eventBus->EmitEvent<KeyPressedEvent>(sdlEvent.key.keysym.sym);
            break;
        case SDL_MOUSEBUTTONUP:
            m_eventBus->EmitEvent<LeftMouseButtonUpEvent>(*m_camera);
            break;
        case SDL_MOUSEBUTTONDOWN:
            m_eventBus->EmitEvent<LeftMouseButtonDownEvent>(*m_camera);
            break;
        }
    }
}

void Game::Setup()
{
    // add system to the game
    m_registry->AddSystem<MovementSystem>();
    m_registry->AddSystem<RenderSystem>();
    m_registry->AddSystem<AnimationSystem>();
    m_registry->AddSystem<CollisionSystem>();
    m_registry->AddSystem<RenderColliderSystem>();
    m_registry->AddSystem<DamageSystem>();
    m_registry->AddSystem<KeyboardControlSystem>();
    m_registry->AddSystem<CameraMovementSystem>();
    m_registry->AddSystem<ProjectileEmitSystem>();
    m_registry->AddSystem<ProjectileLifeCycleSystem>();
    m_registry->AddSystem<RenderTextSystem>();
    m_registry->AddSystem<RenderHealthBarSystem>();
    m_registry->AddSystem<RenderGUISystem>();
    m_registry->AddSystem<ScriptSystem>();

    m_registry->GetSystem<ScriptSystem>().CreateLuaFunctionBindings(m_lua);

    LevelLoader levelLoader;
    m_lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::os);
   // levelLoader.LoadLevel(2, m_registry, m_assetStore, m_renderer, m_lua);
    levelLoader.LoadLevel("PlayerPrototype", m_registry, m_assetStore, m_renderer, m_lua);
    SDL_SetWindowSize(m_window, Game::m_windowWidth, Game::m_windowHeight);
    SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    // set background color
    {
        SDL_Color backgroundColor;

        const sol::optional<sol::table> backgroundColorDefined = m_lua["level_setup"]["background_color"];
        if (backgroundColorDefined != sol::nullopt)
        {
            const auto backgroundColorTable = m_lua["level_setup"]["background_color"];
            backgroundColor = SDL_Color{ backgroundColorTable["r"], backgroundColorTable["g"], backgroundColorTable["b"], backgroundColorTable["a"] };

        }
        else
        {
            Logger::Error("background color not defined on level definition file. Should be [\"level_setup\"][\"background_color\"]");
            backgroundColor = SDL_Color{ 200, 200, 200, 255 };
        }
        m_backgroundColor = std::make_unique<SDL_Color>(backgroundColor);
        SDL_SetRenderDrawColor(m_renderer, m_backgroundColor->r, m_backgroundColor->g, m_backgroundColor->b, m_backgroundColor->a);
    }
    
}

void Game::Update()
{
    // frame management
    if (CONST::FRAMERATE::IS_CAPPED)
    {
        int millisecondsToWait = CONST::FRAMERATE::MILLISECS_PER_FRAME - (SDL_GetTicks() - m_millisecondsPreviousFrame);
        if (millisecondsToWait > 0 && millisecondsToWait <= CONST::FRAMERATE::MILLISECS_PER_FRAME)
        {
            SDL_Delay(millisecondsToWait);
        }
    }

    double deltaTime = (SDL_GetTicks() - m_millisecondsPreviousFrame) * 0.001f;
    m_millisecondsPreviousFrame = SDL_GetTicks();
    
    // subscription to events will only be alive during one frame
    // suboptimal, should def be changed
    m_eventBus->Reset();
    m_registry->GetSystem<MovementSystem>().SubscribeToEvents(m_eventBus);
    m_registry->GetSystem<DamageSystem>().SubscribeToEvents(m_eventBus);
    m_registry->GetSystem<KeyboardControlSystem>().SubscribeToEvents(m_eventBus);
    m_registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(m_eventBus);

    // update the registry to add or remove entities that were waiting for the end of the frame
    m_registry->Update();

    m_registry->GetSystem<KeyboardControlSystem>().Update(deltaTime);
    m_registry->GetSystem<MovementSystem>().Update(deltaTime);
    m_registry->GetSystem<AnimationSystem>().Update();
    m_registry->GetSystem<CollisionSystem>().Update(m_eventBus);
    m_registry->GetSystem<ProjectileEmitSystem>().Update(m_registry);
    m_registry->GetSystem<ProjectileLifeCycleSystem>().Update(deltaTime);
    m_registry->GetSystem<CameraMovementSystem>().Update(*m_camera);
    m_registry->GetSystem<ScriptSystem>().Update(deltaTime, SDL_GetTicks());
}

void Game::Run()
{
    Setup();
    while (m_IsRunning)
    {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::Render()
{
    SDL_SetRenderDrawColor(m_renderer, m_backgroundColor->r, m_backgroundColor->g, m_backgroundColor->b, m_backgroundColor->a);
    SDL_RenderClear(m_renderer);

    m_registry->GetSystem<RenderSystem>().Update(m_renderer, m_assetStore, *m_camera);
    m_registry->GetSystem<RenderTextSystem>().Update(m_assetStore, m_renderer, *m_camera);
    m_registry->GetSystem<RenderHealthBarSystem>().Update(m_renderer, m_assetStore, *m_camera);
    if (m_shouldRenderDebug)
    {
        m_registry->GetSystem<RenderColliderSystem>().Update(m_renderer, *m_camera);
        //m_registry->GetSystem<RenderGUISystem>().Update(m_registry, m_assetStore, *m_camera);
    }

    SDL_RenderPresent(m_renderer);
}

void Game::Destroy()
{
    ImGuiSDL::Deinitialize();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}