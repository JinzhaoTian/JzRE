#!/bin/bash

# 跨平台构建脚本
set -e

echo "JzRE 跨平台构建脚本"
echo "===================="

# 检测操作系统
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    PLATFORM="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM="macos"
elif [[ "$OSTYPE" == "cygwin" ]] || [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
    PLATFORM="windows"
else
    echo "不支持的操作系统: $OSTYPE"
    exit 1
fi

echo "检测到操作系统: $PLATFORM"

# 检查vcpkg是否已初始化
if [ ! -f "vcpkg/vcpkg" ] && [ ! -f "vcpkg/vcpkg.exe" ]; then
    echo "初始化vcpkg..."
    cd vcpkg
    if [[ "$PLATFORM" == "windows" ]]; then
        ./bootstrap-vcpkg.bat
    else
        ./bootstrap-vcpkg.sh
    fi
    cd ..
fi

# 调用相应的构建脚本
case $PLATFORM in
    "linux")
        echo "执行Linux构建脚本..."
        ./scripts/build-linux.sh
        ;;
    "macos")
        echo "执行macOS构建脚本..."
        ./scripts/build-macos.sh
        ;;
    "windows")
        echo "执行Windows构建脚本..."
        ./scripts/build-windows.bat
        ;;
    *)
        echo "不支持的平台: $PLATFORM"
        exit 1
        ;;
esac

echo "构建完成！" 