#pragma once

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>

enum class VSyncMode : u8
{
    Off,
    On,
    Adaptive
};

inline const char* VSyncModeToString(const VSyncMode mode)
{
    switch (mode)
    {
    case VSyncMode::Off:
        return "Off";
    case VSyncMode::On:
        return "On";
    case VSyncMode::Adaptive:
        return "Adaptive";
    default:
        return "Unknown";
    }
}

struct WindowSpecification
{
    std::string Title        = "Mineprint";
    glm::ivec2  Size         = {1280, 720};
    glm::ivec2  OriginalSize = {1280, 720};
    glm::ivec2  MinSize      = {0, 0};
    glm::ivec2  MaxSize      = {0, 0};
    glm::ivec2  Position     = {0, 0}; // Ignored if centered is true.
    bool        StartVisible = true;
    bool        Centered     = true;
    bool        Fullscreen   = false;
    bool        Resizable    = true;
    VSyncMode   VSync        = VSyncMode::On;
};

class Window
{
public:
    explicit Window(WindowSpecification spec);
    ~Window() { Destroy(); }

    Window(const Window& other)                = delete;
    Window(Window&& other) noexcept            = delete;
    Window& operator=(const Window& other)     = delete;
    Window& operator=(Window&& other) noexcept = delete;

    bool Create();
    void PollEvents();
    void Destroy();

    void           Show();
    void           Hide();
    void           SetGLContextCurrent() const;
    void           LockCursor() const;
    void           UnlockCursor() const;
    NODISCARD bool IsCursorLocked() const;

    void SetTitle(const std::string& title);
    void SetSize(const glm::ivec2& size);
    void SetMinSize(const glm::ivec2& minSize);
    void SetMaxSize(const glm::ivec2& maxSize);
    void SetPosition(const glm::ivec2& position);
    void SetFullscreen(bool fullscreen);
    void SetResizable(bool resizable);
    void SetVSync(VSyncMode vsync);

    FORCEINLINE void ToggleFullscreen() { SetFullscreen(!m_Specification.Fullscreen); }

    NODISCARD FORCEINLINE const std::string&         GetTitle() const { return m_Specification.Title; }
    NODISCARD FORCEINLINE const glm::ivec2&          GetSize() const { return m_Specification.Size; }
    NODISCARD FORCEINLINE s32                        GetWidth() const { return m_Specification.Size.x; }
    NODISCARD FORCEINLINE s32                        GetHeight() const { return m_Specification.Size.y; }
    NODISCARD FORCEINLINE const glm::ivec2&          GetPosition() const { return m_Specification.Position; }
    NODISCARD FORCEINLINE bool                       IsFullscreen() const { return m_Specification.Fullscreen; }
    NODISCARD FORCEINLINE bool                       IsResizable() const { return m_Specification.Resizable; }
    NODISCARD FORCEINLINE VSyncMode                  GetVSync() const { return m_Specification.VSync; }
    NODISCARD FORCEINLINE bool                       IsValid() const { return m_Window != nullptr; }
    NODISCARD FORCEINLINE bool                       IsVisible() const { return m_IsVisible; }
    NODISCARD FORCEINLINE SDL_Window*                GetSDLWindow() const { return m_Window; }
    NODISCARD FORCEINLINE const WindowSpecification& GetSpecification() const { return m_Specification; }
    NODISCARD FORCEINLINE SDL_GLContext              GetGLContext() const { return m_GLContext; }

    CascadingMulticastDelegate<false>                              OnWindowClose;
    CascadingMulticastDelegate<false, const glm::ivec2&>           OnWindowResize;
    CascadingMulticastDelegate<false, const glm::ivec2&>           OnWindowMove;
    CascadingMulticastDelegate<false, const SDL_KeyboardEvent&>    OnKeyboardEvent;
    CascadingMulticastDelegate<false, const SDL_MouseButtonEvent&> OnMouseButtonEvent;
    CascadingMulticastDelegate<false, const SDL_MouseMotionEvent&> OnMouseMotionEvent;
    CascadingMulticastDelegate<false, const SDL_MouseWheelEvent&>  OnMouseWheelEvent;
    CascadingMulticastDelegate<false, const SDL_Event&>            OnSDLEvent;

protected:
    WindowSpecification m_Specification;

    bool          m_IsVisible = false;
    SDL_Window*   m_Window    = nullptr;
    SDL_GLContext m_GLContext = nullptr;
};
