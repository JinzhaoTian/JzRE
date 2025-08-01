#include "JzMultithreadedRenderManager.h"
#include "JzRHIContext.h"
#include "JzRHIFactory.h"

using namespace JzRE;

/**
 * @brief RHI Usage Example: Demonstrates how to use the new RHI system for cross-platform rendering
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

    // Create texture
    JzTextureDesc textureDesc;
    textureDesc.type      = JzETextureType::Texture2D;
    textureDesc.format    = JzETextureFormat::RGBA8;
    textureDesc.width     = 256;
    textureDesc.height    = 256;
    textureDesc.debugName = "TestTexture";

    auto texture = device->CreateTexture(textureDesc);
    std::cout << "Create texture: " << texture->GetDebugName()
              << " (" << texture->GetWidth() << "x" << texture->GetHeight() << ")" << std::endl;

    // Create shader
    JzShaderDesc vertexShaderDesc;
    vertexShaderDesc.type      = JzEShaderType::Vertex;
    vertexShaderDesc.source    = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        void main() {
            gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
        }
    )";
    vertexShaderDesc.debugName = "BasicVertexShader";

    JzShaderDesc fragmentShaderDesc;
    fragmentShaderDesc.type      = JzEShaderType::Fragment;
    fragmentShaderDesc.source    = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
        }
    )";
    fragmentShaderDesc.debugName = "BasicFragmentShader";

    auto vertexShader   = device->CreateShader(vertexShaderDesc);
    auto fragmentShader = device->CreateShader(fragmentShaderDesc);
    std::cout << "Create shader: " << vertexShader->GetDebugName()
              << " & " << fragmentShader->GetDebugName() << std::endl;

    // Create pipeline
    JzPipelineDesc pipelineDesc;
    pipelineDesc.shaders               = {vertexShaderDesc, fragmentShaderDesc};
    pipelineDesc.renderState.depthTest = true;
    pipelineDesc.renderState.cullMode  = JzECullMode::Back;
    pipelineDesc.debugName             = "BasicPipeline";

    auto pipeline = device->CreatePipeline(pipelineDesc);
    std::cout << "Create pipeline: " << pipeline->GetDebugName() << std::endl;

    // Create vertex array object
    auto vertexArray = device->CreateVertexArray("TriangleVAO");
    vertexArray->BindVertexBuffer(vertexBuffer);
    vertexArray->BindIndexBuffer(indexBuffer);
    vertexArray->SetVertexAttribute(0, 3, 3 * sizeof(F32), 0);
    std::cout << "Create vertex array object: " << vertexArray->GetDebugName() << std::endl;

    // 3. Immediate Rendering Mode Example
    std::cout << "\n=== Immediate Rendering Mode Example ===" << std::endl;

    device->BeginFrame();

    // Clear screen
    JzClearParams clearParams;
    clearParams.clearColor = true;
    clearParams.clearDepth = true;
    clearParams.colorR     = 0.2f;
    clearParams.colorG     = 0.3f;
    clearParams.colorB     = 0.3f;
    clearParams.colorA     = 1.0f;
    device->Clear(clearParams);

    // Set viewport
    JzViewport viewport;
    viewport.x      = 0.0f;
    viewport.y      = 0.0f;
    viewport.width  = 800.0f;
    viewport.height = 600.0f;
    device->SetViewport(viewport);

    // Bind resources and draw
    device->BindPipeline(pipeline);
    device->BindVertexArray(vertexArray);
    device->BindTexture(texture, 0);

    JzDrawIndexedParams drawParams;
    drawParams.indexCount    = 3;
    drawParams.primitiveType = JzEPrimitiveType::Triangles;
    device->DrawIndexed(drawParams);

    device->EndFrame();
    std::cout << "Immediate rendering mode: triangle drawing completed" << std::endl;

    // 4. Command Buffer Mode Example
    std::cout << "\n=== Command Buffer Mode Example ===" << std::endl;

    auto commandBuffer = device->CreateCommandBuffer("ExampleCommandBuffer");
    commandBuffer->Begin();

    // Record rendering commands
    commandBuffer->Clear(clearParams);
    commandBuffer->SetViewport(viewport);
    commandBuffer->BindPipeline(pipeline);
    commandBuffer->BindVertexArray(vertexArray);
    commandBuffer->BindTexture(texture, 0);
    commandBuffer->DrawIndexed(drawParams);

    commandBuffer->End();

    // Execute command buffer
    device->ExecuteCommandBuffer(commandBuffer);
    std::cout << "Command buffer mode: executed " << commandBuffer->GetCommandCount() << " commands" << std::endl;

    // 5. Multithreading Rendering Example
    std::cout << "\n=== Multithreading Rendering Example ===" << std::endl;

    if (device->SupportsMultithreading()) {
        std::cout << "Device supports multithreading rendering" << std::endl;

        // Set thread count
        rhiContext.SetThreadCount(4);
        std::cout << "Set rendering thread count: " << rhiContext.GetThreadCount() << std::endl;

        // Create multiple command buffers for parallel recording
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
        std::cout << "Multithreading rendering: completed parallel execution of 3 command buffers" << std::endl;
    } else {
        std::cout << "Device does not support multithreading rendering" << std::endl;
    }

    // 6. Performance Statistics Example
    std::cout << "\n=== Performance Statistics Example ===" << std::endl;
    // Note: This requires OpenGLDevice implementation of GetStats method
    std::cout << "Current RHI type: " << JzRHIFactory::GetRHITypeName(device->GetRHIType()) << std::endl;
    std::cout << "Device name: " << device->GetDeviceName() << std::endl;
    std::cout << "Vendor name: " << device->GetVendorName() << std::endl;

    // 7. Resource Cleanup
    std::cout << "\n=== Resource Cleanup ===" << std::endl;

    // RHI resources will be automatically released, but we can manually clean up
    vertexBuffer.reset();
    indexBuffer.reset();
    texture.reset();
    vertexArray.reset();
    pipeline.reset();
    commandBuffer.reset();

    std::cout << "Resource cleanup completed" << std::endl;

    // 8. Close RHI system
    rhiContext.Shutdown();
    std::cout << "\n=== RHI system closed ===" << std::endl;

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