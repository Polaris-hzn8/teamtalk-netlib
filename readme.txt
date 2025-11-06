teamtalk通信协议及底层网络库实现
1、Linux构建方案：
mkdir build && cd build
cmake ../
make
2、Windows构建方案：
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A Win32 ..
# 打开生成的.sln文件进行构建
# 或者使用命令行构建
cmake --build . --config Release
