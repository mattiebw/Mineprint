#include "mppch.h"

#include "Core/Application.h"
#include "Render/Renderer.h"

#ifndef MP_NO_IMGUI
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#endif

Renderer::~Renderer()
{
    Shutdown();
}

bool Renderer::Init(const RendererSpecification& spec)
{
    m_Spec = spec;
    MP_ASSERT(m_Spec.App != nullptr, "Application must be set");
    m_Window = &m_Spec.App->GetWindow();
    MP_ASSERT(m_Window, "Window must be created before initializing the renderer");

    InitOpenGL();

    m_Spec.App->GetWindow().OnWindowResize.BindStatic(&Renderer::OnWindowResize);

    // m_Spec.App->OnDrawIMGui.BindMethod(this, &Renderer::OnDrawIMGui);

    return true;
}

void Renderer::Render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Present()
{
    SDL_GL_SwapWindow(m_Window->GetSDLWindow());
    m_FrameIndex++;
}

void Renderer::Shutdown()
{
}

bool Renderer::InitOpenGL()
{
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8); // @todo(mware): Should this be part of the RendererSpec?

#ifdef MP_GL_DEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

    MP_CHECK(m_Window, "Window must be set");
    m_Context = m_Window->GetGLContext();

    static bool glInitialised = false;
    if (!glInitialised)
    {
        s32 version = gladLoadGL(SDL_GL_GetProcAddress);
        MP_CHECK(version != 0, "Failed to load OpenGL with GLAD2.");
        glInitialised = true;

        MP_INFO("Initialised OpenGL v{}.{}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
        MP_INFO("   OpenGL Vendor: {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
        MP_INFO("   OpenGL Renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
    }

    // Setup error callback
#ifdef MP_GL_DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(GLErrorCallback, nullptr);
#endif

    MP_INFO("Initialised renderer");

    return true;
}

bool Renderer::OnWindowResize(const glm::ivec2& newSize)
{
    glViewport(0, 0, newSize.x, newSize.y);
    return true;
}

void Renderer::GLErrorCallback(GLenum        source, GLenum       type, GLuint id, GLenum severity, GLsizei length,
                               const GLchar* message, const void* userParam)
{
#ifndef SIBOX_SHOW_GL_NOTIFICATIONS
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
        return;
#endif

#ifndef SIBOX_NO_IGNORED_GL_ERROR_IDS
    static std::vector<GLuint> ignoredIDs = {131185};

    if (std::find(ignoredIDs.begin(), ignoredIDs.end(), id) != ignoredIDs.end())
        return;
#endif

    const char* sourceText;
    const char* typeText;
    const char* severityText;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        sourceText = "API";
        break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        sourceText = "Window System";
        break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        sourceText = "Shader Compiler";
        break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
        sourceText = "Third Party";
        break;

    case GL_DEBUG_SOURCE_APPLICATION:
        sourceText = "Application";
        break;

    case GL_DEBUG_SOURCE_OTHER:
        sourceText = "Other";
        break;

    default:
        sourceText = "Unknown";
        break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        typeText = "Error";
        break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        typeText = "Deprecated Behaviour";
        break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        typeText = "Undefined Behaviour";
        break;

    case GL_DEBUG_TYPE_PORTABILITY:
        typeText = "Portability";
        break;

    case GL_DEBUG_TYPE_PERFORMANCE:
        typeText = "Performance";
        break;

    case GL_DEBUG_TYPE_OTHER:
        typeText = "Other";
        break;

    case GL_DEBUG_TYPE_MARKER:
        typeText = "Marker";
        break;

    default:
        typeText = "Unknown";
        break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        severityText = "High";
        break;

    case GL_DEBUG_SEVERITY_MEDIUM:
        severityText = "Medium";
        break;

    case GL_DEBUG_SEVERITY_LOW:
        severityText = "Low";
        break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
        severityText = "Notification";
        break;

    default:
        severityText = "Unknown";
        break;
    }

    MP_ASSERT(severity == GL_DEBUG_SEVERITY_NOTIFICATION, "OpenGL Error ({0} severity, id: {4}): from {1}, {2}: {3}",
              severityText, sourceText, typeText, message,
              id);
}
