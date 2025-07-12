#!/bin/bash

# 检测操作系统
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "检测到 macOS 系统"
    PLATFORM="macos"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "检测到 Linux 系统"
    PLATFORM="linux"
else
    echo "不支持的操作系统: $OSTYPE"
    exit 1
fi

# 创建构建目录
mkdir -p build
mkdir -p output

# 配置CMake
echo "配置 CMake..."
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug

# 编译项目
echo "编译项目..."
cmake --build build --config Debug

# 复制可执行文件到output目录
if [ -f "build/JzRE" ]; then
    cp build/JzRE output/
    echo "编译完成！可执行文件位于: output/JzRE"
elif [ -f "build/JzRE.exe" ]; then
    cp build/JzRE.exe output/
    echo "编译完成！可执行文件位于: output/JzRE.exe"
else
    echo "编译失败！未找到可执行文件"
    exit 1
fi 