#include "mppch.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#ifndef MP_NO_IMGUI
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include <backends/imgui_impl_sdl3.cpp>
#include <backends/imgui_impl_opengl3.cpp>
#endif
