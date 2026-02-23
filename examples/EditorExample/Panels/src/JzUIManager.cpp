/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/Editor/Panels/JzUIManager.h"

#include <array>
#include <filesystem>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_vulkan.h>

#include "JzRE/Editor/UI/JzImGuiTextureBridge.h"
#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/ECS/JzWindowSystem.h"
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"
#include "JzRE/Runtime/Platform/Vulkan/JzVulkanDevice.h"

JzRE::JzUIManager::JzUIManager(JzWindowSystem &windowSystem)
{
    ImGui::CreateContext();

    ImGuiIO &io                          = ImGui::GetIO();
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    SetDocking(false);

    auto *glfwWindow = static_cast<GLFWwindow *>(windowSystem.GetPlatformWindowHandle());
    if (!glfwWindow) {
        JzRE_LOG_ERROR("JzUIManager: invalid GLFW window handle");
        return;
    }

    Bool backendInitialized = false;

    if (JzServiceContainer::Has<JzDevice>()) {
        auto &device = JzServiceContainer::Get<JzDevice>();
        if (device.GetRHIType() == JzERHIType::Vulkan) {
            backendInitialized = InitializeVulkanBackend(glfwWindow);
        }
    }

    if (!backendInitialized) {
        backendInitialized = InitializeOpenGLBackend(glfwWindow);
    }

    if (!backendInitialized) {
        JzRE_LOG_ERROR("JzUIManager: failed to initialize ImGui backend");
    }

    ImGui::StyleColorsDark();
    ApplyTheme();

    JzImGuiTextureBridge::Initialize();
}

JzRE::JzUIManager::~JzUIManager()
{
    JzImGuiTextureBridge::Shutdown();
    ShutdownBackend();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void JzRE::JzUIManager::Render()
{
    if (!m_canvas) {
        return;
    }

    switch (m_backend) {
        case JzEImGuiBackend::OpenGL:
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            m_canvas->Draw();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            break;

        case JzEImGuiBackend::Vulkan: {
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            m_canvas->Draw();

            ImGui::Render();

            if (!JzServiceContainer::Has<JzDevice>()) {
                return;
            }

            auto &device = JzServiceContainer::Get<JzDevice>();
            auto *vkDevice = dynamic_cast<JzVulkanDevice *>(&device);
            if (!vkDevice || !vkDevice->IsFrameRecording()) {
                return;
            }

            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), vkDevice->GetCurrentCommandBuffer());
            break;
        }

        case JzEImGuiBackend::Unknown:
            break;
    }
}

void JzRE::JzUIManager::SetDocking(Bool value)
{
    m_dockingState = value;

    if (value) {
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    } else {
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_DockingEnable;
    }
}

JzRE::Bool JzRE::JzUIManager::IsDocking() const
{
    return m_dockingState;
}

void JzRE::JzUIManager::SetCanvas(JzCanvas &canvas)
{
    RemoveCanvas();

    m_canvas = &canvas;
}

void JzRE::JzUIManager::RemoveCanvas()
{
    m_canvas = nullptr;
}

void JzRE::JzUIManager::EnableEditorLayoutSave(JzRE::Bool value)
{
    if (value)
        ImGui::GetIO().IniFilename = m_layoutSaveFilename.c_str();
    else
        ImGui::GetIO().IniFilename = nullptr;
}

JzRE::Bool JzRE::JzUIManager::IsEditorLayoutSaveEnabled() const
{
    return ImGui::GetIO().IniFilename != nullptr;
}

void JzRE::JzUIManager::SetEditorLayoutSaveFilename(const JzRE::String &filename)
{
    m_layoutSaveFilename = filename;
    if (IsEditorLayoutSaveEnabled())
        ImGui::GetIO().IniFilename = m_layoutSaveFilename.c_str();
}

void JzRE::JzUIManager::ResetLayout(const JzRE::String &configPath) const
{
    ImGui::LoadIniSettingsFromDisk(configPath.c_str());
}

JzRE::Bool JzRE::JzUIManager::LoadFont(const JzRE::String &fontId, const JzRE::String &fontPath, JzRE::F32 fontSize)
{
    if (!std::filesystem::exists(fontPath)) {
        JzRE_LOG_WARN("JzUIManager: font file not found '{}'", fontPath);
        return false;
    }

    if (m_fonts.find(fontId) == m_fonts.end()) {
        auto   &io           = ImGui::GetIO();
        ImFont *fontInstance = io.Fonts->AddFontFromFileTTF(fontPath.c_str(),
                                                            fontSize,
                                                            nullptr,
                                                            io.Fonts->GetGlyphRangesChineseFull());
        if (fontInstance) {
            m_fonts[fontId] = fontInstance;
            return true;
        }
    }
    return false;
}

JzRE::Bool JzRE::JzUIManager::UnloadFont(const JzRE::String &fontId)
{
    if (m_fonts.find(fontId) != m_fonts.end()) {
        m_fonts.erase(fontId);
        return true;
    }
    return false;
}

JzRE::Bool JzRE::JzUIManager::UseFont(const JzRE::String &fontId)
{
    auto foundFont = m_fonts.find(fontId);
    if (foundFont != m_fonts.end()) {
        ImGui::GetIO().FontDefault = foundFont->second;
        return true;
    }
    return false;
}

void JzRE::JzUIManager::UseDefaultFont()
{
    ImGui::GetIO().FontDefault = nullptr;
}

void JzRE::JzUIManager::ApplyTheme()
{
    ImGuiStyle &style = ImGui::GetStyle();

    // Gray theme with good contrast
    // Text
    style.Colors[ImGuiCol_Text]         = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

    // Backgrounds
    style.Colors[ImGuiCol_WindowBg]  = ImVec4(0.18f, 0.18f, 0.18f, 0.95f);
    style.Colors[ImGuiCol_ChildBg]   = ImVec4(0.22f, 0.22f, 0.22f, 0.95f);
    style.Colors[ImGuiCol_PopupBg]   = ImVec4(0.25f, 0.25f, 0.25f, 0.95f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);

    // Borders
    style.Colors[ImGuiCol_Border]       = ImVec4(0.35f, 0.35f, 0.35f, 0.50f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Title bar
    style.Colors[ImGuiCol_TitleBg]          = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive]    = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);

    // Scrollbar
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);

    // Frame / Input
    style.Colors[ImGuiCol_FrameBg]        = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive]  = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);

    // Checkbox / Slider
    style.Colors[ImGuiCol_CheckMark]        = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]       = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);

    // Button
    style.Colors[ImGuiCol_Button]        = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]  = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);

    // Header (CollapsingHeader, TreeNode, etc.)
    style.Colors[ImGuiCol_Header]        = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive]  = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

    // Separator
    style.Colors[ImGuiCol_Separator]        = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
    style.Colors[ImGuiCol_SeparatorActive]  = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);

    // Resize grip
    style.Colors[ImGuiCol_ResizeGrip]        = ImVec4(0.35f, 0.35f, 0.35f, 0.20f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.45f, 0.45f, 0.45f, 0.40f);
    style.Colors[ImGuiCol_ResizeGripActive]  = ImVec4(0.55f, 0.55f, 0.55f, 0.50f);

    // Tabs
    style.Colors[ImGuiCol_Tab]                 = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_TabHovered]          = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_TabActive]           = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_TabSelected]         = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused]        = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive]  = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);

    // Docking
    style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.35f, 0.35f, 0.35f, 0.50f);

    // Selection & Modal
    style.Colors[ImGuiCol_TextSelectedBg]   = ImVec4(0.40f, 0.40f, 0.40f, 0.50f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.60f);

    // Style adjustments
    style.WindowMenuButtonPosition = ImGuiDir_Right;
    style.WindowRounding           = 0.0f;
    style.FrameRounding            = 0.0f;
    style.ScrollbarRounding        = 0.0f;
    style.GrabRounding             = 0.0f;
    style.TabRounding              = 0.0f;
}

JzRE::Bool JzRE::JzUIManager::InitializeOpenGLBackend(void *glfwWindow)
{
    if (ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow *>(glfwWindow), true) == 0) {
        return false;
    }

    if (ImGui_ImplOpenGL3_Init("#version 150") == 0) {
        ImGui_ImplGlfw_Shutdown();
        return false;
    }

    m_backend = JzEImGuiBackend::OpenGL;
    JzRE_LOG_INFO("JzUIManager: initialized ImGui OpenGL backend");
    return true;
}

JzRE::Bool JzRE::JzUIManager::InitializeVulkanBackend(void *glfwWindow)
{
    if (!JzServiceContainer::Has<JzDevice>()) {
        return false;
    }

    auto &device = JzServiceContainer::Get<JzDevice>();
    auto *vkDevice = dynamic_cast<JzVulkanDevice *>(&device);
    if (!vkDevice || !vkDevice->IsInitialized()) {
        return false;
    }

    if (ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow *>(glfwWindow), true) == 0) {
        return false;
    }

    std::array<VkDescriptorPoolSize, 11> poolSizes = {
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000},
    };

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.maxSets       = static_cast<U32>(poolSizes.size()) * 1000;
    poolInfo.poolSizeCount = static_cast<U32>(poolSizes.size());
    poolInfo.pPoolSizes    = poolSizes.data();

    if (vkCreateDescriptorPool(vkDevice->GetVkDevice(), &poolInfo, nullptr, &m_vulkanDescriptorPool) != VK_SUCCESS) {
        ImGui_ImplGlfw_Shutdown();
        return false;
    }

    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.ApiVersion      = VK_API_VERSION_1_0;
    initInfo.Instance        = vkDevice->GetVkInstance();
    initInfo.PhysicalDevice  = vkDevice->GetVkPhysicalDevice();
    initInfo.Device          = vkDevice->GetVkDevice();
    initInfo.QueueFamily     = vkDevice->GetGraphicsQueueFamilyIndex();
    initInfo.Queue           = vkDevice->GetGraphicsQueue();
    initInfo.PipelineCache   = VK_NULL_HANDLE;
    initInfo.DescriptorPool  = m_vulkanDescriptorPool;
    initInfo.RenderPass      = vkDevice->GetSwapchainRenderPass();
    initInfo.Subpass         = 0;
    initInfo.MinImageCount   = 2;
    initInfo.ImageCount      = 2;
    initInfo.MSAASamples     = VK_SAMPLE_COUNT_1_BIT;
    initInfo.Allocator       = nullptr;
    initInfo.CheckVkResultFn = nullptr;

#ifdef IMGUI_IMPL_VULKAN_HAS_DYNAMIC_RENDERING
    initInfo.UseDynamicRendering = false;
#endif

    if (ImGui_ImplVulkan_Init(&initInfo) == 0) {
        vkDestroyDescriptorPool(vkDevice->GetVkDevice(), m_vulkanDescriptorPool, nullptr);
        m_vulkanDescriptorPool = VK_NULL_HANDLE;
        ImGui_ImplGlfw_Shutdown();
        return false;
    }

    if (!ImGui_ImplVulkan_CreateFontsTexture()) {
        JzRE_LOG_WARN("JzUIManager: failed to upload ImGui font texture for Vulkan backend");
    }

    m_backend = JzEImGuiBackend::Vulkan;
    JzRE_LOG_INFO("JzUIManager: initialized ImGui Vulkan backend");
    return true;
}

void JzRE::JzUIManager::ShutdownBackend()
{
    switch (m_backend) {
        case JzEImGuiBackend::OpenGL:
            ImGui_ImplOpenGL3_Shutdown();
            break;

        case JzEImGuiBackend::Vulkan:
            if (JzServiceContainer::Has<JzDevice>()) {
                auto &device = JzServiceContainer::Get<JzDevice>();
                if (auto *vkDevice = dynamic_cast<JzVulkanDevice *>(&device)) {
                    vkDevice->Finish();
                    ImGui_ImplVulkan_Shutdown();

                    if (m_vulkanDescriptorPool != VK_NULL_HANDLE) {
                        vkDestroyDescriptorPool(vkDevice->GetVkDevice(), m_vulkanDescriptorPool, nullptr);
                        m_vulkanDescriptorPool = VK_NULL_HANDLE;
                    }
                }
            }
            break;

        case JzEImGuiBackend::Unknown:
            break;
    }

    m_backend = JzEImGuiBackend::Unknown;
}
