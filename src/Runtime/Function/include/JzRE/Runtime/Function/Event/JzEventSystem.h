/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <algorithm>
#include <map>
#include <vector>
#include <unordered_map>
#include <memory>
#include "JzRE/Runtime/Function/ECS/JzSystem.h"
#include "JzRE/Runtime/Function/Event/JzEventQueue.h"
#include "JzRE/Runtime/Function/Event/JzIEventHandler.h"
#include "JzRE/Runtime/Core/JzLogger.h"

namespace JzRE {

class JzEventSystem : public JzSystem {
public:
    void OnInit(JzWorld &world) override
    {
        // Initialization if needed
    }

    void Update(JzWorld &world, F32 delta) override
    {
        // Process delayed events
        ProcessDelayedEvents();

        // Process global events
        ProcessGlobalQueue();

        // Dispatch to systems (if we were tracking other systems queues, but here we keep it simple for now as per design doc logic adapted)
        DispatchToSystems();

        // Update stats
        UpdateStats(delta);
    }

    JzSystemPhase GetPhase() const override
    {
        return JzSystemPhase::Input;
    }

    // Register Handler
    template <typename T>
    JzEventHandler<T> *RegisterHandler(std::function<void(const T &)> handler, I32 priority = 0)
    {
        auto  u32EventId   = JzECSEventType<T>::Id();
        auto  eventHandler = std::make_unique<JzEventHandler<T>>(std::move(handler), priority);
        auto *ptr          = eventHandler.get();

        m_handlers[u32EventId].push_back(std::move(eventHandler));
        m_priorityQueues[priority].push_back(ptr);

        return ptr;
    }

    // Send Event
    template <typename T>
    void Send(T &&event)
    {
        m_globalQueue.Push(std::forward<T>(event));
        m_stats.eventsByType[JzECSEventType<T>::Id()]++;
    }

    // Send to Entity
    template <typename T>
    void SendToEntity(JzEntity entity, T &&event)
    {
        event.target = entity;
        Send(std::forward<T>(event));
    }

    // Delayed events simply pushed to a vector to be re-queued next frame
    template <typename T>
    void SendDelayed(T &&event, F32 delay = 0.0f)
    {
        // Simple implementation: if delay <= 0, add to delayedEvents to process next frame
        // Complex timed events not fully implemented to keep it simple, treating all delayed as "Next Frame" for now unless we add a specific timinq queue.
        // Design doc uses TimeDelta.
        m_delayedEvents.emplace_back(std::forward<T>(event));
    }

    void RemoveHandler(JzIEventHandler *handler)
    {
        if (!handler) return;

        auto  u32EventId  = handler->GetEventType();
        auto &handlerList = m_handlers[u32EventId];

        handlerList.erase(std::remove_if(handlerList.begin(), handlerList.end(),
                                         [handler](const auto &h) { return h.get() == handler; }),
                          handlerList.end());

        // Also remove from priority queues
        // This is slow (O(N)), but removal is rare.
        for (auto &pair : m_priorityQueues) {
            auto &vec = pair.second;
            vec.erase(std::remove(vec.begin(), vec.end(), handler), vec.end());
        }
    }

private:
    void ProcessGlobalQueue()
    {
        std::vector<JzECSEventWrapper> events;
        const size_t                   batchSize = 100;

        while (true) {
            size_t count = m_globalQueue.PopBatch(events, batchSize);
            if (count == 0) break;

            for (auto &event : events) {
                ProcessSingleEvent(event);
            }

            m_stats.eventsProcessed += count;
            events.clear();
        }
    }

    void ProcessSingleEvent(const JzECSEventWrapper &event)
    {
        auto u32EventId = event.GetTypeId();
        auto it         = m_handlers.find(u32EventId);

        if (it != m_handlers.end()) {
            // Process by priority
            // Iterate priority map (ordered)
            // std::map sorts by key ascending. Design doc uses priority. Usually higher number = higher priority?
            // Or lower number? Design doc doesn't specify, but `return priority`.
            // Standard: Higher is better, or Lower is earlier?
            // Let's assume standard map iteration (ascending). So 0 first, then 1...
            // If we want High Priority first, we should iterate reverse or use std::greater.
            // But existing code uses `std::map<int, ...>`.
            // I'll stick to map iteration order (ascending keys).
            for (const auto &priorityGroup : m_priorityQueues) {
                for (auto *handler : priorityGroup.second) {
                    if (handler->GetEventType() == u32EventId) {
                        try {
                            handler->HandleEvent(event);
                        } catch (const std::exception &e) {
                            JzRE_LOG_ERROR("Event handler error: {}", e.what());
                        }
                    }
                }
            }
        } else {
            m_stats.eventsDropped++;
        }
    }

    void DispatchToSystems()
    {
        // Placeholder: If systems have their own queues
        // for (auto& [system, queue] : systemQueues_) ...
    }

    void ProcessDelayedEvents()
    {
        for (auto &event : m_delayedEvents) {
            m_globalQueue.PushWrapper(std::move(event));
        }
        m_delayedEvents.clear();
    }

    void UpdateStats(F32 delta)
    {
        static F32 lastStatTime  = 0.0f;
        lastStatTime            += delta;

        if (lastStatTime >= 5.0f) {
            JzRE_LOG_DEBUG("Event System Stats: Processed: {}, Dropped: {}", m_stats.eventsProcessed.load(), m_stats.eventsDropped.load());
            lastStatTime = 0.0f;
        }
    }

private:
    std::unordered_map<U32, std::vector<std::unique_ptr<JzIEventHandler>>> m_handlers;
    std::map<I32, std::vector<JzIEventHandler *>>                          m_priorityQueues;
    JzEventQueue                                                           m_globalQueue;
    std::vector<JzECSEventWrapper>                                         m_delayedEvents;

    struct Stats {
        std::atomic<U64>             eventsProcessed{0};
        std::atomic<U64>             eventsDropped{0};
        std::unordered_map<U32, U64> eventsByType;
    } m_stats;
};

} // namespace JzRE
