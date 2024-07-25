#include "GraphicsInterfaceCamera.h"

namespace JzRE {
GraphicsInterfaceCamera::GraphicsInterfaceCamera() :
    wndWidth(0), wndHeight(0),
    Position(glm::vec3(0.0f, 0.0f, 0.0f)), WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)), Front(glm::vec3(0.0f, 0.0f, -1.0f)),
    Yaw(YAW), Pitch(PITCH), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    updateCameraVectors();
}

GraphicsInterfaceCamera::GraphicsInterfaceCamera(I32 width, I32 height, glm::vec3 position, glm::vec3 up, F32 yaw, F32 pitch) :
    wndWidth(width), wndHeight(height), Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

GraphicsInterfaceCamera::GraphicsInterfaceCamera(I32 width, I32 height, F32 posX, F32 posY, F32 posZ, F32 upX, F32 upY, F32 upZ, F32 yaw, F32 pitch) :
    wndWidth(width), wndHeight(height), Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

Bool GraphicsInterfaceCamera::Initialize(I32 width, I32 height, glm::vec3 position, glm::vec3 up) {
    wndWidth = width;
    wndHeight = height;

    Position = position;
    WorldUp = up;
    updateCameraVectors();

    return true;
}

glm::mat4 GraphicsInterfaceCamera::GetViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
}

glm::mat4 GraphicsInterfaceCamera::GetProjectionMatrix() {
    return glm::perspective(glm::radians(Zoom), (F32)this->wndWidth / (F32)this->wndHeight, 0.1f, 100.0f);
}

void GraphicsInterfaceCamera::ProcessMouseMovement(glm::vec2 mouseDelta, GLboolean constrainPitch) {
    Yaw += mouseDelta.x * MouseSensitivity;
    Pitch += mouseDelta.y * MouseSensitivity;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void GraphicsInterfaceCamera::ProcessMouseScroll(glm::vec2 mouseOffset) {
    Zoom -= mouseOffset.y;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void GraphicsInterfaceCamera::ProcessKeyboardMovement(glm::vec2 mouseDelta) {
    Position += Right * mouseDelta.x * MovementSpeed;
    Position += Up * mouseDelta.y * MovementSpeed;
}

void GraphicsInterfaceCamera::updateCameraVectors() {
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}
}; // namespace JzRE