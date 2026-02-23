/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include <thread>
#include <vector>

#include <gtest/gtest.h>

#include "JzRE/Runtime/Platform/Command/JzRHICommandList.h"

namespace JzRE {

TEST(JzRHICommandListTest, RecordsCommandsDuringRecording)
{
    JzRHICommandList commandList("UnitTestList");

    EXPECT_TRUE(commandList.IsEmpty());
    EXPECT_FALSE(commandList.IsRecording());

    commandList.Begin();
    EXPECT_TRUE(commandList.IsRecording());

    JzDrawParams drawParams;
    drawParams.primitiveType = JzEPrimitiveType::Triangles;
    drawParams.vertexCount   = 3;
    drawParams.instanceCount = 1;
    drawParams.firstVertex   = 0;
    drawParams.firstInstance = 0;
    commandList.Draw(drawParams);

    commandList.End();
    EXPECT_FALSE(commandList.IsRecording());
    EXPECT_EQ(commandList.GetCommandCount(), 1U);

    const auto commands = commandList.GetCommands();
    ASSERT_EQ(commands.size(), 1U);
    EXPECT_EQ(commands[0].type, JzRHIECommandType::Draw);

    const auto *payload = std::get_if<JzDrawParams>(&commands[0].payload);
    ASSERT_NE(payload, nullptr);
    EXPECT_EQ(payload->vertexCount, 3U);
}

TEST(JzRHICommandListTest, BeginResetsPreviouslyRecordedCommands)
{
    JzRHICommandList commandList("UnitTestList");

    commandList.Begin();
    JzClearParams clearParams;
    commandList.Clear(clearParams);
    commandList.End();

    EXPECT_EQ(commandList.GetCommandCount(), 1U);

    commandList.Begin();
    commandList.End();

    EXPECT_TRUE(commandList.IsEmpty());
}

TEST(JzRHICommandListTest, IgnoresCommandsOutsideRecordingState)
{
    JzRHICommandList commandList("UnitTestList");

    JzDrawParams drawParams;
    drawParams.vertexCount = 3;
    commandList.Draw(drawParams);

    EXPECT_TRUE(commandList.IsEmpty());
}

TEST(JzRHICommandListTest, SupportsConcurrentRecording)
{
    JzRHICommandList commandList("UnitTestList");

    constexpr U32 kThreadCount       = 4;
    constexpr U32 kCommandsPerThread = 128;

    commandList.Begin();

    std::vector<std::thread> workers;
    workers.reserve(kThreadCount);

    for (U32 i = 0; i < kThreadCount; ++i) {
        workers.emplace_back([&commandList]() {
            JzDrawParams drawParams;
            drawParams.vertexCount   = 3;
            drawParams.instanceCount = 1;
            drawParams.firstVertex   = 0;
            drawParams.firstInstance = 0;

            for (U32 n = 0; n < kCommandsPerThread; ++n) {
                commandList.Draw(drawParams);
            }
        });
    }

    for (auto &worker : workers) {
        worker.join();
    }

    commandList.End();

    EXPECT_EQ(commandList.GetCommandCount(), static_cast<Size>(kThreadCount * kCommandsPerThread));
}

} // namespace JzRE
