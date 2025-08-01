#include "RHI.h"
#include "RHIRenderer.h"
#include "RHIMultithreading.h"

using namespace JzRE;

/**
 * RHI使用示例
 * 展示如何使用新的RHI系统进行跨平台渲染
 */
int main() {
    std::cout << "=== JzRE RHI 使用示例 ===" << std::endl;

    // 1. 初始化RHI系统
    auto& rhiContext = RHIContext::GetInstance();
    
    // 列出支持的图形API
    std::cout << "支持的图形API: ";
    auto supportedTypes = RHIFactory::GetSupportedRHITypes();
    for (auto type : supportedTypes) {
        std::cout << RHIFactory::GetRHITypeName(type) << " ";
    }
    std::cout << std::endl;

    // 初始化RHI（自动选择最佳API）
    if (!rhiContext.Initialize()) {
        std::cerr << "RHI初始化失败" << std::endl;
        return -1;
    }

    auto device = rhiContext.GetDevice();
    
    // 2. 创建基本资源示例
    std::cout << "\n=== 创建RHI资源 ===" << std::endl;
    
    // 创建顶点缓冲
    F32 vertices[] = {
        -0.5f, -0.5f, 0.0f,  // 左下
         0.5f, -0.5f, 0.0f,  // 右下
         0.0f,  0.5f, 0.0f   // 顶部
    };
    
    BufferDesc vertexBufferDesc;
    vertexBufferDesc.type = EBufferType::Vertex;
    vertexBufferDesc.usage = EBufferUsage::StaticDraw;
    vertexBufferDesc.size = sizeof(vertices);
    vertexBufferDesc.data = vertices;
    vertexBufferDesc.debugName = "TriangleVertexBuffer";
    
    auto vertexBuffer = device->CreateBuffer(vertexBufferDesc);
    std::cout << "创建顶点缓冲: " << vertexBuffer->GetDebugName() << std::endl;

    // 创建索引缓冲
    U32 indices[] = {0, 1, 2};
    
    BufferDesc indexBufferDesc;
    indexBufferDesc.type = EBufferType::Index;
    indexBufferDesc.usage = EBufferUsage::StaticDraw;
    indexBufferDesc.size = sizeof(indices);
    indexBufferDesc.data = indices;
    indexBufferDesc.debugName = "TriangleIndexBuffer";
    
    auto indexBuffer = device->CreateBuffer(indexBufferDesc);
    std::cout << "创建索引缓冲: " << indexBuffer->GetDebugName() << std::endl;

    // 创建纹理
    TextureDesc textureDesc;
    textureDesc.type = ETextureType::Texture2D;
    textureDesc.format = ETextureFormat::RGBA8;
    textureDesc.width = 256;
    textureDesc.height = 256;
    textureDesc.debugName = "TestTexture";
    
    auto texture = device->CreateTexture(textureDesc);
    std::cout << "创建纹理: " << texture->GetDebugName() 
              << " (" << texture->GetWidth() << "x" << texture->GetHeight() << ")" << std::endl;

    // 创建着色器
    ShaderDesc vertexShaderDesc;
    vertexShaderDesc.type = EShaderType::Vertex;
    vertexShaderDesc.source = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        void main() {
            gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
        }
    )";
    vertexShaderDesc.debugName = "BasicVertexShader";
    
    ShaderDesc fragmentShaderDesc;
    fragmentShaderDesc.type = EShaderType::Fragment;
    fragmentShaderDesc.source = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
        }
    )";
    fragmentShaderDesc.debugName = "BasicFragmentShader";

    auto vertexShader = device->CreateShader(vertexShaderDesc);
    auto fragmentShader = device->CreateShader(fragmentShaderDesc);
    std::cout << "创建着色器: " << vertexShader->GetDebugName() 
              << " & " << fragmentShader->GetDebugName() << std::endl;

    // 创建渲染管线
    PipelineDesc pipelineDesc;
    pipelineDesc.shaders = {vertexShaderDesc, fragmentShaderDesc};
    pipelineDesc.renderState.depthTest = true;
    pipelineDesc.renderState.cullMode = ECullMode::Back;
    pipelineDesc.debugName = "BasicPipeline";
    
    auto pipeline = device->CreatePipeline(pipelineDesc);
    std::cout << "创建渲染管线: " << pipeline->GetDebugName() << std::endl;

    // 创建顶点数组对象
    auto vertexArray = device->CreateVertexArray("TriangleVAO");
    vertexArray->BindVertexBuffer(vertexBuffer);
    vertexArray->BindIndexBuffer(indexBuffer);
    vertexArray->SetVertexAttribute(0, 3, 3 * sizeof(F32), 0);
    std::cout << "创建顶点数组对象: " << vertexArray->GetDebugName() << std::endl;

    // 3. 立即渲染模式示例
    std::cout << "\n=== 立即渲染模式示例 ===" << std::endl;
    
    device->BeginFrame();
    
    // 清除屏幕
    ClearParams clearParams;
    clearParams.clearColor = true;
    clearParams.clearDepth = true;
    clearParams.colorR = 0.2f;
    clearParams.colorG = 0.3f;
    clearParams.colorB = 0.3f;
    clearParams.colorA = 1.0f;
    device->Clear(clearParams);
    
    // 设置视口
    Viewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = 800.0f;
    viewport.height = 600.0f;
    device->SetViewport(viewport);
    
    // 绑定资源并绘制
    device->BindPipeline(pipeline);
    device->BindVertexArray(vertexArray);
    device->BindTexture(texture, 0);
    
    DrawIndexedParams drawParams;
    drawParams.indexCount = 3;
    drawParams.primitiveType = EPrimitiveType::Triangles;
    device->DrawIndexed(drawParams);
    
    device->EndFrame();
    std::cout << "立即渲染模式: 绘制三角形完成" << std::endl;

    // 4. 命令缓冲模式示例
    std::cout << "\n=== 命令缓冲模式示例 ===" << std::endl;
    
    auto commandBuffer = device->CreateCommandBuffer("ExampleCommandBuffer");
    commandBuffer->Begin();
    
    // 记录渲染命令
    commandBuffer->Clear(clearParams);
    commandBuffer->SetViewport(viewport);
    commandBuffer->BindPipeline(pipeline);
    commandBuffer->BindVertexArray(vertexArray);
    commandBuffer->BindTexture(texture, 0);
    commandBuffer->DrawIndexed(drawParams);
    
    commandBuffer->End();
    
    // 执行命令缓冲
    device->ExecuteCommandBuffer(commandBuffer);
    std::cout << "命令缓冲模式: 执行了 " << commandBuffer->GetCommandCount() << " 个命令" << std::endl;

    // 5. 多线程渲染示例
    std::cout << "\n=== 多线程渲染示例 ===" << std::endl;
    
    if (device->SupportsMultithreading()) {
        std::cout << "设备支持多线程渲染" << std::endl;
        
        // 设置线程数量
        rhiContext.SetThreadCount(4);
        std::cout << "设置渲染线程数: " << rhiContext.GetThreadCount() << std::endl;
        
        // 创建多个命令缓冲用于并行录制
        auto commandQueue = rhiContext.GetCommandQueue();
        
        for (U32 i = 0; i < 3; ++i) {
            auto cmd = commandQueue->CreateCommandBuffer("ParallelCommand" + std::to_string(i));
            cmd->Begin();
            cmd->Clear(clearParams);
            cmd->SetViewport(viewport);
            cmd->BindPipeline(pipeline);
            cmd->BindVertexArray(vertexArray);
            cmd->DrawIndexed(drawParams);
            cmd->End();
            
            commandQueue->SubmitCommandBuffer(cmd);
        }
        
        commandQueue->ExecuteAll();
        commandQueue->Wait();
        std::cout << "多线程渲染: 并行执行3个命令缓冲完成" << std::endl;
    } else {
        std::cout << "设备不支持多线程渲染" << std::endl;
    }

    // 6. 性能统计示例
    std::cout << "\n=== 性能统计 ===" << std::endl;
    // 注意：这里需要OpenGLDevice实现GetStats方法
    std::cout << "当前RHI类型: " << RHIFactory::GetRHITypeName(device->GetRHIType()) << std::endl;
    std::cout << "设备名称: " << device->GetDeviceName() << std::endl;
    std::cout << "厂商: " << device->GetVendorName() << std::endl;

    // 7. 资源清理
    std::cout << "\n=== 清理资源 ===" << std::endl;
    
    // RHI资源会自动释放，但我们可以手动清理
    vertexBuffer.reset();
    indexBuffer.reset();
    texture.reset();
    vertexArray.reset();
    pipeline.reset();
    commandBuffer.reset();
    
    std::cout << "资源清理完成" << std::endl;

    // 8. 关闭RHI系统
    rhiContext.Shutdown();
    std::cout << "\n=== RHI系统已关闭 ===" << std::endl;

    return 0;
}

/**
 * 更高级的多线程渲染示例
 */
void AdvancedMultithreadingExample() {
    std::cout << "\n=== 高级多线程渲染示例 ===" << std::endl;
    
    // 初始化多线程渲染管理器
    MultithreadedRenderManager renderManager;
    if (!renderManager.Initialize(4)) {
        std::cerr << "多线程渲染管理器初始化失败" << std::endl;
        return;
    }
    
    std::cout << "多线程渲染管理器已启动，线程数: " << renderManager.GetThreadCount() << std::endl;
    
    // 模拟游戏循环
    for (U32 frame = 0; frame < 10; ++frame) {
        renderManager.BeginFrame();
        
        // 提交渲染任务
        renderManager.SubmitRenderPass([frame]() {
            std::cout << "  渲染帧 " << frame << " 在线程 " 
                      << std::this_thread::get_id() << " 上执行" << std::endl;
            
            // 模拟渲染工作
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        });
        
        renderManager.EndFrame();
        renderManager.Present();
        
        std::cout << "帧 " << frame << " 渲染完成，平均帧时间: " 
                  << renderManager.GetAverageFrameTime() << "ms" << std::endl;
    }
    
    renderManager.Shutdown();
    std::cout << "多线程渲染示例完成" << std::endl;
} 