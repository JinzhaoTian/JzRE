/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include <cmath>
#include <imgui.h>

#include "JzRE/Editor/Panels/JzAssetView.h"
#include "JzRE/Runtime/Core/JzFileSystemUtils.h"
#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/ECS/JzAssetComponents.h"
#include "JzRE/Runtime/Function/ECS/JzAssetSystem.h"
#include "JzRE/Runtime/Function/ECS/JzRenderSystem.h"
#include "JzRE/Runtime/Function/ECS/JzCameraComponents.h"
#include "JzRE/Runtime/Function/ECS/JzInputComponents.h"
#include "JzRE/Runtime/Function/ECS/JzRenderComponents.h"
#include "JzRE/Runtime/Function/ECS/JzWindowComponents.h"
#include "JzRE/Runtime/Function/ECS/JzTransformComponents.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"
#include "JzRE/Runtime/Function/Project/JzProjectManager.h"
#include "JzRE/Runtime/Resource/JzAssetManager.h"
#include "JzRE/Runtime/Resource/JzModel.h"
#include "JzRE/Runtime/Resource/JzTexture.h"

JzRE::JzAssetView::JzAssetView(const JzRE::String &name, JzRE::Bool is_opened) :
    JzRE::JzView(name, is_opened) { }

// ==================== Public API ====================

void JzRE::JzAssetView::PreviewAsset(const std::filesystem::path &path)
{
    if (path == m_currentPath) {
        return;
    }

    ClearPreview();
    m_currentPath = path;

    auto fileType = JzFileSystemUtils::GetFileType(path.string());

    switch (fileType) {
    case JzEFileType::TEXTURE:
        PreviewTexture(path);
        break;
    case JzEFileType::MODEL:
        PreviewModel(path);
        break;
    default:
        JzRE_LOG_WARN("No preview available for: {}", path.filename().string());
        break;
    }
}

void JzRE::JzAssetView::ClearPreview()
{
    CleanupPreviewEntities();
    m_previewTexture.reset();
    m_previewMode = JzEPreviewMode::None;
    m_currentPath.clear();
}

// ==================== Preview Implementations ====================

void JzRE::JzAssetView::PreviewTexture(const std::filesystem::path &path)
{
    if (!JzServiceContainer::Has<JzAssetManager>() || !JzServiceContainer::Has<JzProjectManager>()) {
        return;
    }

    auto &assetManager   = JzServiceContainer::Get<JzAssetManager>();
    auto &projectManager = JzServiceContainer::Get<JzProjectManager>();

    // Compute relative path from content directory
    auto        contentPath  = projectManager.GetContentPath();
    auto        relativePath = std::filesystem::relative(path, contentPath);
    std::string relativeStr  = relativePath.string();

    auto handle  = assetManager.GetOrLoad<JzTexture>(relativeStr);
    auto texture = assetManager.GetShared(handle);
    if (texture) {
        m_previewTexture = texture->GetRhiTexture();
        m_previewMode    = JzEPreviewMode::Texture;
        JzRE_LOG_INFO("Previewing texture: {}", path.filename().string());
    } else {
        JzRE_LOG_ERROR("Failed to load texture for preview: {}", path.string());
    }
}

void JzRE::JzAssetView::PreviewModel(const std::filesystem::path &path)
{
    if (!JzServiceContainer::Has<JzAssetSystem>() || !JzServiceContainer::Has<JzWorld>() ||
        !JzServiceContainer::Has<JzProjectManager>()) {
        return;
    }

    auto &assetSystem    = JzServiceContainer::Get<JzAssetSystem>();
    auto &world          = JzServiceContainer::Get<JzWorld>();
    auto &projectManager = JzServiceContainer::Get<JzProjectManager>();

    // Compute relative path from content directory
    auto        contentPath  = projectManager.GetContentPath();
    auto        relativePath = std::filesystem::relative(path, contentPath);
    std::string relativeStr  = relativePath.string();

    auto modelHandle = assetSystem.LoadSync<JzModel>(relativeStr);
    if (!modelHandle.IsValid()) {
        JzRE_LOG_ERROR("Failed to load model for preview: {}", path.string());
        return;
    }

    m_previewEntities = assetSystem.SpawnModel(world, modelHandle);

    // Tag all preview entities with JzPreviewOnlyTag and ensure they have
    // material components (some .obj files have no .mtl, so SpawnModel
    // skips material creation — but the render system requires it).
    for (auto entity : m_previewEntities) {
        if (!world.HasComponent<JzPreviewOnlyTag>(entity)) {
            world.AddComponent<JzPreviewOnlyTag>(entity);
        }
        if (!world.HasComponent<JzMaterialAssetComponent>(entity)) {
            auto &matComp   = world.AddComponent<JzMaterialAssetComponent>(entity);
            matComp.isReady = true;
        }
    }

    // Create preview camera if needed
    CreatePreviewCamera();

    // Reset orbit to defaults
    m_orbitYaw      = 0.5f;
    m_orbitPitch    = 0.3f;
    m_orbitDistance = 5.0f;
    m_orbitTarget   = JzVec3(0.0f, 0.0f, 0.0f);
    UpdateCameraFromOrbit();

    m_previewMode = JzEPreviewMode::Model;
    JzRE_LOG_INFO("Previewing model: {}", path.filename().string());
}

void JzRE::JzAssetView::CleanupPreviewEntities()
{
    if (m_previewEntities.empty()) {
        return;
    }

    if (!JzServiceContainer::Has<JzAssetSystem>() || !JzServiceContainer::Has<JzWorld>()) {
        m_previewEntities.clear();
        return;
    }

    auto &assetSystem = JzServiceContainer::Get<JzAssetSystem>();
    auto &world       = JzServiceContainer::Get<JzWorld>();

    for (auto entity : m_previewEntities) {
        if (world.IsValid(entity)) {
            assetSystem.DetachAllAssets(world, entity);
            world.DestroyEntity(entity);
        }
    }
    m_previewEntities.clear();
}

void JzRE::JzAssetView::CreatePreviewCamera()
{
    if (m_previewCamera != INVALID_ENTITY) {
        return;
    }

    if (!JzServiceContainer::Has<JzWorld>()) {
        return;
    }

    auto &world    = JzServiceContainer::Get<JzWorld>();
    m_previewCamera = world.CreateEntity();

    auto &camera       = world.AddComponent<JzCameraComponent>(m_previewCamera);
    camera.isMainCamera = false;
    camera.fov         = 60.0f;
    camera.nearPlane   = 0.1f;
    camera.farPlane    = 100.0f;
    camera.clearColor  = JzVec3(0.15f, 0.15f, 0.15f);

    world.AddComponent<JzTransformComponent>(m_previewCamera);

    // Update the render view with the new camera — the view was registered
    // in JzView's constructor with INVALID_ENTITY before the camera existed.
    if (m_viewHandle != JzRenderSystem::INVALID_VIEW_HANDLE &&
        JzServiceContainer::Has<JzRenderSystem>()) {
        auto &renderSystem = JzServiceContainer::Get<JzRenderSystem>();
        renderSystem.UpdateViewCamera(m_viewHandle, m_previewCamera);
    }

    UpdateCameraFromOrbit();
}

// ==================== Drawing ====================

void JzRE::JzAssetView::_Draw_Impl()
{
    if (m_previewMode == JzEPreviewMode::Texture && m_previewTexture) {
        // Bypass the render pipeline — show the texture directly via JzFrame
        auto size = GetSafeSize();
        if (size.x > 0 && size.y > 0) {
            m_frame->frameTextureId = m_previewTexture->GetTextureID();
            m_frame->frameSize      = JzVec2(static_cast<F32>(size.x), static_cast<F32>(size.y));
        }
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        JzPanelWindow::_Draw_Impl();
        ImGui::PopStyleVar();
    } else {
        // Model or None: use the normal render pipeline
        JzView::_Draw_Impl();
    }
}

JzRE::JzEntity JzRE::JzAssetView::GetCameraEntity()
{
    return m_previewCamera;
}

// ==================== Update (Orbit Camera Input) ====================

void JzRE::JzAssetView::Update(JzRE::F32 deltaTime)
{
    JzView::Update(deltaTime);

    if (m_previewMode != JzEPreviewMode::Model) {
        return;
    }

    if (!IsHovered() && !IsFocused()) {
        m_firstMouse = true;
        return;
    }

    if (!JzServiceContainer::Has<JzWorld>()) {
        return;
    }

    // Get primary window input state from ECS
    auto &world = JzServiceContainer::Get<JzWorld>();

    JzInputStateComponent *inputState = nullptr;
    auto                   inputView  = world.View<JzInputStateComponent, JzPrimaryWindowTag>();
    for (auto entity : inputView) {
        inputState = &world.GetComponent<JzInputStateComponent>(entity);
        break;
    }
    if (!inputState) return;

    const auto &input = *inputState;

    // Get current mouse position
    JzVec2 currentMousePos = input.mouse.position;

    // Calculate mouse delta
    F32 deltaX = 0.0f;
    F32 deltaY = 0.0f;
    if (!m_firstMouse) {
        deltaX = currentMousePos.x - m_lastMousePos.x;
        deltaY = currentMousePos.y - m_lastMousePos.y;
    }

    // Track button states
    Bool leftPressed  = input.mouse.IsButtonPressed(JzEMouseButton::Left);
    Bool rightPressed = input.mouse.IsButtonPressed(JzEMouseButton::Right);

    // Left mouse button — Orbit rotation
    if (leftPressed) {
        if (!m_leftMousePressed) {
            m_leftMousePressed = true;
            m_firstMouse       = true;
        } else if (!m_firstMouse) {
            HandleOrbitRotation(deltaX, deltaY);
        }
    } else {
        m_leftMousePressed = false;
    }

    // Right mouse button — Panning
    if (rightPressed) {
        if (!m_rightMousePressed) {
            m_rightMousePressed = true;
            m_firstMouse        = true;
        } else if (!m_firstMouse) {
            HandlePanning(deltaX, deltaY);
        }
    } else {
        m_rightMousePressed = false;
    }

    // Scroll wheel — Zoom
    JzVec2 scroll = input.mouse.scrollDelta;
    if (std::abs(scroll.y) > 0.001f) {
        HandleZoom(scroll.y);
    }

    // Update last mouse position
    m_lastMousePos = currentMousePos;
    m_firstMouse   = false;
}

// ==================== Orbit Camera Methods ====================

void JzRE::JzAssetView::HandleOrbitRotation(JzRE::F32 deltaX, JzRE::F32 deltaY)
{
    m_orbitYaw   -= deltaX * m_orbitSensitivity;
    m_orbitPitch -= deltaY * m_orbitSensitivity;

    // Clamp pitch to avoid gimbal lock
    constexpr F32 maxPitch = 1.55f; // ~89 degrees
    m_orbitPitch           = std::clamp(m_orbitPitch, -maxPitch, maxPitch);

    UpdateCameraFromOrbit();
}

void JzRE::JzAssetView::HandlePanning(JzRE::F32 deltaX, JzRE::F32 deltaY)
{
    F32 cosYaw   = std::cos(m_orbitYaw);
    F32 sinYaw   = std::sin(m_orbitYaw);
    F32 cosPitch = std::cos(m_orbitPitch);
    F32 sinPitch = std::sin(m_orbitPitch);

    JzVec3 right(cosYaw, 0.0f, sinYaw);
    JzVec3 up(-sinYaw * sinPitch, cosPitch, cosYaw * sinPitch);

    F32 panScale = m_orbitDistance * m_panSensitivity;

    m_orbitTarget.x -= right.x * deltaX * panScale + up.x * deltaY * panScale;
    m_orbitTarget.y += up.y * deltaY * panScale;
    m_orbitTarget.z -= right.z * deltaX * panScale + up.z * deltaY * panScale;

    UpdateCameraFromOrbit();
}

void JzRE::JzAssetView::HandleZoom(JzRE::F32 scrollY)
{
    m_orbitDistance -= scrollY * m_zoomSensitivity;
    m_orbitDistance  = std::clamp(m_orbitDistance, m_minDistance, m_maxDistance);

    UpdateCameraFromOrbit();
}

void JzRE::JzAssetView::UpdateCameraFromOrbit()
{
    if (m_previewCamera == INVALID_ENTITY) {
        return;
    }

    if (!JzServiceContainer::Has<JzWorld>()) {
        return;
    }

    auto &world = JzServiceContainer::Get<JzWorld>();
    if (!world.IsValid(m_previewCamera)) {
        return;
    }

    auto &camera = world.GetComponent<JzCameraComponent>(m_previewCamera);

    F32 cosPitch = std::cos(m_orbitPitch);
    F32 sinPitch = std::sin(m_orbitPitch);
    F32 cosYaw   = std::cos(m_orbitYaw);
    F32 sinYaw   = std::sin(m_orbitYaw);

    camera.position.x = m_orbitTarget.x + m_orbitDistance * cosPitch * sinYaw;
    camera.position.y = m_orbitTarget.y + m_orbitDistance * sinPitch;
    camera.position.z = m_orbitTarget.z + m_orbitDistance * cosPitch * cosYaw;
    camera.rotation.x = -m_orbitPitch;
    camera.rotation.y = m_orbitYaw;
    camera.rotation.z = 0.0f;
    camera.rotation.w = 0.0f;
}
