/**
 * @file rhi_window_example.cpp
 * @brief Example showing how to use RHI-integrated JzWindow
 */

#include "JzContext_RHI.h"
#include "JzRHIDescription.h"
#include "JzRE/RHI/JzRHIETypes.h"
#include <iostream>

namespace JzRE {

class RHIWindowExample {
public:
    RHIWindowExample()  = default;
    ~RHIWindowExample() = default;

    Bool Initialize()
    {
        try {
            // 创建 RHI 集成的上下文
            m_context = std::make_unique<JzContext>(JzERHIType::OpenGL);

            if (!m_context->GetRHIDevice()) {
                std::cerr << "Failed to create RHI device!" << std::endl;
                return false;
            }

            std::cout << "RHI Device: " << m_context->GetRHIDevice()->GetDeviceName() << std::endl;
            std::cout << "Vendor: " << m_context->GetRHIDevice()->GetVendorName() << std::endl;
            std::cout << "Driver: " << m_context->GetRHIDevice()->GetDriverVersion() << std::endl;

            return true;
        } catch (const std::exception &e) {
            std::cerr << "Initialization failed: " << e.what() << std::endl;
            return false;
        }
    }

    void Run()
    {
        if (!m_context || !m_context->window) {
            return;
        }

        // 设置清除参数
        JzClearParams clearParams{};
        clearParams.clearColor   = true;
        clearParams.clearDepth   = true;
        clearParams.clearStencil = false;
        clearParams.colorValue   = {0.2f, 0.3f, 0.8f, 1.0f}; // 蓝色背景
        clearParams.depthValue   = 1.0f;
        clearParams.stencilValue = 0;

        // 主渲染循环
        while (!m_context->window->ShouldClose()) {
            // 处理窗口事件
            m_context->device->PollEvents();

            // === 使用 RHI 设备进行渲染 ===

            // 1. 开始帧
            m_context->BeginFrame();

            // 2. 设置视口（自动根据窗口大小）
            JzViewport viewport = m_context->GetWindowViewport();
            m_context->SetViewport(viewport);

            // 3. 清除屏幕
            m_context->Clear(clearParams);

            // 4. 这里可以添加你的渲染代码
            RenderContent();

            // 5. 结束帧并呈现
            m_context->EndFrame();
            m_context->Present();
        }
    }

    void Shutdown()
    {
        if (m_context) {
            m_context.reset();
        }
    }

private:
    void RenderContent()
    {
        // 这里可以使用 RHI 设备的各种渲染方法
        auto rhiDevice = m_context->GetRHIDevice();
        if (!rhiDevice) return;

        // 示例：创建和使用缓冲区
        /*
        JzBufferDesc bufferDesc{};
        bufferDesc.size = 1024;
        bufferDesc.usage = JzEBufferUsage::Vertex;
        bufferDesc.memoryUsage = JzEMemoryUsage::CPUToGPU;

        auto vertexBuffer = rhiDevice->CreateBuffer(bufferDesc);

        // 示例：创建着色器
        JzShaderDesc shaderDesc{};
        shaderDesc.vertexShaderSource = "your_vertex_shader_source";
        shaderDesc.fragmentShaderSource = "your_fragment_shader_source";

        auto shader = rhiDevice->CreateShader(shaderDesc);

        // 示例：绘制
        JzDrawParams drawParams{};
        drawParams.vertexCount = 3;
        drawParams.firstVertex = 0;

        rhiDevice->Draw(drawParams);
        */
    }

private:
    std::unique_ptr<JzContext> m_context;
};

} // namespace JzRE

int main()
{
    JzRE::RHIWindowExample example;

    if (!example.Initialize()) {
        return -1;
    }

    example.Run();
    example.Shutdown();

    return 0;
}

// === 窗口事件处理示例 ===

namespace JzRE {

class WindowEventHandler {
public:
    static void SetupWindowEvents(JzWindow &window)
    {
        // 窗口大小改变事件
        window.WindowResizedEvent.AddListener([](U16 width, U16 height) {
            std::cout << "Window resized to: " << width << "x" << height << std::endl;

            // RHI 设备的视口会自动更新，但你可以在这里做额外的处理
            if (window.HasRHIDevice()) {
                auto rhiDevice = window.GetRHIDevice();
                if (rhiDevice) {
                    // 可以在这里重新设置渲染目标等
                }
            }
        });

        // 键盘事件
        window.KeyPressedEvent.AddListener([](I32 key) {
            if (key == GLFW_KEY_ESCAPE) {
                std::cout << "ESC key pressed, closing window..." << std::endl;
                // window.SetShouldClose(true);  // 需要访问窗口实例
            }
        });

        // 鼠标事件
        window.MouseButtonPressedEvent.AddListener([](I32 button) {
            std::cout << "Mouse button " << button << " pressed" << std::endl;
        });

        // 窗口关闭事件
        window.WindowClosedEvent.AddListener([]() {
            std::cout << "Window is closing..." << std::endl;
        });
    }
};

} // namespace JzRE

// === 多线程渲染示例 ===

namespace JzRE {

class MultiThreadedRenderExample {
public:
    void RunMultiThreaded()
    {
        JzContext context(JzERHIType::OpenGL);

        if (!context.GetRHIDevice()->SupportsMultithreading()) {
            std::cout << "RHI device does not support multithreading" << std::endl;
            return;
        }

        // 设置线程数
        context.SetThreadCount(4);

        // 在不同线程中使用 RHI 设备
        auto renderThread = [&context](U32 threadId) {
            // 为当前线程设置 RHI 上下文
            if (context.window) {
                context.window->MakeRHIContextCurrent(threadId);
            }

            // 在这个线程中进行渲染
            auto rhiDevice = context.GetRHIDevice();
            if (rhiDevice) {
                // 创建线程专用的命令缓冲区
                auto commandBuffer = rhiDevice->CreateCommandBuffer("Thread_" + std::to_string(threadId));

                // 执行渲染命令
                rhiDevice->ExecuteCommandBuffer(commandBuffer);
            }
        };

        // 启动多个渲染线程
        std::vector<std::thread> threads;
        for (U32 i = 0; i < context.GetThreadCount(); ++i) {
            threads.emplace_back(renderThread, i);
        }

        // 等待所有线程完成
        for (auto &thread : threads) {
            thread.join();
        }
    }
};

} // namespace JzRE