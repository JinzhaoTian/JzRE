@echo off
setlocal enabledelayedexpansion

echo 检测到 Windows 系统

REM 创建构建目录
if not exist build mkdir build
if not exist output mkdir output

REM 配置CMake
echo 配置 CMake...
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -G "MinGW Makefiles"

REM 编译项目
echo 编译项目...
cmake --build build --config Debug

REM 复制可执行文件到output目录
if exist "build\JzRE.exe" (
    copy "build\JzRE.exe" "output\"
    echo 编译完成！可执行文件位于: output\JzRE.exe
) else (
    echo 编译失败！未找到可执行文件
    exit /b 1
)

pause 