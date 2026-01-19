#pragma once

#include "Render/Renderer.h"
#include "Render/Window.h"

struct ApplicationSpecification
{
    std::string Name    = "Application";
    std::string Author  = "Super Cool Game Corp";
    SemVer      Version = SemVer(1, 0, 0);
};

class Application
{
public:
    explicit Application(ApplicationSpecification spec);

    bool Initialise();
    void Run();
    void Shutdown();

    static void BeginImGUI();
    static bool OnSDLEvent(const SDL_Event& e);
    bool        OnWindowClosed();

    void Close();
    void ShowError(const std::string& message, const std::string& title = "Error") const;

    NODISCARD FORCEINLINE const ApplicationSpecification& GetSpecification() const { return m_Specification; }
    NODISCARD FORCEINLINE Window&                         GetWindow() { return m_Window; }
    NODISCARD FORCEINLINE const Window&                   GetWindow() const { return m_Window; }
    NODISCARD FORCEINLINE bool                            IsRunning() const { return m_Running; }

    NODISCARD FORCEINLINE static bool ShouldRestart() { return s_ShouldRestart; }
    FORCEINLINE static void RequestRestart(bool restart = true)
    {
        s_ShouldRestart = restart;
    }

    NODISCARD FORCEINLINE static Application* Get() { return s_Instance; }

    // TODO(mware): Move font management to some other dedicated class that will handle ImGui themeing.
    NODISCARD FORCEINLINE static ImFont* GetOpenSansRegular() { return s_OpenSansRegular; }
    NODISCARD FORCEINLINE static ImFont* GetOpenSansBold() { return s_OpenSansBold; }

    MulticastDelegate<>               OnDrawIMGui;
    CascadingMulticastDelegate<false> OnApplicationCloseRequested;

protected:
    bool InitSDL() const;
    bool InitImGUI();

    void ShutdownImGUI();

    // Application runtime data.
    ApplicationSpecification m_Specification;
    Window                   m_Window;
    Renderer                 m_Renderer;
    bool                     m_Running          = false;
    bool                     m_ImGUIInitialized = false;

    // Static instance and variables.
    static Application* s_Instance;
    static bool         s_ShouldRestart;

    static ImFont* s_OpenSansRegular;
    static ImFont* s_OpenSansBold;
};
