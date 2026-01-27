# Input System Design Document

## Overview

This document describes the design and implementation of the Input System in JzRE's ECS architecture.

## Design Decision: Should Input be a System?

### Answer: YES (pure ECS approach)

JzRE implements a **pure ECS input architecture** where all input flows through components and systems:

```
JzWindowSystem (Input Phase)
  └── Polls GLFW backend, syncs JzInputStateComponent, emits window events
        ↓
JzInputSystem (Input Phase)
  ├── Syncs higher-level components (JzMouseInputComponent, JzKeyboardInputComponent, etc.)
  ├── Updates action values (JzInputActionComponent)
  └── Emits input events (JzKeyEvent, JzMouseButtonEvent, etc.)
        ↓
JzEventDispatcherSystem (Input Phase)
  └── Dispatches queued events to registered handlers
        ↓
Other Systems read Input Components OR subscribe to events
```

## Architecture Benefits

### 1. Separation of Concerns

- **JzWindowSystem**: Hardware abstraction, GLFW polling, populates `JzInputStateComponent`
- **JzInputSystem**: Input processing, component updates, event emission

### 2. ECS Purity

- All input state flows through ECS components — no service-based singletons
- Systems don't directly depend on external services
- Clear data flow: WindowSystem → InputStateComponent → InputSystem → Components → Game Systems

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
1. JzWindowSystem::Update()
   ↓ Polls GLFW events
   ↓ Populates JzInputStateComponent on window entity

2. JzInputSystem::Update()
   ↓ Reads JzInputStateComponent from primary window
   ↓ Syncs JzMouseInputComponent, JzKeyboardInputComponent
   ↓ Syncs JzCameraInputStateComponent
   ↓ Updates JzInputActionComponent action values
   ↓ Emits ECS input events

3. JzCameraSystem::Update()
   ↓ Reads JzCameraInputComponent
   ↓ Applies orbit/pan/zoom logic
   ↓ Updates JzCameraComponent matrices

4. JzRenderSystem::Update()
   ↓ Uses camera matrices for rendering

5. JzInputSystem::ClearFrameState()
   ↓ Reset per-frame input flags for next frame
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

## Event-Driven Input

In addition to the component-based polling approach, the input system emits typed ECS events through `JzEventDispatcherSystem`. This enables reactive event-driven patterns alongside the existing polling model.

### Input Events

All events inherit from `JzREEvent` (timestamp, source entity, target entity) and are defined in `JzInputEvents.h`:

| Event | Fields | Emitted When |
|-------|--------|--------------|
| `JzKeyEvent` | `key` (JzEKeyCode), `scancode`, `action` (JzEKeyAction), `mods` | Key pressed or released |
| `JzMouseButtonEvent` | `button` (JzEMouseButton), `action`, `mods`, `position` | Mouse button pressed or released |
| `JzMouseMoveEvent` | `position`, `delta` | Mouse moved (non-zero delta) |
| `JzMouseScrollEvent` | `offset`, `position` | Scroll wheel used |
| `JzMouseEnterEvent` | `entered` | Cursor enters/leaves window |
| `JzInputActionTriggeredEvent` | `actionName`, `value` | Action first triggered this frame |
| `JzInputActionReleasedEvent` | `actionName`, `duration` | Action released this frame |

### Event Emission

Events are emitted at the end of `JzInputSystem::Update()` by comparing current state against the previous frame:

- **Keyboard**: Diffs `keysPressed` bitset vs previous frame, emits `JzKeyEvent` with `Pressed`/`Released` action
- **Mouse buttons**: Diffs `buttonsPressed` bitset vs previous frame, emits `JzMouseButtonEvent`
- **Mouse movement**: Emits `JzMouseMoveEvent` when `positionDelta` is non-zero
- **Scroll**: Emits `JzMouseScrollEvent` when `scrollDelta` is non-zero
- **Actions**: Emits `JzInputActionTriggeredEvent`/`JzInputActionReleasedEvent` based on action state changes

### Event Subscription Example

```cpp
void MySystem::OnInit(JzWorld &world) {
    auto &dispatcher = JzServiceContainer::Get<JzEventDispatcherSystem>();

    dispatcher.RegisterHandler<JzKeyEvent>([](const JzKeyEvent &event) {
        if (event.key == JzEKeyCode::F5 && event.action == JzEKeyAction::Pressed) {
            // Quick save
        }
    });

    dispatcher.RegisterHandler<JzInputActionTriggeredEvent>([](const JzInputActionTriggeredEvent &event) {
        if (event.actionName == "jump") {
            // Handle jump action
        }
    });
}
```

### When to Use Polling vs Events

| Approach | Use When | Example |
|----------|----------|---------|
| **Polling** (component-based) | Continuous input, every frame | Camera orbit, WASD movement |
| **Events** (dispatcher-based) | Discrete actions, one-shot triggers | Key shortcuts, button clicks |

### Input Action System (Implemented)

The `JzInputActionComponent` maps raw keys to abstract actions:

```cpp
// Define actions with bindings
JzInputActionComponent::Action jumpAction;
jumpAction.name = "jump";
jumpAction.bindings.push_back({JzInputActionComponent::BindingType::Key, .key = static_cast<I32>(JzEKeyCode::Space)});

actions.actions["jump"] = jumpAction;

// Query action state
if (actions.IsActionTriggered("jump")) { /* jump */ }
```

### Input Contexts (Implemented)

Different input mappings per game state:

```cpp
JzInputActionComponent::InputContext gameplayCtx;
gameplayCtx.name = "gameplay";
gameplayCtx.enabledActions = {"jump", "shoot", "move"};
gameplayCtx.priority = 0;

actions.contexts.push_back(gameplayCtx);
actions.activeContext = "gameplay";
```

## Future Enhancements

### 1. Input Buffering

Store recent input events for frame-perfect execution:

```cpp
struct JzInputBufferComponent {
    std::deque<InputEvent> buffer;
    U32 bufferSize = 10;
};
```

### 2. Input Replay

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
