# JzRE 跨平台构建指南

本项目已配置为支持 Windows 和 macOS 环境的编译。

## 系统要求

### Windows
- MinGW-w64 (推荐使用 MSYS2 或 MinGW-w64)
- CMake 3.15.0 或更高版本
- Git

### macOS
- Xcode Command Line Tools
- CMake 3.15.0 或更高版本
- Git

## 构建方法

### 方法一：使用构建脚本（推荐）

#### Windows
```cmd
build.bat
```

#### macOS/Linux
```bash
./build.sh
```

### 方法二：使用 CMake 直接构建

#### Windows
```cmd
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build . --config Debug
```

#### macOS
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Debug
```

### 方法三：使用 VSCode

1. 打开项目文件夹
2. 按 `Ctrl+Shift+P` (Windows) 或 `Cmd+Shift+P` (macOS)
3. 选择 "CMake: Configure"
4. 选择 "CMake: Build"

## VSCode 调试配置

项目包含以下调试配置：

### Windows
- **CMake - Debug (Windows)**: 使用 CMake 构建和 GDB 调试
- **G++ - Debug (Windows)**: 使用 G++ 直接编译和 GDB 调试

### macOS
- **CMake - Debug (macOS)**: 使用 CMake 构建和 LLDB 调试
- **Clang++ - Debug (macOS)**: 使用 Clang++ 直接编译和 LLDB 调试

## 编译任务

### Windows
- **CMake: Build**: 使用 CMake 构建项目
- **G++: Build (Windows)**: 使用 G++ 直接编译

### macOS
- **CMake: Build**: 使用 CMake 构建项目
- **Clang++: Build (macOS)**: 使用 Clang++ 直接编译

## 输出文件

编译完成后，可执行文件将位于：
- Windows: `output/JzRE.exe`
- macOS: `output/JzRE`

## 故障排除

### Windows 常见问题

1. **找不到 MinGW**
   - 确保 MinGW-w64 已正确安装
   - 检查环境变量 PATH 是否包含 MinGW 路径

2. **CMake 生成器错误**
   - 确保使用 "MinGW Makefiles" 生成器
   - 检查 CMake 版本是否满足要求

### macOS 常见问题

1. **找不到 Xcode Command Line Tools**
   ```bash
   xcode-select --install
   ```

2. **权限问题**
   ```bash
   chmod +x build.sh
   ```

3. **库文件缺失**
   - 确保所有依赖库都已正确安装
   - 检查库文件路径是否正确

## 项目结构

```
JzRE/
├── CMakeLists.txt          # CMake 配置文件
├── build.sh               # macOS/Linux 构建脚本
├── build.bat              # Windows 构建脚本
├── .vscode/               # VSCode 配置文件
│   ├── c_cpp_properties.json
│   ├── launch.json
│   ├── settings.json
│   └── tasks.json
├── JzRE/                  # 源代码
│   ├── include/           # 头文件
│   └── source/            # 源文件
├── dependencies/          # 第三方库
└── resources/            # 资源文件
```

## 注意事项

1. 首次构建可能需要较长时间，因为需要编译所有依赖库
2. 确保所有依赖库的路径正确配置
3. 在 macOS 上，可能需要安装额外的系统库
4. 建议使用 CMake 构建方式，因为它能更好地处理依赖关系 