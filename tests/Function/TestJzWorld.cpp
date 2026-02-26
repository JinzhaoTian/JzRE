/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include <vector>

#include <gtest/gtest.h>

#include "JzRE/Runtime/Function/ECS/JzSystem.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"

using namespace JzRE;

namespace {

// Simple POD components for testing
struct Position {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct Velocity {
    float dx = 0.0f;
    float dy = 0.0f;
};

struct Tag {
    int id = 0;
};

// A system that counts how many times Update() was called
class CounterSystem final : public JzSystem {
public:
    void Update(JzWorld &, F32) override
    {
        ++updateCount;
    }

    int updateCount = 0;
};

// A system that tracks shutdown calls
class LifecycleSystem final : public JzSystem {
public:
    LifecycleSystem(std::vector<std::string> &log, const std::string &name) :
        m_log(log),
        m_name(name)
    { }

    void Update(JzWorld &, F32) override { }

    void OnShutdown(JzWorld &) override
    {
        m_log.push_back(m_name + ":shutdown");
    }

private:
    std::vector<std::string> &m_log;
    std::string               m_name;
};

// A system that respects the enabled flag
class EnableAwareSystem final : public JzSystem {
public:
    void Update(JzWorld &, F32) override
    {
        ++updateCount;
    }

    int updateCount = 0;
};

} // namespace

// ===========================================================================
// Entity management
// ===========================================================================

TEST(JzWorld, CreateEntityReturnsValidEntity)
{
    JzWorld  world;
    JzEntity e = world.CreateEntity();
    EXPECT_TRUE(world.IsValid(e));
}

TEST(JzWorld, DestroyEntityInvalidatesIt)
{
    JzWorld  world;
    JzEntity e = world.CreateEntity();

    world.DestroyEntity(e);
    EXPECT_FALSE(world.IsValid(e));
}

TEST(JzWorld, MultipleEntitiesAreUnique)
{
    JzWorld  world;
    JzEntity e1 = world.CreateEntity();
    JzEntity e2 = world.CreateEntity();
    JzEntity e3 = world.CreateEntity();

    EXPECT_NE(e1, e2);
    EXPECT_NE(e2, e3);
    EXPECT_NE(e1, e3);
}

// ===========================================================================
// Component management
// ===========================================================================

TEST(JzWorld, AddComponentAndGetComponent)
{
    JzWorld  world;
    JzEntity e = world.CreateEntity();

    world.AddComponent<Position>(e, 1.0f, 2.0f, 3.0f);

    Position &pos = world.GetComponent<Position>(e);
    EXPECT_FLOAT_EQ(pos.x, 1.0f);
    EXPECT_FLOAT_EQ(pos.y, 2.0f);
    EXPECT_FLOAT_EQ(pos.z, 3.0f);
}

TEST(JzWorld, HasComponentReturnsTrueAfterAdd)
{
    JzWorld  world;
    JzEntity e = world.CreateEntity();

    EXPECT_FALSE(world.HasComponent<Position>(e));

    world.AddComponent<Position>(e);
    EXPECT_TRUE(world.HasComponent<Position>(e));
}

TEST(JzWorld, HasComponentReturnsFalseForDifferentComponent)
{
    JzWorld  world;
    JzEntity e = world.CreateEntity();

    world.AddComponent<Position>(e);
    EXPECT_FALSE(world.HasComponent<Velocity>(e));
}

TEST(JzWorld, RemoveComponentUnregistersIt)
{
    JzWorld  world;
    JzEntity e = world.CreateEntity();

    world.AddComponent<Position>(e);
    EXPECT_TRUE(world.HasComponent<Position>(e));

    world.RemoveComponent<Position>(e);
    EXPECT_FALSE(world.HasComponent<Position>(e));
}

TEST(JzWorld, TryGetComponentReturnsPointerWhenPresent)
{
    JzWorld  world;
    JzEntity e = world.CreateEntity();

    world.AddComponent<Tag>(e, 99);

    Tag *ptr = world.TryGetComponent<Tag>(e);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->id, 99);
}

TEST(JzWorld, TryGetComponentReturnsNullptrWhenAbsent)
{
    JzWorld  world;
    JzEntity e = world.CreateEntity();

    EXPECT_EQ(world.TryGetComponent<Tag>(e), nullptr);
}

TEST(JzWorld, AddOrReplaceComponentOverwritesExisting)
{
    JzWorld  world;
    JzEntity e = world.CreateEntity();

    world.AddComponent<Tag>(e, 1);
    world.AddOrReplaceComponent<Tag>(e, 2);

    EXPECT_EQ(world.GetComponent<Tag>(e).id, 2);
}

TEST(JzWorld, MultipleComponentTypesOnSameEntity)
{
    JzWorld  world;
    JzEntity e = world.CreateEntity();

    world.AddComponent<Position>(e, 10.0f, 20.0f, 30.0f);
    world.AddComponent<Velocity>(e, 1.0f, -1.0f);
    world.AddComponent<Tag>(e, 7);

    EXPECT_TRUE(world.HasComponent<Position>(e));
    EXPECT_TRUE(world.HasComponent<Velocity>(e));
    EXPECT_TRUE(world.HasComponent<Tag>(e));

    EXPECT_FLOAT_EQ(world.GetComponent<Position>(e).x, 10.0f);
    EXPECT_FLOAT_EQ(world.GetComponent<Velocity>(e).dx, 1.0f);
    EXPECT_EQ(world.GetComponent<Tag>(e).id, 7);
}

TEST(JzWorld, GetComponentMutationIsReflected)
{
    JzWorld  world;
    JzEntity e = world.CreateEntity();

    world.AddComponent<Tag>(e, 0);
    world.GetComponent<Tag>(e).id = 55;

    EXPECT_EQ(world.GetComponent<Tag>(e).id, 55);
}

// ===========================================================================
// View / query
// ===========================================================================

TEST(JzWorld, ViewIteratesEntitiesWithMatchingComponents)
{
    JzWorld world;

    JzEntity e1 = world.CreateEntity();
    JzEntity e2 = world.CreateEntity();
    JzEntity e3 = world.CreateEntity(); // no Position

    world.AddComponent<Position>(e1, 1.0f, 0.0f, 0.0f);
    world.AddComponent<Position>(e2, 2.0f, 0.0f, 0.0f);
    world.AddComponent<Velocity>(e3, 1.0f, 0.0f);

    int count = 0;
    for (auto [entity, pos] : world.View<Position>().each()) {
        (void)entity;
        (void)pos;
        ++count;
    }

    EXPECT_EQ(count, 2);
}

TEST(JzWorld, ViewWithMultipleComponentsFiltersCorrectly)
{
    JzWorld world;

    JzEntity e1 = world.CreateEntity();
    JzEntity e2 = world.CreateEntity();

    world.AddComponent<Position>(e1);
    world.AddComponent<Velocity>(e1);

    world.AddComponent<Position>(e2); // no Velocity

    int count = 0;
    for (auto [entity, pos, vel] : world.View<Position, Velocity>().each()) {
        (void)entity;
        (void)pos;
        (void)vel;
        ++count;
    }

    EXPECT_EQ(count, 1);
}

// ===========================================================================
// System management
// ===========================================================================

TEST(JzWorld, RegisterSystemReturnsNonNull)
{
    JzWorld world;
    auto    sys = world.RegisterSystem<CounterSystem>();
    EXPECT_NE(sys, nullptr);
}

TEST(JzWorld, UpdateCallsSystemUpdate)
{
    JzWorld world;
    auto    sys = world.RegisterSystem<CounterSystem>();

    world.Update(0.016f);
    EXPECT_EQ(sys->updateCount, 1);

    world.Update(0.016f);
    EXPECT_EQ(sys->updateCount, 2);
}

TEST(JzWorld, DisabledSystemIsSkippedDuringUpdate)
{
    JzWorld world;
    auto    sys = world.RegisterSystem<EnableAwareSystem>();

    sys->SetEnabled(false);
    world.Update(0.016f);

    EXPECT_EQ(sys->updateCount, 0);
}

TEST(JzWorld, ReenablingSystemResumesUpdates)
{
    JzWorld world;
    auto    sys = world.RegisterSystem<EnableAwareSystem>();

    sys->SetEnabled(false);
    world.Update(0.016f);
    EXPECT_EQ(sys->updateCount, 0);

    sys->SetEnabled(true);
    world.Update(0.016f);
    EXPECT_EQ(sys->updateCount, 1);
}

TEST(JzWorld, SystemPhaseDefaultIsLogic)
{
    JzWorld world;
    auto    sys = world.RegisterSystem<CounterSystem>();
    EXPECT_EQ(sys->GetPhase(), JzSystemPhase::Logic);
}

TEST(JzWorld, MultipleSystemsAllReceiveUpdate)
{
    JzWorld world;
    auto    s1 = world.RegisterSystem<CounterSystem>();
    auto    s2 = world.RegisterSystem<CounterSystem>();

    world.Update(0.016f);

    EXPECT_EQ(s1->updateCount, 1);
    EXPECT_EQ(s2->updateCount, 1);
}

// ===========================================================================
// System shutdown
// ===========================================================================

TEST(JzWorld, ShutdownSystemsCallsOnShutdownInReverseOrder)
{
    JzWorld                  world;
    std::vector<std::string> log;

    world.RegisterSystem<LifecycleSystem>(log, "A");
    world.RegisterSystem<LifecycleSystem>(log, "B");
    world.RegisterSystem<LifecycleSystem>(log, "C");

    world.ShutdownSystems();

    ASSERT_EQ(log.size(), 3u);
    EXPECT_EQ(log[0], "C:shutdown");
    EXPECT_EQ(log[1], "B:shutdown");
    EXPECT_EQ(log[2], "A:shutdown");
}

TEST(JzWorld, ShutdownIsIdempotent)
{
    JzWorld                  world;
    std::vector<std::string> log;

    world.RegisterSystem<LifecycleSystem>(log, "A");

    world.ShutdownSystems();
    EXPECT_EQ(log.size(), 1u);

    log.clear();
    world.ShutdownSystems(); // second call must not invoke OnShutdown again
    EXPECT_TRUE(log.empty());
}
