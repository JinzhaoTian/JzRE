# JzRE 单元测试

这个目录包含了JzRE项目的单元测试，使用Google Test (gtest) 框架。

## 测试覆盖

当前的测试涵盖以下组件：

### 1. JzVector 测试 (`test_jz_vector.cpp`)
- 向量构造函数测试
- 向量运算符测试（加法、减法、乘法、除法等）
- 向量数学函数测试（长度、点积、叉积、单位化等）
- 不同维度向量测试（2D、3D、4D）
- 整数和浮点数向量测试

### 2. JzMatrix 测试 (`test_jz_matrix.cpp`)  
- 矩阵构造函数测试
- 矩阵基本操作测试（加法、减法、乘法）
- 矩阵转置测试
- 4x4矩阵特殊功能测试（单位矩阵、平移、缩放）
- 旋转矩阵测试（X、Y、Z轴旋转）
- 矩阵乘法和矩阵向量乘法测试
- LookAt矩阵和投影矩阵测试

### 3. JzClock 测试 (`test_jz_clock.cpp`)
- 时钟初始状态测试
- 时钟更新和时间累积测试
- 时间缩放功能测试
- 帧率计算测试
- 边界情况测试（零缩放、负缩放、大缩放值）

## 构建和运行测试

### 前提条件
确保已经安装了所有依赖，包括gtest（通过vcpkg安装）。

### 构建
```bash
# 在项目根目录下
mkdir -p build
cd build
cmake .. --preset=default
make JzRETests
```

### 运行测试
```bash
# 运行所有测试
./bin/JzRETests

# 运行特定测试
./bin/JzRETests --gtest_filter="JzVectorTest.*"
./bin/JzRETests --gtest_filter="JzMatrixTest.*" 
./bin/JzRETests --gtest_filter="JzClockTest.*"

# 详细输出
./bin/JzRETests --gtest_output="verbose"

# 生成XML报告
./bin/JzRETests --gtest_output="xml:test_results.xml"
```

### 使用CTest运行
```bash
# 在build目录下
ctest
ctest --verbose
ctest --output-on-failure
```

## 添加新测试

要添加新的测试文件：

1. 在 `tests/` 目录下创建新的 `.cpp` 文件
2. 包含必要的头文件和 `gtest/gtest.h`
3. 编写测试类和测试用例
4. CMake会自动发现新的测试文件并编译

测试文件应该遵循以下命名约定：
- `test_[component_name].cpp` - 例如：`test_jz_renderer.cpp`

## 测试编写指南

### 测试类结构
```cpp
class ComponentTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试前的设置
    }
    
    void TearDown() override {
        // 测试后的清理
    }
    
    // 测试数据成员
};
```

### 测试用例
```cpp
TEST_F(ComponentTest, TestName) {
    // 安排（Arrange）
    // 执行（Act）  
    // 断言（Assert）
    EXPECT_EQ(expected, actual);
    ASSERT_TRUE(condition);
}
```

### 常用断言
- `EXPECT_EQ(a, b)` - 相等
- `EXPECT_NE(a, b)` - 不相等
- `EXPECT_LT(a, b)` - 小于
- `EXPECT_GT(a, b)` - 大于
- `EXPECT_FLOAT_EQ(a, b)` - 浮点数相等
- `EXPECT_NEAR(a, b, tolerance)` - 近似相等
- `EXPECT_TRUE(condition)` - 条件为真
- `EXPECT_FALSE(condition)` - 条件为假

## 持续集成

测试可以集成到CI/CD管道中：

```bash
# 构建并运行测试
cmake --build build --target JzRETests
ctest --test-dir build --output-on-failure
```

## 性能测试

对于性能敏感的组件，可以考虑添加基准测试：

```cpp
TEST_F(ComponentTest, PerformanceTest) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // 执行被测试的代码
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // 断言执行时间在合理范围内
    EXPECT_LT(duration.count(), 1000); // 少于1ms
}
```
