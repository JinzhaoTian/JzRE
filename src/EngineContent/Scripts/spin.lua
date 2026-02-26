-- spin.lua
-- Example engine script: rotates an entity around the Y axis over time.
--
-- Attach to any entity that has a JzTransformComponent:
--   world:AddComponent<JzScriptComponent>(entity, { scriptPath = "spin.lua" })

local angle = 0.0

function OnStart(entity)
    local t = world.getTransform(entity)
    if t then
        t.position = Vec3(0, 0, 0)
    end
    log.info("spin.lua OnStart entity=" .. tostring(entity))
end

function OnUpdate(entity, dt)
    angle = angle + dt

    local t = world.getTransform(entity)
    if t then
        t.rotation = Vec3(0, angle, 0)
        t:setDirty()
    end
end

function OnStop(entity)
    log.info("spin.lua OnStop entity=" .. tostring(entity))
end
