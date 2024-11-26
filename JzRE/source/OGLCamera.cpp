#include "OGLCamera.h"

namespace JzRE {
OGLCamera::OGLCamera() :
    wndWidth(0), wndHeight(0),
    Position(glm::vec3(0.0f, 0.0f, 0.0f)), WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)), Front(glm::vec3(0.0f, 0.0f, -1.0f)),
    Yaw(YAW), Pitch(PITCH), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    Target = glm::vec3(0.0f, 0.0f, 0.0f);
    Radius = 10.0f;
    UpdateCameraVectors();
}

OGLCamera::OGLCamera(I32 width, I32 height, glm::vec3 position, glm::vec3 up, F32 yaw, F32 pitch) :
    wndWidth(width), wndHeight(height), Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    Target = glm::vec3(0.0f, 0.0f, 0.0f);
    Radius = 10.0f;
    UpdateCameraVectors();
}

OGLCamera::OGLCamera(I32 width, I32 height, F32 posX, F32 posY, F32 posZ, F32 upX, F32 upY, F32 upZ, F32 yaw, F32 pitch) :
    wndWidth(width), wndHeight(height), Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    Target = glm::vec3(0.0f, 0.0f, 0.0f);
    Radius = 10.0f;
    UpdateCameraVectors();
}

void OGLCamera::ResetPosition(glm::vec3 position) {
    Position = position;
    WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    Front = glm::vec3(0.0f, 0.0f, -1.0f);
    Yaw = YAW;
    Pitch = PITCH;
    Target = glm::vec3(0.0f, 0.0f, 0.0f);
    Radius = 10.0f;
    UpdateCameraVectors();
}

glm::vec3 OGLCamera::GetCameraPosition() {
    return this->Position;
}

glm::mat4 OGLCamera::GetViewMatrix() {
    // return glm::lookAt(Position, Position + Front, Up);
    return glm::lookAt(Position, Target, Up);
}

glm::mat4 OGLCamera::GetProjectionMatrix() {
    return glm::perspective(glm::radians(Zoom), (F32)this->wndWidth / (F32)this->wndHeight, 0.1f, 100.0f);
}

void OGLCamera::ProcessMouseMovement(glm::vec2 mouseDelta, GLboolean constrainPitch) {
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

void OGLCamera::ProcessMouseScroll(glm::vec2 mouseOffset) {
    Radius -= mouseOffset.y * MouseSensitivity;
    if (Radius < 2.0f)
        Radius = 2.0f;
    if (Radius > 20.0f)
        Radius = 20.0f;

    UpdateCameraVectors();
}

void OGLCamera::ProcessKeyboardMovement(glm::vec2 mouseDelta) {
    Target.x += mouseDelta.x * MovementSpeed;
    Target.y += mouseDelta.y * MovementSpeed;
    UpdateCameraVectors();
}

void OGLCamera::UpdateCameraVectors() {
    glm::vec3 offset;
    offset.x = Radius * cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    offset.y = Radius * sin(glm::radians(Pitch));
    offset.z = Radius * sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Position = Target + offset;

    Right = glm::normalize(glm::cross(Position - Target, WorldUp));
    Up = glm::normalize(glm::cross(Right, Position - Target));
}
}; // namespace JzRE