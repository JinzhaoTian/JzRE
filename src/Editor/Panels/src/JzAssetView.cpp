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
    if (!JzServiceContainer::Has<JzAssetSystem>() || !JzServiceContainer::Has<JzWorld>() || !JzServiceContainer::Has<JzProjectManager>()) {
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

    // Ensure view is registered before binding camera (RenderSystem may initialize later).
    if (IsOpened() && m_viewHandle == JzRenderSystem::INVALID_VIEW_HANDLE && JzServiceContainer::Has<JzRenderSystem>()) {
        RegisterRenderTarget();
    }

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

    auto &world     = JzServiceContainer::Get<JzWorld>();
    m_previewCamera = world.CreateEntity();

    auto &camera        = world.AddComponent<JzCameraComponent>(m_previewCamera);
    camera.isMainCamera = false;
    camera.fov          = 60.0f;
    camera.nearPlane    = 0.1f;
    camera.farPlane     = 100.0f;
    camera.clearColor   = JzVec3(0.15f, 0.15f, 0.15f);
    auto viewSize       = GetSafeSize();
    if (viewSize.x > 0 && viewSize.y > 0) {
        camera.aspect  = static_cast<F32>(viewSize.x) / static_cast<F32>(viewSize.y);
        m_lastViewSize = viewSize;
    }

    auto &transform    = world.AddComponent<JzTransformComponent>(m_previewCamera);
    transform.position = JzVec3(0.0f, 0.0f, 5.0f);
    transform.rotation = JzVec3(0.0f, 0.0f, 0.0f);
    transform.scale    = JzVec3(1.0f, 1.0f, 1.0f);

    // Add orbit controller - JzCameraSystem will handle orbit logic
    auto &orbit            = world.AddComponent<JzOrbitControllerComponent>(m_previewCamera);
    orbit.target           = JzVec3(0.0f, 0.0f, 0.0f);
    orbit.yaw              = 0.5f;
    orbit.pitch            = 0.3f;
    orbit.distance         = 5.0f;
    orbit.orbitSensitivity = m_orbitSensitivity;
    orbit.panSensitivity   = m_panSensitivity;
    orbit.zoomSensitivity  = m_zoomSensitivity;
    orbit.minDistance      = m_minDistance;
    orbit.maxDistance      = m_maxDistance;

    // Add camera input component - JzCameraSystem reads this for orbit control
    world.AddComponent<JzCameraInputComponent>(m_previewCamera);

    // Ensure the render target exists before updating the view's camera.
    if (IsOpened() && m_viewHandle == JzRenderSystem::INVALID_VIEW_HANDLE && JzServiceContainer::Has<JzRenderSystem>()) {
        RegisterRenderTarget();
    }

    // Update the render view with the new camera
    if (m_viewHandle != JzRenderSystem::INVALID_VIEW_HANDLE && JzServiceContainer::Has<JzRenderSystem>()) {
        auto &renderSystem = JzServiceContainer::Get<JzRenderSystem>();
        renderSystem.UpdateViewCamera(m_viewHandle, m_previewCamera);
    }
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

// ==================== Update (Capture Input for JzCameraSystem) ====================

void JzRE::JzAssetView::Update(JzRE::F32 deltaTime)
{
    JzView::Update(deltaTime);

    // Ensure the render target is registered when RenderSystem becomes available.
    if (IsOpened() && m_viewHandle == JzRenderSystem::INVALID_VIEW_HANDLE && JzServiceContainer::Has<JzRenderSystem>()) {
        RegisterRenderTarget();
    }

    if (m_viewHandle != JzRenderSystem::INVALID_VIEW_HANDLE && JzServiceContainer::Has<JzRenderSystem>() && m_previewCamera != INVALID_ENTITY) {
        auto &renderSystem = JzServiceContainer::Get<JzRenderSystem>();
        renderSystem.UpdateViewCamera(m_viewHandle, m_previewCamera);
    }

    if (m_previewCamera != INVALID_ENTITY && JzServiceContainer::Has<JzWorld>()) {
        auto viewSize = GetSafeSize();
        if (viewSize.x > 0 && viewSize.y > 0) {
            auto &world = JzServiceContainer::Get<JzWorld>();
            if (world.IsValid(m_previewCamera) && world.HasComponent<JzCameraComponent>(m_previewCamera)) {
                if (viewSize.x != m_lastViewSize.x || viewSize.y != m_lastViewSize.y) {
                    m_lastViewSize = viewSize;
                }
                auto &camera = world.GetComponent<JzCameraComponent>(m_previewCamera);
                camera.aspect =
                    static_cast<F32>(viewSize.x) / static_cast<F32>(viewSize.y);
            }
        }
    }

    if (m_previewMode != JzEPreviewMode::Model) {
        return;
    }

    if (!JzServiceContainer::Has<JzWorld>()) {
        return;
    }

    auto &world = JzServiceContainer::Get<JzWorld>();

    // Ensure preview camera has input component
    if (m_previewCamera == INVALID_ENTITY || !world.IsValid(m_previewCamera)) {
        return;
    }

    auto *cameraInput = world.TryGetComponent<JzCameraInputComponent>(m_previewCamera);
    if (!cameraInput) {
        return;
    }

    // Reset input state each frame
    cameraInput->orbitActive    = false;
    cameraInput->panActive      = false;
    cameraInput->mouseDelta     = JzVec2(0.0f, 0.0f);
    cameraInput->scrollDelta    = 0.0f;
    cameraInput->resetRequested = false;

    // Only process input when the panel is hovered or focused
    if (!IsHovered() && !IsFocused()) {
        m_firstMouse = true;
        return;
    }

    // Get primary window input state from ECS
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

    // Track button states and write to camera input component
    Bool leftPressed  = input.mouse.IsButtonPressed(JzEMouseButton::Left);
    Bool rightPressed = input.mouse.IsButtonPressed(JzEMouseButton::Right);

    // Left mouse button — Orbit rotation
    if (leftPressed) {
        if (!m_leftMousePressed) {
            m_leftMousePressed = true;
            m_firstMouse       = true;
        } else if (!m_firstMouse) {
            cameraInput->orbitActive = true;
            cameraInput->mouseDelta  = JzVec2(deltaX, deltaY);
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
            cameraInput->panActive  = true;
            cameraInput->mouseDelta = JzVec2(deltaX, deltaY);
        }
    } else {
        m_rightMousePressed = false;
    }

    // Scroll wheel — Zoom
    JzVec2 scroll = input.mouse.scrollDelta;
    if (std::abs(scroll.y) > 0.001f) {
        cameraInput->scrollDelta = scroll.y;
    }

    // Update last mouse position
    m_lastMousePos = currentMousePos;
    m_firstMouse   = false;
}
