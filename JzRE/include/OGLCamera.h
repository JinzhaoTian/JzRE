#pragma once

#include "CommonTypes.h"

namespace JzRE {

// Default camera values
const F32 YAW = -90.0f;
const F32 PITCH = 0.0f;
const F32 SPEED = 0.01f;
const F32 SENSITIVITY = 0.1f;
const F32 ZOOM = 45.0f;

class OGLCamera {
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    F32 Yaw;
    F32 Pitch;
    // camera options
    F32 MovementSpeed;
    F32 MouseSensitivity;
    F32 Zoom;

    glm::vec3 Target;
    F32 Radius; // 与目标的距离

    I32 wndWidth;
    I32 wndHeight;

    OGLCamera();
    OGLCamera(I32 width, I32 height,
                            glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                            F32 yaw = YAW, F32 pitch = PITCH);
    OGLCamera(I32 width, I32 height,
                            F32 posX, F32 posY, F32 posZ, F32 upX, F32 upY, F32 upZ,
                            F32 yaw = YAW, F32 pitch = PITCH);

    void ResetPosition(glm::vec3 position);
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();
    glm::vec3 GetCameraPosition();
    void ProcessMouseMovement(glm::vec2 mouseDelta, GLboolean constrainPitch = true);
    void ProcessMouseScroll(glm::vec2 mouseOffset);
    void ProcessKeyboardMovement(glm::vec2 mouseDelta);

private:
    void UpdateCameraVectors();
};
}; // namespace JzRE