/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include <vector>

#include <gtest/gtest.h>

#include "JzRE/Runtime/Function/ECS/JzSystem.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"

namespace {

class JzShutdownOrderSystem final : public JzRE::JzSystem {
public:
    JzShutdownOrderSystem(std::vector<JzRE::I32> &order, JzRE::I32 id) :
        m_order(order),
        m_id(id)
    { }

    void Update(JzRE::JzWorld &world, JzRE::F32 delta) override
    {
        (void)world;
        (void)delta;
    }

    void OnShutdown(JzRE::JzWorld &world) override
    {
        (void)world;
        m_order.push_back(m_id);
    }

private:
    std::vector<JzRE::I32> &m_order;
    JzRE::I32               m_id = 0;
};

} // namespace

TEST(JzWorldShutdown, CallsOnShutdownInReverseOrderAndIsIdempotent)
{
    JzRE::JzWorld world;

    std::vector<JzRE::I32> shutdownOrder;

    auto first  = world.RegisterSystem<JzShutdownOrderSystem>(shutdownOrder, 1);
    auto second = world.RegisterSystem<JzShutdownOrderSystem>(shutdownOrder, 2);
    auto third  = world.RegisterSystem<JzShutdownOrderSystem>(shutdownOrder, 3);

    ASSERT_TRUE(first != nullptr);
    ASSERT_TRUE(second != nullptr);
    ASSERT_TRUE(third != nullptr);

    world.ShutdownSystems();

    const std::vector<JzRE::I32> expectedOrder = {3, 2, 1};
    EXPECT_EQ(shutdownOrder, expectedOrder);

    shutdownOrder.clear();
    world.ShutdownSystems();
    EXPECT_TRUE(shutdownOrder.empty());
}
