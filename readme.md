# TeamTalk IMCore 编译和安装指南

---

仓库定位：

1. 跨平台网络库
2. 服务端与客户端公用基础
3. 服务端与客户端通信协议定义

## 前置条件

### 1. 依赖库
确保 `.sdk` 目录下已安装以下依赖：

```bash
teamtalk/.sdk/
├── protobuf/
│   ├── include/google/protobuf/
│   ├── lib/libprotobuf-lite.so (或 .a)
│   └── bin/protoc
├── jsoncpp/
│   ├── include/json
│   └── lib/jsoncpp.so (或 .a)
└── spdlog/
    └── include/spdlog/
```

### 2. 系统工具
- CMake >= 3.10
- C++ 编译器（支持 C++11）
  - Linux: g++ 或 clang++
  - Windows: MSVC 或 MinGW
- Python 3.x（用于生成 protobuf 代码）

## 编译步骤

### 步骤 1: 生成 Protobuf 代码

```bash
cd proto
python3 build.py
```

这将：
- 查找所有 `.proto` 文件
- 生成 C++ 代码（`.pb.h` 和 `.pb.cc`）
- 将头文件复制到 `include/imcore/ttidl/`
- 将源文件复制到 `src/imcore/ttidl/`

### 步骤 2: 配置构建

```bash
# 返回项目根目录
cd ..

# 创建构建目录
mkdir build
cd build

# 配置 CMake
cmake ..
```

**自定义安装路径（可选）：**
```bash
cmake -DCMAKE_INSTALL_PREFIX=/custom/path ..
```

### 步骤 3: 编译

```bash
make
```

或者使用多核编译加速：
```bash
make -j$(nproc)  # Linux
make -j4         # 使用 4 个核心
```

### 步骤 4: 安装

```bash
make install
```

默认安装到 `teamtalk/.sdk/teamtalk-imcore/` 目录：
```
teamtalk/.sdk/teamtalk-imcore/
├── include/imcore/        # 头文件
│   ├── common/
│   ├── netlib/
│   ├── security/
│   ├── slog/
│   ├── string/
│   ├── ttidl/            # protobuf 生成的头文件
│   └── ostype.h
└── lib/
    ├── libimcore.so       # Linux 动态库
    ├── libimcore.so.1
    └── libimcore.so.1.0.0
    # 或 Windows:
    # ├── imcore.dll
    # └── imcore.lib
```

## 快速编译脚本

创建一个一键编译脚本 `build.sh`：

```bash
#!/bin/bash
set -e

echo "=========================================="
echo "TeamTalk IMCore Build Script"
echo "=========================================="

# 1. 生成 protobuf 代码
echo "Step 1: Generating protobuf code..."
cd proto
python3 build.py
cd ..

# 2. 配置和编译
echo "Step 2: Building..."
if [ ! -d "build" ]; then
    mkdir build
fi
cd build
cmake ..
make -j$(nproc)

# 3. 安装
echo "Step 3: Installing..."
make install

echo "=========================================="
echo "Build completed successfully!"
echo "Library installed to: $(cmake -L .. 2>/dev/null | grep CMAKE_INSTALL_PREFIX | cut -d= -f2)"
echo "=========================================="
```

使用方法：
```bash
chmod +x build.sh
./build.sh
```

## Windows 编译

### 使用 Visual Studio

```cmd
REM 1. 生成 protobuf 代码
cd proto
python build.py
cd ..

REM 2. 生成 VS 项目
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A x64 ..

REM 3. 打开 VS 项目或命令行编译
cmake --build . --config Release

REM 4. 安装
cmake --install . --config Release
```

### 使用 MinGW

```cmd
cd proto
python build.py
cd ..

mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
mingw32-make install
```

