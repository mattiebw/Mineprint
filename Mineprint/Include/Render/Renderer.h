#pragma once

#include <glad/gl.h>

class Application;
class Window;

struct RendererSpecification
{
    Application* App   = nullptr;
};

class Renderer
{
public:
    Renderer() = default;
    ~Renderer();

    Renderer(const Renderer& other)                = delete;
    Renderer(Renderer&& other) noexcept            = delete;
    Renderer& operator=(const Renderer& other)     = delete;
    Renderer& operator=(Renderer&& other) noexcept = delete;

    bool Init(const RendererSpecification& spec);
    void Render();
    void Present();
    void Shutdown();

    NODISCARD FORCEINLINE const RendererSpecification& GetSpecification() const { return m_Spec; }
    
    static void GLErrorCallback(GLenum        source,
                                GLenum        type,
                                GLuint        id,
                                GLenum        severity,
                                GLsizei       length,
                                const GLchar *message,
                                const void *  userParam);

protected:
    // Initialisation functions
    bool InitOpenGL();

    // Event functions
    static bool OnWindowResize(const glm::ivec2& newSize);

    Window*       m_Window  = nullptr;
    SDL_GLContext m_Context = nullptr;
    
    // Frame state data
    u64 m_FrameIndex = 0;

    RendererSpecification m_Spec = {};
};
