#!/bin/bash

# Linux构建脚本
set -e

echo "开始构建 JzRE (Linux)..."

# 检查架构
ARCH=$(uname -m)
if [ "$ARCH" = "x86_64" ]; then
    TRIPLET="x64-linux-dynamic"
else
    echo "不支持的架构: $ARCH"
    exit 1
fi

echo "检测到架构: $ARCH，使用triplet: $TRIPLET"

# 创建构建目录
BUILD_DIR="build-linux-$ARCH"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# 安装vcpkg依赖
echo "安装vcpkg依赖..."
../vcpkg/vcpkg install --triplet=$TRIPLET --x-install-root=vcpkg_installed

# 配置项目
echo "配置项目..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DVCPKG_TARGET_TRIPLET=$TRIPLET

# 构建项目
echo "构建项目..."
cmake --build . --config Release -j$(nproc)

echo "构建完成！可执行文件位于: $BUILD_DIR/bin/JzRE" 