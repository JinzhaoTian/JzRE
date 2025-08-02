# JzRE RHI

JzRE RHI（Render Hardware Interface）是一个现代化的跨平台图形API抽象层，参考UE5的RHI设计理念，为JzRE渲染引擎提供统一的图形接口。

## 主要特性

🚀 **跨平台支持**
- OpenGL 3.3+ (已实现)
- Vulkan (架构已准备)
- Direct3D 11/12 (架构已准备)
- Metal (架构已准备)

⚡ **高性能渲染**
- 立即渲染模式 - 兼容传统代码
- 命令缓冲模式 - 支持延迟执行
- 多线程渲染 - 充分利用多核CPU

🛠️ **现代化设计**
- C++20标准
- 智能指针管理
- 类型安全的枚举
- RAII资源管理

🔧 **易于扩展**
- 模块化架构
- 插件式后端
- 统一的资源抽象

🚀 **运行时API切换**
- 支持运行时在不同图形API间无缝切换
- 智能状态保持和资源迁移
- 完备的错误处理和回退机制

## 快速开始

### 1. 基本初始化

```cpp
auto& rhiContext = JzRE::JzRHIContext::GetInstance();
if (!rhiContext.Initialize()) {
    std::cerr << "RHI初始化失败" << std::endl;
    return -1;
}

auto device = rhiContext.GetDevice();
std::cout << "使用图形API: " << JzRHIFactory::GetRHITypeName(device->GetRHIType()) << std::endl;
```

### 2. 创建资源

```cpp
// 创建顶点缓冲
BufferDesc bufferDesc;
bufferDesc.type = EBufferType::Vertex;
bufferDesc.usage = EBufferUsage::StaticDraw;
bufferDesc.size = sizeof(vertices);
bufferDesc.data = vertices;
auto vertexBuffer = device->CreateBuffer(bufferDesc);

// 创建纹理
TextureDesc textureDesc;
textureDesc.format = ETextureFormat::RGBA8;
textureDesc.width = 512;
textureDesc.height = 512;
auto texture = device->CreateTexture(textureDesc);
```

### 3. 渲染

```cpp
// 立即模式渲染
device->BeginFrame();
device->Clear(clearParams);
device->BindPipeline(pipeline);
device->BindVertexArray(vertexArray);
device->DrawIndexed(drawParams);
device->EndFrame();

// 或使用命令缓冲
auto commandBuffer = device->CreateCommandBuffer();
commandBuffer->Begin();
commandBuffer->Clear(clearParams);
commandBuffer->BindPipeline(pipeline);
commandBuffer->DrawIndexed(drawParams);
commandBuffer->End();
device->ExecuteCommandBuffer(commandBuffer);
```

### 4. 多线程渲染

```cpp
// 启用多线程
rhiContext.SetThreadCount(4);

// 并行提交命令
auto commandQueue = rhiContext.GetCommandQueue();
commandQueue->SubmitCommandBuffer(commandBuffer1);
commandQueue->SubmitCommandBuffer(commandBuffer2);
commandQueue->ExecuteAll();
```

## 扩展开发

### 添加新的图形API后端

1. 在`ERHIType`枚举中添加新类型
2. 创建对应的头文件（如`D3D12RHI.h`）
3. 实现所有RHI接口类
4. 在`RHIFactory`中添加创建逻辑

### 添加新的渲染功能

RHI架构支持轻松扩展：
- 新的资源类型
- 新的渲染命令
- 新的渲染状态
- 自定义调试功能

## 性能调优建议

1. **批处理绘制调用**：尽可能合并相似的绘制操作
2. **使用命令缓冲**：对于复杂场景，命令缓冲比立即模式更高效
3. **合理使用多线程**：根据场景复杂度调整线程数
4. **资源复用**：避免频繁创建和销毁资源
5. **状态缓存**：RHI会自动缓存状态，但仍需注意状态切换频率

## 调试功能

- **调试标签**：为所有资源设置有意义的调试名称
- **性能统计**：实时监控绘制调用、三角形数量等
- **错误检查**：自动检测常见的使用错误
- **内存跟踪**：监控GPU内存使用情况

## 未来规划

- [ ] 完整的Vulkan后端实现
- [ ] Direct3D 12后端实现
- [ ] 渲染图系统
- [ ] GPU计算着色器支持
- [ ] 实时光线追踪集成
- [ ] VR/AR渲染优化
