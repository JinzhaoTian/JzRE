/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <glad/glad.h>
#include "JzRE/RHI/JzGPUBufferObject.h"
#include "JzRE/Core/JzRETypes.h"

namespace JzRE {
/**
 * @brief OpenGL Implementation of RHI Buffer
 */
class JzOpenGLBuffer : public JzGPUBufferObject {
public:
    /**
     * @brief Constructor
     * @param desc The description of the buffer
     */
    JzOpenGLBuffer(const JzGPUBufferObjectDesc &desc);

    /**
     * @brief Destructor
     */
    ~JzOpenGLBuffer() override;

    /**
     * @brief Update the data
     * @param data The data to update
     * @param size The size of the data
     * @param offset The offset of the data
     */
    void UpdateData(const void *data, Size size, Size offset = 0) override;

    /**
     * @brief Map the buffer
     * @return The mapped data
     */
    void *MapBuffer() override;

    /**
     * @brief Unmap the buffer
     */
    void UnmapBuffer() override;

    /**
     * @brief Get the handle of the buffer
     * @return The handle of the buffer
     */
    GLuint GetHandle() const;

private:
    GLuint m_handle = 0;
    GLenum m_target;
    GLenum m_usage;
};
} // namespace JzRE