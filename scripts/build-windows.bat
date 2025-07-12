@echo off
REM Windows构建脚本
setlocal EnableDelayedExpansion

echo 开始构建 JzRE (Windows)...

REM 检查架构
set ARCH=x64
set TRIPLET=x64-windows-dynamic

echo 使用triplet: %TRIPLET%

REM 创建构建目录
set BUILD_DIR=build-windows-%ARCH%
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%"

REM 安装vcpkg依赖
echo 安装vcpkg依赖...
..\vcpkg\vcpkg.exe install --triplet=%TRIPLET% --x-install-root=vcpkg_installed

REM 配置项目
echo 配置项目...
cmake .. ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake ^
    -DVCPKG_TARGET_TRIPLET=%TRIPLET% ^
    -A x64

REM 构建项目
echo 构建项目...
cmake --build . --config Release

echo 构建完成！可执行文件位于: %BUILD_DIR%\bin\Release\JzRE.exe
pause 