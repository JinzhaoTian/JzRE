#pragma once

/**
 * 3rd party dependencies
 */
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__APPLE__)
#define GLFW_EXPOSE_NATIVE_COCOA
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#endif
#include <GLFW/glfw3native.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

/**
 * sys
 */
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <functional>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <typeinfo>

/**
 * c++ 11
 */
#include <array>
#include <atomic>
#include <utility>
#include <memory>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <fstream>
#include <regex>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <type_traits>

/**
 * c++ 17
 */
#include <any>
#include <filesystem>
#include <optional>

/**
 * c++ 20
 */
#include <ranges>
#include <span>
#include <compare>
#include <concepts>
#include <format>
#include <semaphore>

/**
 * JzRE definitions
 */
namespace JzRE {

using Bool    = bool;
using U8      = uint8_t;
using U16     = uint16_t;
using U32     = uint32_t;
using U64     = uint64_t;
using Size    = size_t;
using I8      = int8_t;
using I16     = int16_t;
using I32     = int32_t;
using I64     = int64_t;
using F32     = float;
using F64     = double;
using String  = std::string;
using WString = std::wstring;

} // namespace JzRE