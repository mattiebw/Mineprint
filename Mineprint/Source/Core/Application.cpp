#include "mppch.h"
#include "Core/Application.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_filesystem.h>

#ifndef MP_NO_IMGUI
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_opengl3.h>
#endif

#include "Core/Input/Input.h"
#include "Core/EmbeddedContent/OpenSans.h"

Application* Application::s_Instance        = nullptr;
bool         Application::s_ShouldRestart   = false;
ImFont*      Application::s_OpenSansRegular = nullptr;
ImFont*      Application::s_OpenSansBold    = nullptr;

Application::Application(ApplicationSpecification spec)
    : m_Specification(std::move(spec)),
      m_Window(Window({.Title = m_Specification.Name, .Size = {1280, 720}, .Fullscreen = false, .Resizable = true}))
{
    MP_ASSERT(!m_Specification.Name.empty(), "Application name cannot be empty");
    MP_ASSERT(!m_Specification.Author.empty(), "Application author cannot be empty");
    MP_ASSERT(m_Specification.Version.Packed() > 0, "Application version must be greater than 0");
}

bool Application::Initialise()
{
    MP_ASSERT(!s_Instance, "Application already initialised!");
    s_Instance = this;

    InitLog(SDL_GetPrefPath(m_Specification.Author.c_str(), m_Specification.Name.c_str()));

    MP_INFO("Initialising application: {} by {}", m_Specification.Name, m_Specification.Author);
    auto workingDir = std::filesystem::current_path().string();
    MP_INFO("Working directory: {}", workingDir);

    if (!InitSDL())
        return false;

    if (!m_Window.Create())
        return false;

    Input::Init();

    // Bindings to window events.
    m_Window.OnWindowClose.BindMethod(this, &Application::OnWindowClosed);

    m_Window.OnSDLEvent.BindStatic(&Application::OnSDLEvent);

    m_Window.OnKeyboardEvent.BindLambda([this](const SDL_KeyboardEvent& event)
    {
        Input::ProcessKeyboardInputEvent(event);
        return false;
    });

    m_Window.OnMouseButtonEvent.BindLambda([this](const SDL_MouseButtonEvent& event)
    {
        Input::ProcessMouseInputEvent(event);
        return false;
    });

    m_Window.OnMouseMotionEvent.BindLambda([this](const SDL_MouseMotionEvent& event)
    {
        Input::ProcessMouseMotionEvent(event);
        return false;
    });

    if (!m_Renderer.Init({.App = this}))
    {
        // The renderer will do its own error logging.
        return false;
    }

    if (!InitImGUI())
    {
        MP_ERROR("Failed to initialise ImGUI");
        return false;
    }

    return true;
}

void Application::Run()
{
    m_Running = true;

    while (m_Running)
    {
        Input::PreUpdate();
        m_Window.PollEvents();

        static s32 theInteger = 5;
        MP_CHECK(!Input::IsKeyDownThisFrame(Scancode::R),
                 "DON'T PRESS R!! Static number: {}, rvalue number: {}, window size: {}, window title: {}", theInteger,
                 420, m_Window.GetSize(), m_Window.GetTitle());
        MP_CHECK(!Input::IsKeyDownThisFrame(Scancode::B));

        if (Input::IsKeyDownThisFrame(Scancode::A))
            PrintAssertionReport();

        if (Input::IsKeyDownThisFrame(Scancode::Escape))
            Close();

        BeginImGUI();

        OnDrawIMGui.Execute();

        ImGui::Begin("App Settings");
        ImGui::Checkbox("Should Restart", &s_ShouldRestart);
        if (ImGui::BeginCombo("VSync", VSyncModeToString(m_Window.GetVSync())))
        {
            if (ImGui::Selectable("Off", m_Window.GetVSync() == VSyncMode::Off))
                m_Window.SetVSync(VSyncMode::Off);
            if (ImGui::Selectable("On", m_Window.GetVSync() == VSyncMode::On))
                m_Window.SetVSync(VSyncMode::On);
            if (ImGui::Selectable("Adaptive", m_Window.GetVSync() == VSyncMode::Adaptive))
                m_Window.SetVSync(VSyncMode::Adaptive);
            ImGui::EndCombo();
        }
        ImGui::End();

        ImGui::Render();

        m_Renderer.Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_Window*   backupCurrentWindow  = SDL_GL_GetCurrentWindow();
        SDL_GLContext backupCurrentContext = SDL_GL_GetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(backupCurrentWindow, backupCurrentContext);

        m_Renderer.Present();
    }
}

void Application::Shutdown()
{
    MP_INFO("Shutting down {}", m_Specification.Name);

    m_Renderer.Shutdown();

    ShutdownImGUI();

    Input::Shutdown();

    if (m_Window.IsValid())
        m_Window.Destroy();

    SDL_Quit();

    // We'll only shut down the log fully if we're not restarting.
    // If we are restarting, we don't want to re-initialise the log, as that'll create a new log file.
    if (!ShouldRestart())
        ShutdownLog();

    s_Instance = nullptr;
}

void Application::BeginImGUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

bool Application::OnSDLEvent(const SDL_Event& e)
{
#ifndef MP_NO_IMGUI
    ImGui_ImplSDL3_ProcessEvent(&e);
#endif

    return false;
}

bool Application::OnWindowClosed()
{
    Close();
    return false;
}

void Application::Close()
{
    if (!m_Running)
        return;

    if (!OnApplicationCloseRequested.Execute())
        return;

    MP_INFO("Closing {}!", m_Specification.Name);
    m_Running = false;
}

void Application::ShowError(const std::string& message, const std::string& title) const
{
    MP_ERROR("{}", message);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.data(), message.data(), m_Window.GetSDLWindow());
}

bool Application::InitSDL() const
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        ShowError(fmt::format("Failed to initialise SDL: {}", SDL_GetError()), "SDL Error");
        return false;
    }

    return true;
}

bool Application::InitImGUI()
{
    MP_CHECK(!m_ImGUIInitialized, "ImGUI already initialized");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io    = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable | ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    MP_CHECK(ImGui_ImplSDL3_InitForOpenGL(m_Window.GetSDLWindow(), m_Window.GetGLContext()),
             "Failed to init ImGUI SDL3 backend for OpenGL");
    MP_CHECK(ImGui_ImplOpenGL3_Init("#version 330"), "Failed to init ImGUI OpenGL3 backend");

    // Add default fonts.
    ImFontConfig config;
    config.FontDataOwnedByAtlas = false;
    s_OpenSansRegular = io.Fonts->AddFontFromMemoryTTF(OpenSans_Regular_ttf, static_cast<int>(OpenSans_Regular_ttf_len),
                                                       16.0f, &config);
    s_OpenSansBold = io.Fonts->AddFontFromMemoryTTF(OpenSans_Bold_ttf, static_cast<int>(OpenSans_Bold_ttf_len), 16.0f,
                                                    &config);
    io.FontDefault = s_OpenSansRegular;

    m_ImGUIInitialized = true;

    return true;
}

void Application::ShutdownImGUI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    m_ImGUIInitialized = false;
}
