/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <array>
#include <bitset>
#include <string>
#include <unordered_map>
#include <vector>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"

namespace JzRE {

// ==================== Input Device Types ====================

/**
 * @brief Input device types for identification
 */
enum class JzEInputDeviceType : U8 {
    Keyboard,
    Mouse,
    Gamepad,
    Touch,
    Joystick
};

// ==================== Keyboard Input ====================

/**
 * @brief Keyboard key codes (GLFW compatible)
 */
enum class JzEKeyCode : I32 {
    Unknown       = -1,
    Space         = 32,
    Apostrophe    = 39,
    Comma         = 44,
    Minus         = 45,
    Period        = 46,
    Slash         = 47,
    Num0          = 48,
    Num1          = 49,
    Num2          = 50,
    Num3          = 51,
    Num4          = 52,
    Num5          = 53,
    Num6          = 54,
    Num7          = 55,
    Num8          = 56,
    Num9          = 57,
    Semicolon     = 59,
    Equal         = 61,
    A             = 65,
    B             = 66,
    C             = 67,
    D             = 68,
    E             = 69,
    F             = 70,
    G             = 71,
    H             = 72,
    I             = 73,
    J             = 74,
    K             = 75,
    L             = 76,
    M             = 77,
    N             = 78,
    O             = 79,
    P             = 80,
    Q             = 81,
    R             = 82,
    S             = 83,
    T             = 84,
    U             = 85,
    V             = 86,
    W             = 87,
    X             = 88,
    Y             = 89,
    Z             = 90,
    LeftBracket   = 91,
    Backslash     = 92,
    RightBracket  = 93,
    GraveAccent   = 96,
    Escape        = 256,
    Enter         = 257,
    Tab           = 258,
    Backspace     = 259,
    Insert        = 260,
    Delete        = 261,
    Right         = 262,
    Left          = 263,
    Down          = 264,
    Up            = 265,
    PageUp        = 266,
    PageDown      = 267,
    Home          = 268,
    End           = 269,
    CapsLock      = 280,
    ScrollLock    = 281,
    NumLock       = 282,
    PrintScreen   = 283,
    Pause         = 284,
    F1            = 290,
    F2            = 291,
    F3            = 292,
    F4            = 293,
    F5            = 294,
    F6            = 295,
    F7            = 296,
    F8            = 297,
    F9            = 298,
    F10           = 299,
    F11           = 300,
    F12           = 301,
    Keypad0       = 320,
    Keypad1       = 321,
    Keypad2       = 322,
    Keypad3       = 323,
    Keypad4       = 324,
    Keypad5       = 325,
    Keypad6       = 326,
    Keypad7       = 327,
    Keypad8       = 328,
    Keypad9       = 329,
    KeypadDecimal = 330,
    KeypadDivide  = 331,
    KeypadMultiply= 332,
    KeypadSubtract= 333,
    KeypadAdd     = 334,
    KeypadEnter   = 335,
    KeypadEqual   = 336,
    LeftShift     = 340,
    LeftControl   = 341,
    LeftAlt       = 342,
    LeftSuper     = 343,
    RightShift    = 344,
    RightControl  = 345,
    RightAlt      = 346,
    RightSuper    = 347,
    Menu          = 348
};

/**
 * @brief Mouse button codes
 */
enum class JzEMouseButton : I32 {
    Left   = 0,
    Right  = 1,
    Middle = 2,
    Button4 = 3,
    Button5 = 4,
    Button6 = 5,
    Button7 = 6,
    Button8 = 7
};

// ==================== Input State Component ====================

/**
 * @brief Comprehensive input state component for a window/entity.
 *
 * This component stores all input device states in a data-driven,
 * cache-friendly format suitable for ECS iteration.
 */
struct JzInputStateComponent {
    // ========== Keyboard State ==========
    struct KeyboardState {
        static constexpr Size KEY_COUNT = 512;

        std::bitset<KEY_COUNT> keysPressed;   ///< Keys currently held down
        std::bitset<KEY_COUNT> keysDown;      ///< Keys pressed this frame
        std::bitset<KEY_COUNT> keysUp;        ///< Keys released this frame
        std::bitset<KEY_COUNT> keysRepeating; ///< Keys repeating (held)

        String textBuffer;           ///< Text input buffer
        Bool   textInputEnabled{false}; ///< Whether text input mode is active

        Bool IsKeyPressed(I32 key) const { return key >= 0 && key < static_cast<I32>(KEY_COUNT) && keysPressed[static_cast<Size>(key)]; }
        Bool IsKeyDown(I32 key) const { return key >= 0 && key < static_cast<I32>(KEY_COUNT) && keysDown[static_cast<Size>(key)]; }
        Bool IsKeyUp(I32 key) const { return key >= 0 && key < static_cast<I32>(KEY_COUNT) && keysUp[static_cast<Size>(key)]; }
        Bool IsKeyRepeating(I32 key) const { return key >= 0 && key < static_cast<I32>(KEY_COUNT) && keysRepeating[static_cast<Size>(key)]; }

        Bool IsKeyPressed(JzEKeyCode key) const { return IsKeyPressed(static_cast<I32>(key)); }
        Bool IsKeyDown(JzEKeyCode key) const { return IsKeyDown(static_cast<I32>(key)); }
        Bool IsKeyUp(JzEKeyCode key) const { return IsKeyUp(static_cast<I32>(key)); }

        void ClearFrameState() {
            keysDown.reset();
            keysUp.reset();
            keysRepeating.reset();
            textBuffer.clear();
        }
    } keyboard;

    // ========== Mouse State ==========
    struct MouseState {
        static constexpr Size BUTTON_COUNT = 8;

        JzVec2 position{0.0f, 0.0f};      ///< Current mouse position
        JzVec2 positionDelta{0.0f, 0.0f}; ///< Mouse movement since last frame
        JzVec2 scrollDelta{0.0f, 0.0f};   ///< Scroll wheel delta
        JzVec2 lastPosition{0.0f, 0.0f};  ///< Previous frame position

        std::bitset<BUTTON_COUNT> buttonsPressed; ///< Buttons currently held
        std::bitset<BUTTON_COUNT> buttonsDown;    ///< Buttons pressed this frame
        std::bitset<BUTTON_COUNT> buttonsUp;      ///< Buttons released this frame

        /**
         * @brief Mouse cursor mode
         */
        enum class Mode : U8 {
            Normal,   ///< Free movement, cursor visible
            Captured, ///< Cursor locked to window center
            Hidden    ///< Cursor hidden but free movement
        } mode{Mode::Normal};

        Bool entered{false}; ///< Cursor entered window this frame

        Bool IsButtonPressed(I32 button) const { return button >= 0 && button < static_cast<I32>(BUTTON_COUNT) && buttonsPressed[static_cast<Size>(button)]; }
        Bool IsButtonDown(I32 button) const { return button >= 0 && button < static_cast<I32>(BUTTON_COUNT) && buttonsDown[static_cast<Size>(button)]; }
        Bool IsButtonUp(I32 button) const { return button >= 0 && button < static_cast<I32>(BUTTON_COUNT) && buttonsUp[static_cast<Size>(button)]; }

        Bool IsButtonPressed(JzEMouseButton button) const { return IsButtonPressed(static_cast<I32>(button)); }
        Bool IsButtonDown(JzEMouseButton button) const { return IsButtonDown(static_cast<I32>(button)); }
        Bool IsButtonUp(JzEMouseButton button) const { return IsButtonUp(static_cast<I32>(button)); }

        void ClearFrameState() {
            buttonsDown.reset();
            buttonsUp.reset();
            positionDelta = {0.0f, 0.0f};
            scrollDelta = {0.0f, 0.0f};
            entered = false;
        }
    } mouse;

    // ========== Gamepad State ==========
    struct GamepadState {
        static constexpr Size MAX_GAMEPADS = 4;
        static constexpr Size BUTTON_COUNT = 15;
        static constexpr Size AXIS_COUNT = 6;

        struct Pad {
            Bool   connected{false};
            String name;

            std::array<F32, BUTTON_COUNT>  buttons{};        ///< Analog button values (0-1)
            std::array<Bool, BUTTON_COUNT> buttonsPressed{}; ///< Buttons currently held
            std::array<Bool, BUTTON_COUNT> buttonsDown{};    ///< Buttons pressed this frame
            std::array<Bool, BUTTON_COUNT> buttonsUp{};      ///< Buttons released this frame

            std::array<JzVec2, 4> axes{}; ///< Left stick, right stick, triggers

            F32 leftMotor{0.0f};  ///< Low frequency vibration
            F32 rightMotor{0.0f}; ///< High frequency vibration
        };

        std::array<Pad, MAX_GAMEPADS> pads;

        void ClearFrameState() {
            for (auto &pad : pads) {
                std::fill(pad.buttonsDown.begin(), pad.buttonsDown.end(), false);
                std::fill(pad.buttonsUp.begin(), pad.buttonsUp.end(), false);
            }
        }
    } gamepad;

    // ========== Device Info ==========
    struct DeviceInfo {
        Bool               connected{false};
        String             name;
        JzEInputDeviceType type{JzEInputDeviceType::Keyboard};
        I32                id{-1};
    };

    std::vector<DeviceInfo> devices;

    // ========== Frame State Management ==========

    /**
     * @brief Clear per-frame state (call at end of frame)
     */
    void ClearFrameState() {
        keyboard.ClearFrameState();
        mouse.ClearFrameState();
        gamepad.ClearFrameState();
    }

    /**
     * @brief First frame flag for delta calculation
     */
    Bool firstFrame{true};
};

// ==================== Input Action Component ====================

/**
 * @brief Input action mapping component for high-level input abstraction.
 *
 * Maps raw input (keys, buttons, axes) to named actions for game logic.
 */
struct JzInputActionComponent {
    /**
     * @brief Input binding type
     */
    enum class BindingType : U8 {
        Key,
        MouseButton,
        MouseAxis,
        GamepadButton,
        GamepadAxis
    };

    /**
     * @brief Single input binding
     */
    struct Binding {
        BindingType type{BindingType::Key};

        // Key binding
        I32 key{0};
        I32 mods{0};

        // Mouse button/axis
        I32 mouseButton{0};
        I32 mouseAxis{0}; ///< 0=X, 1=Y, 2=ScrollX, 3=ScrollY

        // Gamepad
        I32 gamepadId{0};
        I32 gamepadButton{0};
        I32 gamepadAxis{0};

        F32  deadzone{0.1f};
        F32  sensitivity{1.0f};
        Bool invert{false};
    };

    /**
     * @brief Action definition
     */
    struct Action {
        String name;
        String displayName;

        std::vector<Binding> bindings;

        // Current state
        F32  value{0.0f};           ///< Current value (0-1 or -1 to 1)
        Bool pressed{false};        ///< Currently pressed
        Bool wasPressed{false};     ///< Pressed last frame
        F32  pressedDuration{0.0f}; ///< How long pressed

        // Frame events
        Bool triggered{false}; ///< Just pressed this frame
        Bool released{false};  ///< Just released this frame

        void Update(F32 deltaTime) {
            wasPressed = pressed;
            pressed    = (std::abs(value) > 0.1f);
            triggered  = pressed && !wasPressed;
            released   = !pressed && wasPressed;

            if (pressed) {
                pressedDuration += deltaTime;
            } else {
                pressedDuration = 0.0f;
            }
        }
    };

    std::unordered_map<String, Action> actions;

    /**
     * @brief Input context for switching control schemes
     */
    struct InputContext {
        String                          name;
        std::vector<String>             enabledActions;
        I32                             priority{0};
        Bool                            enabled{true};
    };

    std::vector<InputContext> contexts;
    String                    activeContext{"default"};

    // ========== Helper Methods ==========

    F32 GetActionValue(const String &name) const {
        auto it = actions.find(name);
        return it != actions.end() ? it->second.value : 0.0f;
    }

    Bool IsActionPressed(const String &name) const {
        auto it = actions.find(name);
        return it != actions.end() ? it->second.pressed : false;
    }

    Bool IsActionTriggered(const String &name) const {
        auto it = actions.find(name);
        return it != actions.end() ? it->second.triggered : false;
    }

    Bool IsActionReleased(const String &name) const {
        auto it = actions.find(name);
        return it != actions.end() ? it->second.released : false;
    }
};

// ==================== Camera Input Component (Enhanced) ====================

/**
 * @brief Camera-specific input control state.
 *
 * Processed input signals for camera control systems.
 */
struct JzCameraInputStateComponent {
    Bool   orbitActive{false};     ///< Orbit mode active (left mouse held)
    Bool   panActive{false};       ///< Pan mode active (right mouse held)
    Bool   zoomActive{false};      ///< Zoom mode active (middle mouse or scroll)
    JzVec2 mouseDelta{0.0f, 0.0f}; ///< Mouse movement delta
    F32    scrollDelta{0.0f};      ///< Scroll wheel delta for zoom
    Bool   resetRequested{false};  ///< Reset camera to default position

    // Movement input (from keyboard)
    JzVec3 movementInput{0.0f, 0.0f, 0.0f}; ///< WASD movement direction
    Bool   speedBoost{false};               ///< Shift key held for faster movement
};

// ==================== Legacy Input Components ====================
// Note: For comprehensive input state, use JzInputStateComponent and
// JzInputActionComponent above. These legacy components
// are maintained for backward compatibility and are auto-synced by JzInputSystem.

/**
 * @brief Component for storing mouse input state.
 *
 * @deprecated Consider using JzInputStateComponent.mouse for comprehensive
 * mouse state including buttons, position, delta, and scroll in a single component.
 *
 * This component is updated by JzInputSystem and read by other systems
 * (like CameraSystem) to process mouse input.
 */
struct JzMouseInputComponent {
    JzVec2 position{0.0f, 0.0f};      ///< Current mouse position
    JzVec2 positionDelta{0.0f, 0.0f}; ///< Mouse movement since last frame
    JzVec2 scroll{0.0f, 0.0f};        ///< Scroll wheel delta this frame

    Bool leftButtonDown{false};   ///< Left mouse button state
    Bool rightButtonDown{false};  ///< Right mouse button state
    Bool middleButtonDown{false}; ///< Middle mouse button state

    Bool leftButtonPressed{false};   ///< Left button pressed this frame
    Bool rightButtonPressed{false};  ///< Right button pressed this frame
    Bool middleButtonPressed{false}; ///< Middle button pressed this frame

    Bool leftButtonReleased{false};   ///< Left button released this frame
    Bool rightButtonReleased{false};  ///< Right button released this frame
    Bool middleButtonReleased{false}; ///< Middle button released this frame
};

/**
 * @brief Component for storing keyboard input state.
 *
 * This component is updated by JzInputSystem and provides
 * high-level key state queries.
 */
struct JzKeyboardInputComponent {
    // Common key states for quick access
    Bool w{false}, a{false}, s{false}, d{false};
    Bool space{false}, shift{false}, ctrl{false}, alt{false};
    Bool escape{false}, enter{false}, tab{false};

    // Arrow keys
    Bool up{false}, down{false}, left{false}, right{false};

    // Function keys
    Bool f1{false}, f2{false}, f3{false}, f4{false};
};

} // namespace JzRE
