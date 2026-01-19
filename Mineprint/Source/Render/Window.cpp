#include "mppch.h"
#include "Render/Window.h"

#include <SDL3/SDL_events.h>

Window::Window(WindowSpecification spec)
    : m_Specification(std::move(spec))
{
}

bool Window::Create()
{
    MP_CHECK(!m_Specification.Title.empty(), "Window title cannot be empty");
    MP_CHECK(m_Specification.Size.x > 0 && m_Specification.Size.y > 0, "Window size must be greater than 0");

    m_Window = SDL_CreateWindow(m_Specification.Title.data(), m_Specification.Size.x, m_Specification.Size.y,
                                SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL);
    MP_CHECK(m_Window != nullptr, "Failed to create window: {}", SDL_GetError());

    // Set size related properties.
    m_Specification.OriginalSize = m_Specification.Size;
    SDL_SetWindowSize(m_Window, m_Specification.Size.x, m_Specification.Size.y);
    SDL_SetWindowMinimumSize(m_Window, m_Specification.MinSize.x, m_Specification.MinSize.y);
    SDL_SetWindowMaximumSize(m_Window, m_Specification.MaxSize.x, m_Specification.MaxSize.y);
    SDL_SetWindowResizable(m_Window, m_Specification.Resizable);

    // Set position related properties.
    if (m_Specification.Centered)
        SDL_SetWindowPosition(m_Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    else
        SDL_SetWindowPosition(m_Window, m_Specification.Position.x, m_Specification.Position.y);

    SDL_SetWindowFullscreen(m_Window, m_Specification.Fullscreen);

    // Associate the Window object with the SDL window, so we can call Window functions from SDL events.
    SDL_PropertiesID props = SDL_GetWindowProperties(m_Window);
    SDL_SetPointerProperty(props, "Window", this);

    if (m_Specification.StartVisible)
        Show();

    // Init OpenGL.
    m_GLContext = SDL_GL_CreateContext(m_Window);
    SetGLContextCurrent();

    SetVSync(m_Specification.VSync);

    return true;
}

void Window::PollEvents()
{
    // TODO(mware): This should be refactored out to the Application - we store the Window pointer as an SDL property,
    // so we can still execute window delegates. If pointer stability is an issue, we can use some sort of handle.
    // TODO(mware): Update window close to have a CascadingDelegate to check if the close should actually happen

    SDL_Event windowEvent;

    while (SDL_PollEvent(&windowEvent))
    {
        OnSDLEvent.Execute(windowEvent);

        switch (windowEvent.type)
        {
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            {
                if (windowEvent.window.windowID != SDL_GetWindowID(m_Window))
                    break;
                OnWindowClose.Execute();
                break;
            }
        case SDL_EVENT_WINDOW_RESIZED:
            {
                glm::ivec2 newSize   = {windowEvent.window.data1, windowEvent.window.data2};
                m_Specification.Size = newSize;
                OnWindowResize.Execute(newSize);
                break;
            }
        case SDL_EVENT_WINDOW_MOVED:
            {
                glm::ivec2 newPos        = {windowEvent.window.data1, windowEvent.window.data2};
                m_Specification.Position = newPos;
                OnWindowMove.Execute(newPos);
                break;
            }
        case SDL_EVENT_KEY_UP:
        case SDL_EVENT_KEY_DOWN:
            {
                OnKeyboardEvent.Execute(windowEvent.key);
                break;
            }
        case SDL_EVENT_MOUSE_BUTTON_UP:
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                OnMouseButtonEvent.Execute(windowEvent.button);
                break;
            }
        case SDL_EVENT_MOUSE_MOTION:
            {
                OnMouseMotionEvent.Execute(windowEvent.motion);
                break;
            }
        case SDL_EVENT_MOUSE_WHEEL:
            {
                OnMouseWheelEvent.Execute(windowEvent.wheel);
                break;
            }
        default:
            break;
        }
    }
}

void Window::Destroy()
{
    // Unbind from all events
    OnWindowClose.UnbindAll();
    OnWindowResize.UnbindAll();
    OnWindowMove.UnbindAll();
    OnKeyboardEvent.UnbindAll();
    OnMouseButtonEvent.UnbindAll();
    OnMouseMotionEvent.UnbindAll();
    OnSDLEvent.UnbindAll();

    // Destroy the GL context.
    if (m_GLContext)
    {
        SDL_GL_DestroyContext(m_GLContext);
        m_GLContext = nullptr;
    }

    // Destroy the window, if it exists
    if (m_Window)
    {
        SDL_DestroyWindow(m_Window);
        m_Window = nullptr;
    }
}

void Window::Show()
{
    MP_ASSERT(m_Window && "Window is not initialised");
    SDL_ShowWindow(m_Window);
    m_IsVisible = true;
}

void Window::Hide()
{
    MP_ASSERT(m_Window && "Window is not initialised");
    SDL_HideWindow(m_Window);
    m_IsVisible = false;
}

void Window::SetGLContextCurrent() const
{
    SDL_GL_MakeCurrent(m_Window, m_GLContext);
}

void Window::LockCursor() const
{
    SDL_SetWindowRelativeMouseMode(m_Window, true);
}

void Window::UnlockCursor() const
{
    SDL_WarpMouseInWindow(m_Window, static_cast<float>(m_Specification.Size.x) / 2,
                          static_cast<float>(m_Specification.Size.y) / 2);
    SDL_SetWindowRelativeMouseMode(m_Window, false);
}

bool Window::IsCursorLocked() const
{
    return SDL_GetWindowRelativeMouseMode(m_Window);
}

void Window::SetTitle(const std::string& title)
{
    MP_ASSERT(m_Window, "Window must be created before setting the title");
    SDL_SetWindowTitle(m_Window, title.data());
    m_Specification.Title = title;
}

void Window::SetSize(const glm::ivec2& size)
{
    MP_ASSERT(m_Window, "Window must be created before setting the size");
    SDL_SetWindowSize(m_Window, size.x, size.y);
    m_Specification.Size = size;
}

void Window::SetMinSize(const glm::ivec2& minSize)
{
    MP_ASSERT(m_Window, "Window must be created before setting the minimum size");
    SDL_SetWindowMinimumSize(m_Window, minSize.x, minSize.y);
    m_Specification.MinSize = minSize;
}

void Window::SetMaxSize(const glm::ivec2& maxSize)
{
    MP_ASSERT(m_Window, "Window must be created before setting the maximum size");
    SDL_SetWindowMaximumSize(m_Window, maxSize.x, maxSize.y);
    m_Specification.MaxSize = maxSize;
}

void Window::SetPosition(const glm::ivec2& position)
{
    MP_ASSERT(m_Window, "Window must be created before setting the position");
    SDL_SetWindowPosition(m_Window, position.x, position.y);
    m_Specification.Position = position;
}

void Window::SetFullscreen(bool fullscreen)
{
    MP_ASSERT(m_Window, "Window must be created before setting fullscreen");
    SDL_SetWindowFullscreen(m_Window, fullscreen);
    m_Specification.Fullscreen = fullscreen;
}

void Window::SetResizable(bool resizable)
{
    MP_ASSERT(m_Window, "Window must be created before setting resizable");
    SDL_SetWindowResizable(m_Window, resizable);
    m_Specification.Resizable = resizable;
}

void Window::SetVSync(VSyncMode vsync)
{
    MP_ASSERT(m_Window, "Window must be created before setting vsync");
    MP_ASSERT(m_GLContext, "Window must have valid GL context before setting VSync");
    SetGLContextCurrent();

    m_Specification.VSync = vsync;

    switch (vsync)
    {
    case VSyncMode::Off:
        SDL_GL_SetSwapInterval(0);
        break;
    case VSyncMode::On:
        SDL_GL_SetSwapInterval(1);
        break;
    case VSyncMode::Adaptive:
        const bool success = SDL_GL_SetSwapInterval(-1);
        if (!success)
        {
            MP_WARN("Failed to use adaptive VSync - falling back to regular VSync.");
            SDL_GL_SetSwapInterval(1);
            m_Specification.VSync = VSyncMode::On;
        }
        break;
    }
}
