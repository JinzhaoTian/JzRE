/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <vector>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"

namespace JzRE {

// ==================== Window State Enum ====================

/**
 * @brief Window state enumeration
 */
enum class JzEWindowState : U8 {
    Normal,
    Minimized,
    Maximized,
    Fullscreen,
    Hidden
};

// ==================== Window Component ====================

/**
 * @brief Window state component for ECS.
 *
 * Stores comprehensive window state in a data-driven format.
 * Updated by JzWindowSystem from the backend (GLFW).
 */
struct JzWindowStateComponent {
    // ========== Basic Properties ==========
    String  title;
    JzIVec2 position{0, 0};
    JzIVec2 size{1280, 720};
    JzIVec2 framebufferSize{1280, 720}; ///< Actual pixel size (for HiDPI)
    JzVec2  contentScale{1.0f, 1.0f};   ///< DPI scale factor

    // ========== Window State ==========
    JzEWindowState state{JzEWindowState::Normal};

    // ========== Window Attributes ==========
    Bool visible{true};
    Bool focused{false};
    Bool resizable{true};
    Bool decorated{true};
    Bool floating{false}; ///< Always on top
    Bool transparentFramebuffer{false};

    // ========== Size Limits ==========
    JzIVec2 minimumSize{0, 0};
    JzIVec2 maximumSize{0, 0}; ///< 0 = no limit

    // ========== Rendering Properties ==========
    JzVec4 clearColor{0.1f, 0.1f, 0.1f, 1.0f};
    I32    swapInterval{1}; ///< V-Sync: 0=off, 1=on, 2=adaptive

    // ========== Native Handles ==========
    void *nativeHandle{nullptr}; ///< Platform-specific window handle
    void *glContext{nullptr};    ///< OpenGL context (if applicable)

    // ========== Statistics ==========
    struct Stats {
        F64  averageFPS{0.0};
        F64  frameTime{0.0};
        U64  frameCount{0};
        Size memoryUsage{0};
    } stats;

    // ========== Internal State Flags ==========
    Bool shouldClose{false};
    Bool sizeDirty{false};  ///< Size changed, needs backend update
    Bool stateDirty{false}; ///< State changed, needs backend update

    // ========== Saved Windowed State (for fullscreen toggle) ==========
    JzIVec2 windowedSize{1280, 720};
    JzIVec2 windowedPosition{0, 0};

    // ========== Helper Methods ==========

    F32 GetAspectRatio() const
    {
        return size.y > 0 ? static_cast<F32>(size.x) / static_cast<F32>(size.y) : 1.0f;
    }

    Bool IsFullscreen() const
    {
        return state == JzEWindowState::Fullscreen;
    }
    Bool IsMinimized() const
    {
        return state == JzEWindowState::Minimized;
    }
    Bool IsMaximized() const
    {
        return state == JzEWindowState::Maximized;
    }
    Bool IsHidden() const
    {
        return state == JzEWindowState::Hidden;
    }
};

// ==================== Display/Monitor Component ====================

/**
 * @brief Display/monitor information component.
 *
 * Represents a physical display device.
 */
struct JzDisplayComponent {
    String  name;
    JzIVec2 position{0, 0};     ///< Virtual position
    JzIVec2 physicalSize{0, 0}; ///< Physical size in mm
    JzIVec2 resolution{1920, 1080};
    JzVec2  scale{1.0f, 1.0f}; ///< DPI scale
    F32     refreshRate{60.0f};
    I32     redBits{8}, greenBits{8}, blueBits{8};

    /**
     * @brief Video mode descriptor
     */
    struct VideoMode {
        JzIVec2 resolution;
        I32     refreshRate;
        I32     redBits, greenBits, blueBits;
    };

    std::vector<VideoMode> videoModes;
    Size                   currentVideoMode{0};

    Bool primary{false};
    Bool connected{true};
};

// ==================== Window Event Types ====================

/**
 * @brief Window event types
 */
enum class JzEWindowEventType : U8 {
    Resized,
    Moved,
    FocusGained,
    FocusLost,
    Iconified,
    Restored,
    Maximized,
    Closed,
    FileDropped,
    RefreshRequested,
    FramebufferResized,
    ContentScaleChanged
};

/**
 * @brief Window event data
 */
struct JzWindowEvent {
    JzEWindowEventType type;

    // Event-specific data
    union {
        struct {
            I32 width;
            I32 height;
        } resized;
        struct {
            I32 x;
            I32 y;
        } moved;
        struct {
            Bool focused;
        } focus;
        struct {
            F32 xScale;
            F32 yScale;
        } contentScale;
    } data;

    // File drop paths (separate due to non-trivial type)
    std::vector<String> droppedPaths;
    JzVec2              dropPosition{0.0f, 0.0f};
};

/**
 * @brief Window event queue component.
 *
 * Stores pending window events for processing by systems.
 */
struct JzWindowEventQueueComponent {
    std::vector<JzWindowEvent> events;

    void Push(JzWindowEvent event)
    {
        events.push_back(std::move(event));
    }

    void Clear()
    {
        events.clear();
    }

    Bool HasEvents() const
    {
        return !events.empty();
    }
};

// ==================== Window Tags ====================

/**
 * @brief Tag for the primary/main window
 */
struct JzPrimaryWindowTag { };

/**
 * @brief Tag for window that should not be closed by user
 */
struct JzPersistentWindowTag { };

} // namespace JzRE
