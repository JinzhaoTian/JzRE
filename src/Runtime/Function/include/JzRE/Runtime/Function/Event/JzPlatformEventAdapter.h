/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <vector>

#include "JzRE/Runtime/Core/JzPlatformEvent.h"
#include "JzRE/Runtime/Core/JzPlatformEventQueue.h"
#include "JzRE/Runtime/Function/ECS/JzEntity.h"
#include "JzRE/Runtime/Function/Event/JzEventDispatcherSystem.h"
#include "JzRE/Runtime/Function/Event/JzInputEvents.h"
#include "JzRE/Runtime/Function/Event/JzWindowEvents.h"
#include "JzRE/Runtime/Platform/Window/JzPlatformInputEvents.h"

namespace JzRE {

/**
 * @brief Adapts Platform layer events to ECS events with entity context.
 *
 * This class bridges the Platform layer (no entity awareness) with the
 * Function layer ECS event system. It consumes events from JzPlatformEventQueue
 * and dispatches them as JzREEvent-derived types through JzEventDispatcherSystem.
 */
class JzPlatformEventAdapter {
public:
    /**
     * @brief Process all pending platform events and dispatch as ECS events.
     *
     * @param platformQueue Source queue from JzIWindowBackend
     * @param dispatcher Target ECS event dispatcher
     * @param windowEntity The window entity to use as source
     * @param maxEvents Maximum number of events to process per call (0 = unlimited)
     */
    void ProcessPlatformEvents(
        JzPlatformEventQueue    &platformQueue,
        JzEventDispatcherSystem &dispatcher,
        JzEntity                 windowEntity,
        size_t                   maxEvents = 256)
    {
        std::vector<JzPlatformEventWrapper> events;
        events.reserve(maxEvents > 0 ? maxEvents : 64);

        size_t count = platformQueue.PopBatch(events, maxEvents > 0 ? maxEvents : SIZE_MAX);

        for (size_t i = 0; i < count; ++i) {
            DispatchAsECSEvent(events[i], dispatcher, windowEntity);
        }
    }

private:
    void DispatchAsECSEvent(
        const JzPlatformEventWrapper &wrapper,
        JzEventDispatcherSystem      &dispatcher,
        JzEntity                      windowEntity)
    {
        // Key events
        if (auto *platformEvent = wrapper.As<JzPlatformKeyEvent>()) {
            JzKeyEvent ecsEvent;
            ecsEvent.timestamp = platformEvent->timestamp;
            ecsEvent.source    = windowEntity;
            ecsEvent.key       = static_cast<JzEKeyCode>(platformEvent->key);
            ecsEvent.scancode  = platformEvent->scancode;
            ecsEvent.action    = static_cast<JzEKeyAction>(platformEvent->action);
            ecsEvent.mods      = platformEvent->mods;
            dispatcher.Send(std::move(ecsEvent));
            return;
        }

        // Mouse button events
        if (auto *platformEvent = wrapper.As<JzPlatformMouseButtonEvent>()) {
            JzMouseButtonEvent ecsEvent;
            ecsEvent.timestamp = platformEvent->timestamp;
            ecsEvent.source    = windowEntity;
            ecsEvent.button    = static_cast<JzEMouseButton>(platformEvent->button);
            ecsEvent.action    = static_cast<JzEKeyAction>(platformEvent->action);
            ecsEvent.mods      = platformEvent->mods;
            ecsEvent.position  = platformEvent->position;
            dispatcher.Send(std::move(ecsEvent));
            return;
        }

        // Mouse move events
        if (auto *platformEvent = wrapper.As<JzPlatformMouseMoveEvent>()) {
            JzMouseMoveEvent ecsEvent;
            ecsEvent.timestamp = platformEvent->timestamp;
            ecsEvent.source    = windowEntity;
            ecsEvent.position  = platformEvent->position;
            // Note: delta is computed by the input system based on previous position
            ecsEvent.delta = {0.0f, 0.0f};
            dispatcher.Send(std::move(ecsEvent));
            return;
        }

        // Mouse scroll events
        if (auto *platformEvent = wrapper.As<JzPlatformMouseScrollEvent>()) {
            JzMouseScrollEvent ecsEvent;
            ecsEvent.timestamp = platformEvent->timestamp;
            ecsEvent.source    = windowEntity;
            ecsEvent.offset    = platformEvent->offset;
            dispatcher.Send(std::move(ecsEvent));
            return;
        }

        // Mouse enter events
        if (auto *platformEvent = wrapper.As<JzPlatformMouseEnterEvent>()) {
            JzMouseEnterEvent ecsEvent;
            ecsEvent.timestamp = platformEvent->timestamp;
            ecsEvent.source    = windowEntity;
            ecsEvent.entered   = platformEvent->entered;
            dispatcher.Send(std::move(ecsEvent));
            return;
        }

        // Window resize events
        if (auto *platformEvent = wrapper.As<JzPlatformWindowResizeEvent>()) {
            JzWindowResizedEvent ecsEvent;
            ecsEvent.timestamp = platformEvent->timestamp;
            ecsEvent.source    = windowEntity;
            ecsEvent.size      = platformEvent->size;
            dispatcher.Send(std::move(ecsEvent));
            return;
        }

        // Framebuffer resize events
        if (auto *platformEvent = wrapper.As<JzPlatformFramebufferResizeEvent>()) {
            JzWindowFramebufferResizedEvent ecsEvent;
            ecsEvent.timestamp = platformEvent->timestamp;
            ecsEvent.source    = windowEntity;
            ecsEvent.size      = platformEvent->size;
            dispatcher.Send(std::move(ecsEvent));
            return;
        }

        // Window move events
        if (auto *platformEvent = wrapper.As<JzPlatformWindowMoveEvent>()) {
            JzWindowMovedEvent ecsEvent;
            ecsEvent.timestamp = platformEvent->timestamp;
            ecsEvent.source    = windowEntity;
            ecsEvent.position  = platformEvent->position;
            dispatcher.Send(std::move(ecsEvent));
            return;
        }

        // Window focus events
        if (auto *platformEvent = wrapper.As<JzPlatformWindowFocusEvent>()) {
            JzWindowFocusEvent ecsEvent;
            ecsEvent.timestamp = platformEvent->timestamp;
            ecsEvent.source    = windowEntity;
            ecsEvent.focused   = platformEvent->focused;
            dispatcher.Send(std::move(ecsEvent));
            return;
        }

        // Window iconify events
        if (auto *platformEvent = wrapper.As<JzPlatformWindowIconifyEvent>()) {
            JzWindowIconifiedEvent ecsEvent;
            ecsEvent.timestamp = platformEvent->timestamp;
            ecsEvent.source    = windowEntity;
            ecsEvent.iconified = platformEvent->iconified;
            dispatcher.Send(std::move(ecsEvent));
            return;
        }

        // Window maximize events
        if (auto *platformEvent = wrapper.As<JzPlatformWindowMaximizeEvent>()) {
            JzWindowMaximizedEvent ecsEvent;
            ecsEvent.timestamp = platformEvent->timestamp;
            ecsEvent.source    = windowEntity;
            ecsEvent.maximized = platformEvent->maximized;
            dispatcher.Send(std::move(ecsEvent));
            return;
        }

        // Window close events
        if (auto *platformEvent = wrapper.As<JzPlatformWindowCloseEvent>()) {
            JzWindowClosedEvent ecsEvent;
            ecsEvent.timestamp = platformEvent->timestamp;
            ecsEvent.source    = windowEntity;
            dispatcher.Send(std::move(ecsEvent));
            return;
        }

        // File drop events
        if (auto *platformEvent = wrapper.As<JzPlatformFileDropEvent>()) {
            JzFileDroppedEvent ecsEvent;
            ecsEvent.timestamp    = platformEvent->timestamp;
            ecsEvent.source       = windowEntity;
            ecsEvent.filePaths    = platformEvent->paths;
            ecsEvent.dropPosition = platformEvent->position;
            dispatcher.Send(std::move(ecsEvent));
            return;
        }

        // Content scale events
        if (auto *platformEvent = wrapper.As<JzPlatformContentScaleEvent>()) {
            JzWindowContentScaleChangedEvent ecsEvent;
            ecsEvent.timestamp = platformEvent->timestamp;
            ecsEvent.source    = windowEntity;
            ecsEvent.scale     = platformEvent->scale;
            dispatcher.Send(std::move(ecsEvent));
            return;
        }

        // Character input events are handled separately by text input systems
        // JzPlatformCharEvent -> no direct ECS event mapping currently
    }
};

} // namespace JzRE
