#pragma once

#include "CommonTypes.h"

/**
 * 多线程渲染工具函数
 */
namespace JzRE {
// 获取推荐的线程数量
U32 GetRecommendedThreadCount();

// 检查多线程渲染支持
Bool IsMultithreadedRenderingSupported();

// 线程亲和性设置
void SetThreadAffinity(std::thread &thread, U32 coreId);

// 性能分析
void BeginProfileRegion(const String &name);
void EndProfileRegion();
} // namespace JzRE
