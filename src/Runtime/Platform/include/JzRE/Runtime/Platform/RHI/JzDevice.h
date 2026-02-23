/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <memory>
#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/Command/JzRHICommandList.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUBufferObject.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUFramebufferObject.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUShaderProgramObject.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUTextureObject.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUVertexArrayObject.h"
#include "JzRE/Runtime/Platform/RHI/JzRHIPipeline.h"

namespace JzRE {

/**
 * @brief Supported graphics API types.
 */
enum class JzERHIType : U8 {
    Unknown = 0,
    OpenGL,
    Vulkan,
    D3D11,
    D3D12,
    Metal
};

/**
 * @brief Render path mode.
 */
enum class JzERenderMode : U8 {
    Immediate,
    CommandBuffer
};

/**
 * @brief Interface of RHI device.
 */
class JzDevice {
public:
    /**
     * @brief Constructor.
     *
     * @param rhiType Backend API type.
     */
    explicit JzDevice(JzERHIType rhiType) :
        rhiType(rhiType)
    { }

    /**
     * @brief Destructor.
     */
    virtual ~JzDevice() = default;

    /**
     * @brief Get backend API type.
     */
    JzERHIType GetRHIType() const
    {
        return rhiType;
    }

    /**
     * @brief Get device name.
     */
    virtual String GetDeviceName() const = 0;

    /**
     * @brief Get vendor name.
     */
    virtual String GetVendorName() const = 0;

    /**
     * @brief Get driver version.
     */
    virtual String GetDriverVersion() const = 0;

    /**
     * @brief Create a GPU buffer object.
     */
    virtual std::shared_ptr<JzGPUBufferObject> CreateBuffer(const JzGPUBufferObjectDesc &desc) = 0;

    /**
     * @brief Create a GPU texture object.
     */
    virtual std::shared_ptr<JzGPUTextureObject> CreateTexture(const JzGPUTextureObjectDesc &desc) = 0;

    /**
     * @brief Create a GPU shader object.
     */
    virtual std::shared_ptr<JzGPUShaderProgramObject> CreateShader(const JzShaderProgramDesc &desc) = 0;

    /**
     * @brief Create a GPU pipeline object.
     */
    virtual std::shared_ptr<JzRHIPipeline> CreatePipeline(const JzPipelineDesc &desc) = 0;

    /**
     * @brief Create a framebuffer object.
     */
    virtual std::shared_ptr<JzGPUFramebufferObject> CreateFramebuffer(const String &debugName = "") = 0;

    /**
     * @brief Create a vertex array object.
     */
    virtual std::shared_ptr<JzGPUVertexArrayObject> CreateVertexArray(const String &debugName = "") = 0;

    /**
     * @brief Create one command list for recording draw commands.
     */
    virtual std::shared_ptr<JzRHICommandList> CreateCommandList(const String &debugName = "") = 0;

    /**
     * @brief Execute one recorded command list.
     */
    virtual void ExecuteCommandList(std::shared_ptr<JzRHICommandList> commandList) = 0;

    /**
     * @brief Execute a batch of recorded command lists in order.
     */
    virtual void ExecuteCommandLists(const std::vector<std::shared_ptr<JzRHICommandList>> &commandLists) = 0;

    /**
     * @brief Begin frame recording/execution.
     */
    virtual void BeginFrame() = 0;

    /**
     * @brief End frame recording/execution.
     */
    virtual void EndFrame() = 0;

    /**
     * @brief Flush backend queue/present stage.
     */
    virtual void Flush() = 0;

    /**
     * @brief Block until device idle.
     */
    virtual void Finish() = 0;

    /**
     * @brief Whether backend supports multi-threaded recording/submission.
     */
    virtual Bool SupportsMultithreading() const = 0;

protected:
    JzERHIType rhiType;
};

} // namespace JzRE
