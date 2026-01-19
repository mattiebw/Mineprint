#pragma once

#define SPDLOG_EOL ""
#define FMT_UNICODE 0

// Utility functions and types
#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <filesystem>
#include <deque>

// Data types
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

// imgui
#ifndef VULC_NO_IMGUI
#include <imgui.h>
#endif

// glm
#include <glm/glm.hpp>

// fmt
#include <spdlog/fmt/fmt.h>

// Core project includes
#include "Core/MineprintLog.h" // Include the logger first, so we can use it in the assertion macro.
#include <SDL3/SDL_assert.h> // Even though we use our own assertion system, we need this for SDL_TriggerBreakpoint!
#include "Core/Assert.h"
#include "Core/MineprintCore.h"
#include "Core/Utility/Random.h"
#include "Core/Utility/UUID.h"
#include "Core/Utility/Buffer.h"
#include "Core/Utility/Timer.h"
#include "Core/Utility/FileUtil.h"
#include "Core/MathUtil.h"
#include "Core/Delegate.h"
#include "Core/Formatters.h"
