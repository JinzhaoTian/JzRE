#include <gtest/gtest.h>
#include "JzClock.h"
#include <thread>
#include <chrono>

using namespace JzRE;

class JzClockTest : public ::testing::Test {
protected:
    void SetUp() override {
        clock = std::make_unique<JzClock>();
    }

    void TearDown() override {
        clock.reset();
    }

    std::unique_ptr<JzClock> clock;
};

// 测试时钟初始状态
TEST_F(JzClockTest, InitialState) {
    // 初始状态下，时间应该为0或接近0
    EXPECT_GE(clock->GetTimeSinceStart(), 0.0f);
    EXPECT_EQ(clock->GetTimeScale(), 1.0f);
    EXPECT_GE(clock->GetDeltaTime(), 0.0f);
    EXPECT_GE(clock->GetDeltaTimeUnScale(), 0.0f);
}

// 测试时钟更新
TEST_F(JzClockTest, Update) {
    // 第一次更新
    clock->Update();
    F32 firstTime = clock->GetTimeSinceStart();
    F32 firstDelta = clock->GetDeltaTime();
    
    EXPECT_GE(firstTime, 0.0f);
    EXPECT_GE(firstDelta, 0.0f);

    // 等待一小段时间后再次更新
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    clock->Update();
    
    F32 secondTime = clock->GetTimeSinceStart();
    F32 secondDelta = clock->GetDeltaTime();
    
    // 时间应该增加
    EXPECT_GT(secondTime, firstTime);
    EXPECT_GT(secondDelta, 0.0f);
}

// 测试时间缩放
TEST_F(JzClockTest, TimeScale) {
    // 设置时间缩放为2.0
    clock->SetTimeScale(2.0f);
    EXPECT_FLOAT_EQ(clock->GetTimeScale(), 2.0f);
    
    clock->Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    clock->Update();
    
    F32 scaledDelta = clock->GetDeltaTime();
    F32 unscaledDelta = clock->GetDeltaTimeUnScale();
    
    // 缩放后的时间应该是未缩放时间的2倍（在误差范围内）
    EXPECT_NEAR(scaledDelta, unscaledDelta * 2.0f, 0.001f);
}

// 测试时间缩放系数
TEST_F(JzClockTest, ScaleCoefficient) {
    clock->SetTimeScale(1.0f);
    F32 originalScale = clock->GetTimeScale();
    
    // 使用Scale方法乘以系数
    clock->Scale(0.5f);
    EXPECT_FLOAT_EQ(clock->GetTimeScale(), originalScale * 0.5f);
    
    // 再次缩放
    clock->Scale(2.0f);
    EXPECT_FLOAT_EQ(clock->GetTimeScale(), originalScale * 0.5f * 2.0f);
}

// 测试帧率计算
TEST_F(JzClockTest, FrameRate) {
    // 进行多次更新以获得稳定的帧率计算
    for (int i = 0; i < 5; ++i) {
        clock->Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // 模拟约60FPS
    }
    
    F32 frameRate = clock->GetFrameRate();
    
    // 帧率应该在合理范围内（考虑到测试环境的不确定性）
    EXPECT_GT(frameRate, 30.0f);  // 至少30FPS
    EXPECT_LT(frameRate, 200.0f); // 不超过200FPS
}

// 测试零时间缩放
TEST_F(JzClockTest, ZeroTimeScale) {
    clock->SetTimeScale(0.0f);
    EXPECT_FLOAT_EQ(clock->GetTimeScale(), 0.0f);
    
    clock->Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    clock->Update();
    
    F32 scaledDelta = clock->GetDeltaTime();
    F32 unscaledDelta = clock->GetDeltaTimeUnScale();
    
    // 缩放时间应该为0，未缩放时间应该大于0
    EXPECT_FLOAT_EQ(scaledDelta, 0.0f);
    EXPECT_GT(unscaledDelta, 0.0f);
}

// 测试负时间缩放
TEST_F(JzClockTest, NegativeTimeScale) {
    clock->SetTimeScale(-1.0f);
    EXPECT_FLOAT_EQ(clock->GetTimeScale(), -1.0f);
    
    clock->Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    clock->Update();
    
    F32 scaledDelta = clock->GetDeltaTime();
    F32 unscaledDelta = clock->GetDeltaTimeUnScale();
    
    // 缩放时间应该为负值
    EXPECT_LT(scaledDelta, 0.0f);
    EXPECT_GT(unscaledDelta, 0.0f);
    EXPECT_NEAR(scaledDelta, -unscaledDelta, 0.001f);
}

// 测试大时间缩放值
TEST_F(JzClockTest, LargeTimeScale) {
    clock->SetTimeScale(100.0f);
    EXPECT_FLOAT_EQ(clock->GetTimeScale(), 100.0f);
    
    clock->Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    clock->Update();
    
    F32 scaledDelta = clock->GetDeltaTime();
    F32 unscaledDelta = clock->GetDeltaTimeUnScale();
    
    // 缩放时间应该是未缩放时间的100倍（在误差范围内）
    EXPECT_NEAR(scaledDelta, unscaledDelta * 100.0f, 0.01f);
}

// 测试连续更新的时间累积
TEST_F(JzClockTest, TimeAccumulation) {
    clock->Update();
    F32 initialTime = clock->GetTimeSinceStart();
    
    F32 totalExpectedTime = 0.0f;
    
    // 进行多次更新，每次间隔固定时间
    for (int i = 0; i < 3; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        clock->Update();
        totalExpectedTime += 0.01f; // 10ms = 0.01s
    }
    
    F32 finalTime = clock->GetTimeSinceStart();
    F32 elapsedTime = finalTime - initialTime;
    
    // 实际经过的时间应该接近预期时间（考虑到系统调度的不确定性）
    EXPECT_NEAR(elapsedTime, totalExpectedTime, 0.005f); // 5ms的误差容忍
}
