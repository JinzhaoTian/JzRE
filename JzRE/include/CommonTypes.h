#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <array>
#include <unordered_set>
#include <unordered_map>
#include <cmath>
#include <functional>
#include <memory>
#include <iostream>
#include <sstream>
#include <cmath>
#include <cassert>
#include <type_traits>

#include <windows.h>

#define RGBA(r, g, b, a) ((b) | ((g) << 8) | ((r) << 16) | ((a) << 24))

namespace JzRE {

using U8 = uint8_t;
using U16 = uint16_t;
using U32 = uint32_t;
using U64 = uint64_t;

using I8 = int8_t;
using I16 = int16_t;
using I32 = int32_t;
using I64 = int64_t;

using Size = size_t;

using Bool = bool;

using F32 = float;
using F64 = double;

using String = std::string;
using WString = std::wstring;

template <typename T>
using List = std::vector<T>;

template <typename T>
using Queue = std::queue<T>;

template <typename Key, typename Value>
using Map = std::map<Key, Value>;

template <typename Key, typename Value>
using UnorderedMap = std::unordered_map<Key, Value>;

template <typename T>
using Set = std::set<T>;

template <typename T>
using SharedPtr = std::shared_ptr<T>;

template <typename T>
using WeakPtr = std::weak_ptr<T>;

template <typename T>
using UniquePtr = std::unique_ptr<T>;

template <typename T>
using RawPtr = T *;

template <typename T, typename... Args>
inline SharedPtr<T> CreateSharedPtr(Args &&...args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T, typename... Args>
inline UniquePtr<T> CreateUniquePtr(Args &&...args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T, typename... Args>
inline RawPtr<T> CreateRawPtr(Args &&...args) {
    return new T(std::forward<Args>(args)...);
}

template <typename To, typename From>
inline To StaticCast(From from) {
    return static_cast<To>(from);
}

} // namespace JzRE