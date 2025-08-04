# JzWindow RHI 集成改造方案

## 概述

本文档描述了如何改造 `JzWindow` 类，使其能够使用 `JzRHIDevice` 的相关方法，实现窗口管理与渲染硬件接口的深度集成。

## 改造目标

- 让 `JzWindow` 能够直接使用 `JzRHIDevice` 的渲染方法
- 简化渲染管线的使用流程
- 保持向后兼容性
- 支持多种渲染后端（OpenGL、Vulkan等）

## 主要改动

### 1. JzWindow 类改造

#### 新增成员变量
```cpp
std::shared_ptr<JzRHIDevice> m_rhiDevice;  // RHI 设备引用
```

#### 新增构造函数
```cpp
JzWindow(const JzDevice &device, const JzWindowSettings &windowSettings, 
         std::shared_ptr<JzRHIDevice> rhiDevice);
```

#### 新增 RHI 集成方法
- `SetRHIDevice()` - 设置 RHI 设备
- `GetRHIDevice()` - 获取 RHI 设备
- `BeginFrame()` - 开始帧渲染
- `EndFrame()` - 结束帧渲染
- `Present()` - 呈现帧
- `SetViewport()` - 设置视口
- `Clear()` - 清除缓冲区
- `HasRHIDevice()` - 检查是否有 RHI 设备
- `GetViewport()` - 获取窗口视口
- `MakeRHIContextCurrent()` - 设置 RHI 上下文为当前

#### 改进的方法
- `SwapBuffers()` - 现在会先调用 RHI 设备的 `Present()` 方法
- `MakeCurrentContext()` - 同时设置 GLFW 和 RHI 上下文
- `OnResize()` - 窗口大小改变时自动更新 RHI 视口

### 2. JzContext 类改造

- 创建窗口时自动传入 RHI 设备引用
- 提供统一的渲染接口方法
- 简化帧渲染流程

## 使用方式

### 基本使用

```cpp
// 1. 创建 RHI 集成的上下文
JzContext context(JzERHIType::OpenGL);

// 2. 在渲染循环中使用
while (!context.window->ShouldClose()) {
    context.device->PollEvents();
    
    // 开始帧
    context.BeginFrame();
    
    // 设置视口
    context.SetViewport(context.GetWindowViewport());
    
    // 清除屏幕
    JzClearParams clearParams{};
    clearParams.clearColor = true;
    clearParams.colorValue = {0.2f, 0.3f, 0.8f, 1.0f};
    context.Clear(clearParams);
    
    // 你的渲染代码...
    
    // 结束帧并呈现
    context.EndFrame();
    context.Present();
}
```

### 直接使用 JzWindow

```cpp
// 创建设备和窗口
JzDevice device(deviceSettings);
JzWindowSettings windowSettings;
auto rhiDevice = JzRHIFactory::CreateDevice(JzERHIType::OpenGL);
JzWindow window(device, windowSettings, rhiDevice);

// 检查 RHI 设备是否可用
if (window.HasRHIDevice()) {
    window.BeginFrame();
    
    // 使用 RHI 设备进行渲染
    auto rhi = window.GetRHIDevice();
    // ... 渲染代码 ...
    
    window.EndFrame();
    window.Present();
    window.SwapBuffers();
}
```

### 高级功能

#### 多线程渲染支持
```cpp
if (rhiDevice->SupportsMultithreading()) {
    // 为不同线程设置上下文
    window.MakeRHIContextCurrent(threadIndex);
}
```

#### 事件处理集成
```cpp
window.WindowResizedEvent.AddListener([&](U16 width, U16 height) {
    // 窗口大小改变时，RHI 视口会自动更新
    // 可以在这里做额外的处理
});
```

## 兼容性

### 向后兼容
- 原有的 `JzWindow(const JzDevice &, const JzWindowSettings &)` 构造函数仍然可用
- 所有原有的窗口方法都保持不变
- 如果没有设置 RHI 设备，窗口照常工作

### 渐进式升级
1. 现有代码无需修改即可继续使用
2. 需要 RHI 功能时，可以调用 `SetRHIDevice()` 方法
3. 新项目可以直接使用 RHI 集成的构造函数

## 架构优势

### 1. 统一接口
- 窗口管理和渲染操作通过统一的接口进行
- 减少了不同系统间的耦合

### 2. 自动管理
- 视口自动随窗口大小调整
- 上下文管理自动化
- 资源生命周期管理

### 3. 多后端支持
- 同一套代码可以支持 OpenGL、Vulkan 等不同后端
- RHI 抽象层提供统一的 API

### 4. 性能优化
- 减少不必要的状态切换
- 更好的多线程支持
- 优化的资源管理

## 扩展性

### 未来可扩展的功能
- 支持多窗口渲染
- VR/AR 渲染支持
- 更多的渲染后端（DirectX 12、Metal 等）
- 更细粒度的性能控制

### 插件化支持
- 可以通过插件系统添加特定平台的优化
- 支持自定义渲染管线

## 注意事项

1. **线程安全**：使用多线程渲染时，确保正确设置上下文
2. **资源管理**：RHI 设备的生命周期需要比窗口更长
3. **错误处理**：始终检查 `HasRHIDevice()` 的返回值
4. **性能考虑**：频繁的视口切换可能影响性能

## 迁移指南

### 从旧版本迁移

1. **最小改动迁移**：
   ```cpp
   // 旧代码
   JzWindow window(device, windowSettings);
   
   // 新代码（无需修改）
   JzWindow window(device, windowSettings);
   // 需要时再设置 RHI 设备
   window.SetRHIDevice(rhiDevice);
   ```

2. **完全 RHI 集成迁移**：
   ```cpp
   // 替换为新的构造函数
   JzWindow window(device, windowSettings, rhiDevice);
   
   // 使用新的渲染方法
   window.BeginFrame();
   window.Clear(clearParams);
   // 渲染...
   window.EndFrame();
   window.Present();
   ```

## 总结

这次改造实现了 `JzWindow` 与 `JzRHIDevice` 的深度集成，提供了：

- 更简洁的 API 设计
- 更好的性能和可扩展性
- 完整的向后兼容性
- 统一的多后端支持

这为 JzRE 引擎的未来发展奠定了坚实的基础。