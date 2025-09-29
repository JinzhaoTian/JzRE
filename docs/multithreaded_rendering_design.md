# JzRE 多线程渲染架构设计

## 1. 概述

本文档旨在为 JzRE 引擎设计一套多线程渲染架构，核心目标是将 **逻辑/UI线程** 与 **渲染线程** 分离。这将解决当前单线程模型中，渲染耗时阻塞应用逻辑和UI响应的问题，从而显著提升引擎的性能和用户体验的流畅度。

## 2. 当前单线程架构分析

目前，推测引擎的主循环在单个主线程上按顺序执行所有任务：

1.  **输入处理**: 响应键盘、鼠标等事件。
2.  **逻辑更新**: 更新场景、ECS系统、动画、物理等。
3.  **UI 更新**: 执行 ImGui 的 UI 逻辑，构建界面元素。
4.  **场景渲染**: 遍历场景，直接调用 RHI/OpenGL 指令进行绘制。
5.  **UI 渲染**: 调用 ImGui 的渲染函数，直接将 UI 绘制到屏幕。
6.  **缓冲区交换**: 将后台缓冲区的内容显示到屏幕上。

这种串行模式的瓶颈在于，任何一步的耗时都会延迟后续所有步骤，导致整体帧率下降和卡顿。

## 3. 设计方案：解耦的渲染前后端

我们采用经典的 **“命令缓冲” (Command Buffer)** 模式，将渲染任务拆分为前后端，分别在不同线程上运行。

-   **主线程 (Main/Logic Thread)**:
    -   负责处理输入、游戏逻辑和 UI 逻辑。
    -   **生成渲染指令**: 它将所有绘制请求（场景物体、UI）转换为一个抽象的、与图形API无关的 **渲染命令列表**。
    -   **不直接调用** 任何 RHI 或图形 API 函数。

-   **渲染线程 (Render Thread)**:
    -   **独占图形上下文 (Graphics Context)**，所有 RHI/API 调用都在此线程执行。
    -   等待并消费主线程生成的渲染命令列表。
    -   解析命令并将其转换为底层的 RHI 调用。
    -   执行最终的缓冲区交换。

---

## 4. 核心组件设计

#### a. 渲染命令 (`JzRenderCommand`)

使用 `std::variant` 定义的、描述单个渲染操作的数据结构。

```cpp
// 示例：绘制网格的命令
struct DrawMeshCommand {
    JzRHIVertexBuffer* vertexBuffer;
    JzRHIIndexBuffer* indexBuffer;
    JzRHIPipelineState* pipelineState;
    glm::mat4 transform;
    // ... 其他材质、纹理等渲染资源句柄
};

// 示例：渲染UI的命令
struct RenderImGuiCommand {
    // 从 ImDrawData 深拷贝的顶点/索引/命令数据
    std::vector<ImDrawVert> vertices;
    std::vector<ImDrawIdx> indices;
    std::vector<ImDrawCmd> commands;
    // ... 其他UI渲染所需状态
};

// 命令集合
using JzRenderCommand = std::variant<
    DrawMeshCommand,
    UpdateCameraCommand,
    RenderImGuiCommand
    // ... 其他命令
>;
```

#### b. 命令缓冲区 (`JzCommandBuffer`)

一帧内所有 `JzRenderCommand` 的集合。

```cpp
class JzCommandBuffer {
public:
    void addCommand(JzRenderCommand command);
    void clear();
    const std::vector<JzRenderCommand>& getCommands() const;
private:
    std::vector<JzRenderCommand> mCommands;
};
```

#### c. 渲染前端 (`JzRenderFrontend`)

-   运行在 **主线程**。
-   为上层逻辑提供渲染接口，如 `submit(mesh)`。
-   内部不执行渲染，而是将请求打包成 `JzRenderCommand` 并存入命令缓冲区。
-   管理两个命令缓冲区的交换，实现双缓冲机制。

#### d. 渲染后端 (`JzRenderBackend`)

-   运行在 **渲染线程**。
-   持有 RHI Device 等核心渲染资源。
-   循环等待并解析前端提交的命令缓冲区，执行真正的 RHI 调用。

#### e. 同步机制

使用 **双缓冲的命令缓冲区**，配合 `std::mutex` 和 `std::condition_variable` 实现高效、无锁等待的同步。主线程向A缓冲区写入时，渲染线程可以安全地读取B缓冲区，反之亦然。

---

## 5. 详细工作流程

1.  **启动**: `JzREInstance` 创建 `Frontend` 和 `Backend`，并启动渲染线程（`Backend::run`）。
2.  **主线程循环**:
    1.  等待渲染线程完成上一帧的渲染（通过条件变量）。
    2.  更新所有游戏逻辑和UI逻辑。
    3.  所有渲染操作都通过 `Frontend` 提交，生成命令并填充到 **写缓冲区**。
    4.  调用 `ImGui::Render()`，将其 `ImDrawData` **深拷贝** 并作为 `RenderImGuiCommand` 提交。
    5.  调用 `Frontend::present()`，该函数交换读写缓冲区，并 **唤醒渲染线程**。
    6.  主线程无需等待，立即开始下一帧的逻辑更新。
3.  **渲染线程循环**:
    1.  使用条件变量 **等待** 主线程的唤醒信号。
    2.  获取到包含完整一帧指令的 **读缓冲区**。
    3.  遍历命令，通过 `std::visit` 解析并执行每个命令对应的 RHI 调用。
    4.  调用 `swapBuffers()` 将结果显示到屏幕。
    5.  清空刚处理完的命令缓冲区，并 **通知主线程** 自己已就绪，可以接收下一帧。

---

## 6. 实施计划

1.  **创建新组件**: 在 `src/RHI/` 或 `src/Editor/` 下实现 `JzRenderCommand.h`, `JzRenderFrontend.h/cpp`, `JzRenderBackend.h/cpp`。
2.  **重构 `JzRHIRenderer`**: 将其功能迁移至 `JzRenderBackend`。
3.  **修改 `JzREInstance`**: 调整 `run()` 循环，集成前后端，并管理渲染线程的生命周期。
4.  **修改 `JzUIManager` 和 `JzEditor`**: 将所有直接渲染调用替换为向 `JzRenderFrontend` 提交命令。

## 7. 收益

-   **性能与流畅度**: 逻辑更新与UI响应不再被渲染阻塞，帧率更稳定平滑。
-   **高CPU利用率**: 在多核CPU上实现真正的并行计算。
-   **架构清晰与可扩展**: 完美契合RHI的抽象设计，未来可轻松接入Vulkan、Metal等其他后端。

---

## 8. 与ECS系统的整合方案

将新的渲染架构与ECS结合的关键在于重新定义 `JzRenderSystem` 的职责。它将不再是渲染的“执行者”，而是渲染命令的“生成者”。数据流将变为单向：

**ECS (实体/组件) -> `RenderSystem` -> `JzRenderCommand` -> `JzRenderFrontend` -> `JzRenderBackend` -> GPU**

### a. `JzRenderSystem` 的新职责 (主线程)

`JzRenderSystem` 依然在主线程上作为ECS更新的一部分被调用，但其内部逻辑变为遍历实体、收集数据、创建渲染命令并提交给 `JzRenderFrontend`。

```cpp
// JzRenderSystem.h
class JzRenderSystem : public System {
public:
    // frontend的引用在构造时传入
    JzRenderSystem(JzRenderFrontend& frontend);
    void update(EntityManager& entityManager, float dt) override;
private:
    JzRenderFrontend& mRenderFrontend;
};

// JzRenderSystem.cpp
void JzRenderSystem::update(EntityManager& entityManager, float dt) {
    // 1. 获取所有可见的、需要渲染的实体
    //    (可能由 JzVisibilitySystem 提前计算好)
    auto renderableView = entityManager.view<TransformComponent, MeshComponent, MaterialComponent>();

    // 2. 遍历实体，生成渲染命令
    for (auto entity : renderableView) {
        auto& transform = renderableView.get<TransformComponent>(entity);
        auto& mesh = renderableView.get<MeshComponent>(entity);
        auto& material = renderableView.get<MaterialComponent>(entity);

        // 3. 组装一个 DrawMeshCommand
        DrawMeshCommand cmd;
        cmd.transform = transform.getWorldMatrix(); // 变换已由 SceneGraphSystem 计算好
        cmd.vertexBuffer = mesh.getVertexBufferRHI();
        cmd.indexBuffer = mesh.getIndexBufferRHI();
        cmd.pipelineState = material.getPipelineStateRHI();
        // ... 填充其他材质数据

        // 4. 提交命令给渲染前端
        mRenderFrontend.submit(cmd);
    }
}
```

### b. ECS 系统间的协作 (主线程)

为了最高效地工作，ECS内的相关系统需要按特定顺序执行：

1.  **`JzSceneGraphSystem`**: 首先运行，计算出每个实体的最终世界坐标变换矩阵。
2.  **`JzVisibilitySystem`**: 接着运行，利用世界坐标进行视锥剔除，生成“可见实体列表”。
3.  **`JzRenderSystem`**: 最后运行，它只遍历“可见实体列表”，为可见物体生成渲染命令并提交。

### c. 渲染线程 (无变化)

渲染线程的 `JzRenderBackend` **完全不需要知道ECS的存在**。它接收到的 `DrawMeshCommand` 是纯粹的渲染数据，它只关心RHI资源句柄和参数，并据此执行绘图。这维持了完美的关注点分离。

### d. 整合优势

-   **高度解耦**: ECS系统与渲染API实现完全分离。`RenderSystem` 仅作为数据转换的桥梁。
-   **逻辑清晰**: 主线程负责“决定画什么”（逻辑、剔除），渲染线程只负责“去画”（执行命令）。
-   **性能优化**: 剔除工作在主线程完成，无效实体不产生渲染命令，减轻了渲染线程负担。
-   **易于调试**: 可以拦截、序列化甚至回放 `CommandBuffer` 中的渲染命令，便于调试复杂的渲染问题。