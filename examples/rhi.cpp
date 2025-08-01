#include "JzMultithreadedRenderManager.h"
#include "JzRHIContext.h"
#include "JzRHIFactory.h"

using namespace JzRE;

/**
 * @brief RHI Usage Example: Demonstrates how to use the new RHI system for cross-platform rendering
 * including runtime API switching capabilities
 */
int main()
{
    std::cout << "=== JzRE RHI Usage Example ===" << std::endl;

    // 1. Initialize RHI system
    auto &rhiContext = JzRHIContext::GetInstance();

    // List supported graphics APIs
    std::cout << "Supported Graphics APIs: ";
    auto supportedTypes = JzRHIFactory::GetSupportedRHITypes();
    for (auto type : supportedTypes) {
        std::cout << JzRHIFactory::GetRHITypeName(type) << " ";
    }
    std::cout << std::endl;

    // Initialize RHI (automatically selects the best API)
    if (!rhiContext.Initialize()) {
        std::cerr << "RHI initialization failed" << std::endl;
        return -1;
    }

    auto device = rhiContext.GetDevice();

    // 2. Create basic resource example
    std::cout << "\n=== Create RHI Resources ===" << std::endl;

    // Create vertex buffer
    F32 vertices[] = {
        -0.5f, -0.5f, 0.0f, // bottom left
        0.5f, -0.5f, 0.0f,  // bottom right
        0.0f, 0.5f, 0.0f    // top
    };

    JzBufferDesc vertexBufferDesc;
    vertexBufferDesc.type      = JzEBufferType::Vertex;
    vertexBufferDesc.usage     = JzEBufferUsage::StaticDraw;
    vertexBufferDesc.size      = sizeof(vertices);
    vertexBufferDesc.data      = vertices;
    vertexBufferDesc.debugName = "TriangleVertexBuffer";

    auto vertexBuffer = device->CreateBuffer(vertexBufferDesc);
    std::cout << "Create vertex buffer: " << vertexBuffer->GetDebugName() << std::endl;

    // Create index buffer
    U32 indices[] = {0, 1, 2};

    JzBufferDesc indexBufferDesc;
    indexBufferDesc.type      = JzEBufferType::Index;
    indexBufferDesc.usage     = JzEBufferUsage::StaticDraw;
    indexBufferDesc.size      = sizeof(indices);
    indexBufferDesc.data      = indices;
    indexBufferDesc.debugName = "TriangleIndexBuffer";

    auto indexBuffer = device->CreateBuffer(indexBufferDesc);
    std::cout << "Create index buffer: " << indexBuffer->GetDebugName() << std::endl;

    // 3. 新增：演示运行时RHI切换
    std::cout << "\n=== Runtime RHI Switching Demo ===" << std::endl;
    
    // 注册切换回调
    rhiContext.RegisterSwitchCallback("MainCallback", [](JzERHIType oldType, JzERHIType newType) {
        std::cout << "RHI切换回调: " << static_cast<int>(oldType) << " -> " << static_cast<int>(newType) << std::endl;
    });
    
    // 注册资源迁移回调（简化示例）
    rhiContext.RegisterMigrationCallback("Buffer", [](const JzRHIStateSnapshot& snapshot, std::shared_ptr<JzRHIDevice> device) -> Bool {
        std::cout << "保存缓冲区资源..." << std::endl;
        // 这里应该实际保存缓冲区数据到snapshot
        return true;
    });
    
    // 设置备用RHI
    rhiContext.SetFallbackRHI(JzERHIType::OpenGL);
    
    std::cout << "当前RHI: " << JzRHIFactory::GetRHITypeName(rhiContext.GetRHIType()) << std::endl;
    
    // 尝试切换到不同的RHI（如果可用）
    for (auto targetType : supportedTypes) {
        if (targetType != rhiContext.GetRHIType()) {
            std::cout << "\n尝试切换到: " << JzRHIFactory::GetRHITypeName(targetType) << std::endl;
            
            if (rhiContext.CanSwitchRHI(targetType)) {
                Bool switchResult = rhiContext.SwitchRHI(targetType);
                
                if (switchResult) {
                    std::cout << "✅ 切换成功!" << std::endl;
                    
                    // 验证切换后仍可创建资源
                    auto newDevice = rhiContext.GetDevice();
                    auto testBuffer = newDevice->CreateBuffer(vertexBufferDesc);
                    if (testBuffer) {
                        std::cout << "✅ 新设备资源创建测试通过" << std::endl;
                    }
                    
                    // 演示完成后切换回原来的RHI
                    // （实际应用中可能不需要）
                    
                } else {
                    std::cout << "❌ 切换失败" << std::endl;
                }
            } else {
                std::cout << "⚠️ 无法切换到此RHI类型" << std::endl;
            }
            
            break; // 只演示一次切换
        }
    }
    
    // 4. 新增：性能和状态监控
    std::cout << "\n=== RHI State Monitoring ===" << std::endl;
    std::cout << "当前RHI类型: " << JzRHIFactory::GetRHITypeName(rhiContext.GetRHIType()) << std::endl;
    std::cout << "是否正在切换: " << (rhiContext.IsSwitchingRHI() ? "是" : "否") << std::endl;
    std::cout << "备用RHI类型: " << JzRHIFactory::GetRHITypeName(rhiContext.GetFallbackRHI()) << std::endl;
    
    // 5. 新增：交互式切换演示
    std::cout << "\n=== Interactive Switching Demo ===" << std::endl;
    std::cout << "支持的RHI类型:" << std::endl;
    
    for (size_t i = 0; i < supportedTypes.size(); ++i) {
        std::cout << i + 1 << ". " << JzRHIFactory::GetRHITypeName(supportedTypes[i]);
        if (supportedTypes[i] == rhiContext.GetRHIType()) {
            std::cout << " (当前)";
        }
        std::cout << std::endl;
    }
    
    std::cout << "\n输入数字选择要切换的RHI (0退出): ";
    int choice;
    while (std::cin >> choice && choice != 0) {
        if (choice > 0 && choice <= static_cast<int>(supportedTypes.size())) {
            JzERHIType selectedType = supportedTypes[choice - 1];
            
            if (rhiContext.CanSwitchRHI(selectedType)) {
                std::cout << "正在切换到: " << JzRHIFactory::GetRHITypeName(selectedType) << std::endl;
                
                if (rhiContext.SwitchRHI(selectedType)) {
                    std::cout << "✅ 切换成功!" << std::endl;
                } else {
                    std::cout << "❌ 切换失败!" << std::endl;
                }
            } else {
                std::cout << "⚠️ 无法切换到选定的RHI类型" << std::endl;
            }
        } else {
            std::cout << "无效选择" << std::endl;
        }
        
        std::cout << "当前RHI: " << JzRHIFactory::GetRHITypeName(rhiContext.GetRHIType()) << std::endl;
        std::cout << "输入数字选择要切换的RHI (0退出): ";
    }

    // 6. 清理资源
    std::cout << "\n=== Cleanup ===" << std::endl;
    
    // 取消注册回调
    rhiContext.UnregisterSwitchCallback("MainCallback");
    rhiContext.UnregisterMigrationCallback("Buffer");
    
    // 关闭RHI系统
    rhiContext.Shutdown();
    
    std::cout << "RHI示例程序结束" << std::endl;
    return 0;
}

/**
 * Advanced multithreading rendering example
 */
void AdvancedMultithreadingExample()
{
    std::cout << "\n=== Advanced Multithreading Rendering Example ===" << std::endl;

    // Initialize multithreading render manager
    JzMultithreadedRenderManager renderManager;
    if (!renderManager.Initialize(4)) {
        std::cerr << "Multithreading render manager initialization failed" << std::endl;
        return;
    }

    std::cout << "Multithreading render manager started, thread count: " << renderManager.GetThreadCount() << std::endl;

    // Simulate game loop
    for (U32 frame = 0; frame < 10; ++frame) {
        renderManager.BeginFrame();

        // Submit rendering tasks
        renderManager.SubmitRenderPass([frame]() {
            std::cout << "   Rendering frame " << frame << " on thread "
                      << std::this_thread::get_id() << std::endl;

            // Simulate rendering work
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        });

        renderManager.EndFrame();
        renderManager.Present();

        std::cout << "Frame " << frame << " rendered, average frame time: "
                  << renderManager.GetAverageFrameTime() << "ms" << std::endl;
    }

    renderManager.Shutdown();
    std::cout << "Multithreading rendering example completed" << std::endl;
}