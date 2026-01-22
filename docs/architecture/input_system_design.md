# Input System Design Document

## Overview

This document describes the design and implementation of the Input System in JzRE's ECS architecture.

## Design Decision: Should Input be a System?

### Answer: YES (with a hybrid approach)

JzRE implements a **two-layer input architecture** that provides the benefits of both approaches:

```
Layer 1: Platform Layer
  └── JzInputManager (Service)
        ↓
Layer 2: Function Layer (ECS)
  └── JzInputSystem (System in Input Phase)
        ↓
        Updates Input Components
        ↓
  Other Systems read Input Components
```

## Architecture Benefits

### 1. Separation of Concerns

- **JzInputManager**: Hardware abstraction, event handling, platform-specific code
- **JzInputSystem**: Game logic, input processing, component updates

### 2. ECS Purity

- All game logic flows through ECS components
- Systems don't directly depend on external services
- Clear data flow: Input Manager → Input System → Components → Game Systems

### 3. Testability

- Mock input components for system testing
- No need to mock entire input manager
- Deterministic input replay via component data

### 4. Performance

- Input processing runs in dedicated Input phase (first)
- Ensures fresh input data available to all other systems
- Avoids redundant input queries

### 5. Extensibility

- Easy to add input actions, contexts, dead zones
- Can implement input buffering/replay
- Support multiple input sources (player 1, player 2, AI, replay, etc.)

## Implementation Details

### Phase Execution Order

```
1. Input Phase       → JzInputSystem
2. Physics Phase     → (user systems)
3. Animation Phase   → (user systems)
4. Logic Phase       → (user systems)
5. PreRender Phase   → JzCameraSystem, JzLightSystem
6. Culling Phase     → (user systems)
7. RenderPrep Phase  → (user systems)
8. Render Phase      → JzRenderSystem
```

### Input Components

#### JzMouseInputComponent

Stores complete mouse state updated each frame:

- Current position and delta
- Button states (down/up)
- Press/release events (this frame only)
- Scroll delta

#### JzKeyboardInputComponent

Common key states for quick access:

- Movement keys (WASD)
- Modifiers (Shift, Ctrl, Alt)
- Special keys (Space, Enter, Escape, Tab)
- Arrow keys
- Function keys (F1-F4)

#### JzCameraInputComponent

Processed input specifically for camera control:

- Orbit mode active flag
- Pan mode active flag
- Mouse delta (for active mode only)
- Scroll delta for zoom
- Reset request flag

### System Flow Example: Camera Control

```cpp
// Frame N
1. m_window->PollEvents()
   ↓ GLFW callbacks fire
   ↓ JzInputManager updates internal state

2. JzInputSystem::Update()
   ↓ Reads JzInputManager
   ↓ Updates JzMouseInputComponent
   ↓ Updates JzCameraInputComponent

3. JzCameraSystem::Update()
   ↓ Reads JzCameraInputComponent
   ↓ Applies orbit/pan/zoom logic
   ↓ Updates JzCameraComponent matrices

4. JzRenderSystem::Update()
   ↓ Uses camera matrices for rendering

5. m_inputManager->ClearEvents()
   ↓ Reset press/release events for next frame
```

## Comparison with Alternative Approaches

### Alternative 1: Direct InputManager Access (Old Approach)

```cpp
// CameraSystem directly accesses InputManager
void CameraSystem::Update() {
    auto& input = ServiceContainer::Get<JzInputManager>();
    if (input.IsMouseButtonDown(LEFT)) {
        // ...
    }
}
```

**Problems:**

- ❌ Breaks ECS purity (system depends on external service)
- ❌ Hidden dependencies (not visible in component requirements)
- ❌ Hard to test (need full ServiceContainer)
- ❌ Tight coupling to platform layer

### Alternative 2: Pure Component Approach (No InputManager)

```cpp
// No InputManager, raw input goes directly to components
```

**Problems:**

- ❌ Duplicates platform abstraction code
- ❌ Mixes concerns (hardware polling with game logic)
- ❌ Poor code reuse

### Our Hybrid Approach: Best of Both Worlds

```cpp
// InputManager for platform abstraction
// InputSystem for ECS integration
// Components for game logic
```

**Benefits:**

- ✅ Clean separation of concerns
- ✅ ECS-friendly design
- ✅ Easy to test and extend
- ✅ Follows engine layering (Platform → Function)

## Code Organization

### Files Created

```
src/Runtime/Function/
├── include/JzRE/Runtime/Function/ECS/
│   └── JzInputSystem.h              # Input system header
└── src/ECS/
    └── JzInputSystem.cpp            # Input system implementation

src/Runtime/Function/include/JzRE/Runtime/Function/ECS/
└── JzRenderComponents.h            # Added input components

src/Runtime/Interface/
├── include/JzRE/Runtime/JzRERuntime.h  # Added m_inputSystem member
└── src/JzRERuntime.cpp                  # Registered InputSystem
```

### Files Modified

```
src/Runtime/Function/src/ECS/
└── JzCameraSystem.cpp               # Uses input components instead of InputManager

docs/architecture/
└── ecs.md                               # Updated with input architecture docs
```

## Future Enhancements

### 1. Input Action System

Map raw keys to abstract actions:

```cpp
struct JzInputActionComponent {
    std::unordered_map<String, Bool> actions;  // "jump" -> pressed
    std::unordered_map<String, F32> axes;      // "move_x" -> value
};
```

### 2. Input Contexts

Different input mappings per game state:

```cpp
enum class InputContext { Menu, Gameplay, Cinematic };

struct JzInputContextComponent {
    InputContext current;
    std::unordered_map<InputContext, InputMappings> contexts;
};
```

### 3. Input Buffering

Store recent input events for frame-perfect execution:

```cpp
struct JzInputBufferComponent {
    std::deque<InputEvent> buffer;
    U32 bufferSize = 10;
};
```

### 4. Input Replay

Record and replay input for testing:

```cpp
struct JzInputRecordingComponent {
    Bool recording = false;
    std::vector<FrameInput> recording;
    Size playbackFrame = 0;
};
```

## Conclusion

The hybrid input architecture successfully balances:

- **Engineering Pragmatism**: Uses existing platform abstractions
- **Architectural Purity**: Integrates cleanly with ECS
- **Performance**: Efficient, single-pass input processing
- **Maintainability**: Clear responsibilities and dependencies

This design follows the principle: **Use the right tool for each layer**.

- Platform Layer → Hardware abstraction (InputManager)
- Function Layer → Game logic integration (InputSystem)
- ECS Components → State storage and communication

The result is a scalable, testable, and maintainable input system that serves as a model for other cross-cutting concerns in the engine.

## References

- [JzInputSystem.h](../../src/Runtime/Function/include/JzRE/Runtime/Function/ECS/JzInputSystem.h)
- [JzRenderComponents.h](../../src/Runtime/Function/include/JzRE/Runtime/Function/ECS/JzRenderComponents.h)
- [ECS Architecture](./ecs.md)
- [System Phases](./rendering_pipeline.md)
