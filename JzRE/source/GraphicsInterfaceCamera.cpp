#include "GraphicsInterfaceCamera.h"

namespace JzRE {
GraphicsInterfaceCamera::GraphicsInterfaceCamera() :
    wndWidth(0), wndHeight(0),
    Position(glm::vec3(0.0f, 0.0f, 0.0f)), WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)), Front(glm::vec3(0.0f, 0.0f, -1.0f)),
    Yaw(YAW), Pitch(PITCH), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    Target = glm::vec3(0.0f, 0.0f, 0.0f);
    Radius = 10.0f;
    UpdateCameraVectors();
}

GraphicsInterfaceCamera::GraphicsInterfaceCamera(I32 width, I32 height, glm::vec3 position, glm::vec3 up, F32 yaw, F32 pitch) :
    wndWidth(width), wndHeight(height), Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    Target = glm::vec3(0.0f, 0.0f, 0.0f);
    Radius = 10.0f;
    UpdateCameraVectors();
}

GraphicsInterfaceCamera::GraphicsInterfaceCamera(I32 width, I32 height, F32 posX, F32 posY, F32 posZ, F32 upX, F32 upY, F32 upZ, F32 yaw, F32 pitch) :
    wndWidth(width), wndHeight(height), Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    Target = glm::vec3(0.0f, 0.0f, 0.0f);
    Radius = 10.0f;
    UpdateCameraVectors();
}

void GraphicsInterfaceCamera::ResetPosition(glm::vec3 position) {
    Position = position;
    WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    Front = glm::vec3(0.0f, 0.0f, -1.0f);
    Yaw = YAW;
    Pitch = PITCH;
    Target = glm::vec3(0.0f, 0.0f, 0.0f);
    Radius = 10.0f;
    UpdateCameraVectors();
}

glm::vec3 GraphicsInterfaceCamera::GetCameraPosition() {
    return this->Position;
}

glm::mat4 GraphicsInterfaceCamera::GetViewMatrix() {
    // return glm::lookAt(Position, Position + Front, Up);
    return glm::lookAt(Position, Target, Up);
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
    UpdateCameraVectors();
}

void GraphicsInterfaceCamera::ProcessMouseScroll(glm::vec2 mouseOffset) {
    // Zoom -= mouseOffset.y;
    // if (Zoom < 1.0f)
    //     Zoom = 1.0f;
    // if (Zoom > 45.0f)
    //     Zoom = 45.0f;

    Radius -= mouseOffset.y * MouseSensitivity;
    if (Radius < 2.0f)
        Radius = 2.0f;
    if (Radius > 10.0f)
        Radius = 10.0f;
    
    UpdateCameraVectors();
}

void GraphicsInterfaceCamera::ProcessKeyboardMovement(glm::vec2 mouseDelta) {
    Position += Right * mouseDelta.x * MouseSensitivity;
    Position += Up * mouseDelta.y * MouseSensitivity;
    UpdateCameraVectors();
}

void GraphicsInterfaceCamera::UpdateCameraVectors() {
    glm::vec3 offset;
    offset.x = Radius * cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    offset.y = Radius * sin(glm::radians(Pitch));
    offset.z = Radius * sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Position = Target + offset;

    Right = glm::normalize(glm::cross(Position - Target, WorldUp));
    Up = glm::normalize(glm::cross(Right, Position - Target));

    // // calculate the new Front vector
    // glm::vec3 front;
    // front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    // front.y = sin(glm::radians(Pitch));
    // front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    // Front = glm::normalize(front);
    // // also re-calculate the Right and Up vector
    // Right = glm::normalize(glm::cross(Front, WorldUp)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    // Up = glm::normalize(glm::cross(Right, Front));
}
}; // namespace JzRE