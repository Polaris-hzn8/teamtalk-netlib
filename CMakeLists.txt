cmake_minimum_required(VERSION 3.10)
project(imcore)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(THIRD_DIR ${CMAKE_SOURCE_DIR}/third)

# 递归收集各模块源文件
file(GLOB_RECURSE NETCORE_HEADERS  "src/core/*.h"     "src/core/*.hpp")
file(GLOB_RECURSE NETCORE_SOURCES  "src/core/*.cpp"   "src/core/*.cc")
file(GLOB_RECURSE IMPDU_HEADERS    "src/impdu/*.h"    "src/impdu/*.hpp")
file(GLOB_RECURSE IMPDU_SOURCES    "src/impdu/*.cpp"  "src/impdu/*.cc")
file(GLOB_RECURSE UTILITY_HEADERS  "src/utility/*.h"  "src/utility/*.hpp")
file(GLOB_RECURSE UTILITY_SOURCES  "src/utility/*.cpp")
file(GLOB_RECURSE SECURITY_HEADERS "src/security/*.h" "src/security/*.hpp")
file(GLOB_RECURSE SECURITY_SOURCES "src/security/*.cpp" "src/security/*.c")
file(GLOB_RECURSE JSONCPP_HEADERS  "src/jsoncpp/*.h"  "src/jsoncpp/*.hpp")
file(GLOB_RECURSE JSONCPP_SOURCES  "src/jsoncpp/*.cpp")
file(GLOB_RECURSE SLOG_HEADERS     "src/slog/*.h"     "src/slog/*.hpp")
file(GLOB_RECURSE SLOG_SOURCES     "src/slog/*.cpp")
file(GLOB_RECURSE PB_HEADERS       "src/pbgen/*.h"    "src/pbgen/*.hpp")
file(GLOB_RECURSE PB_SOURCES       "src/pbgen/*.cpp"  "src/pbgen/*.cc")
# src 根目录（im_conn、ostype 等）
file(GLOB MAIN_HEADERS "src/*.h" "src/*.hpp")
file(GLOB MAIN_SOURCES "src/*.cpp" "src/*.cc")

########################################################################
# 平台差异化
if(WIN32)
    message(STATUS "Configuring for Windows")
    add_definitions(
        -DWIN32
        -D_WINDOWS
        -D_CRT_SECURE_NO_WARNINGS
        -D_WINSOCK_DEPRECATED_NO_WARNINGS
        -DTIXML_USE_STL
    )
    if(MSVC)
        add_compile_options(/W3)
    endif()
    set(PLATFORM_LIBS protobuf-lite)
else()
    message(STATUS "Configuring for Linux")
    add_definitions(
        -DLINUX
        -D_UNIX
        -D_REENTRANT
        -D_FILE_OFFSET_BITS=64
        -DTIXML_USE_STL
        -DLINUX_DAEMON
    )
    add_compile_options(-g -W -Wall)
    # pthread 是 spdlog 异步模式在 Linux 下的依赖
    set(PLATFORM_LIBS protobuf-lite pthread)
endif()

########################################################################
# 头文件路径
include_directories(
    src/
    src/core
    src/impdu
    src/pbgen
    src/slog
    src/utility
    src/security
    src/jsoncpp
    src/jsoncpp/json
    ${THIRD_DIR}/protobuf/include
    ${THIRD_DIR}/spdlog/include
)

########################################################################
# 构建静态库
add_library(imcore STATIC
    ${MAIN_HEADERS}    ${MAIN_SOURCES}
    ${NETCORE_HEADERS} ${NETCORE_SOURCES}
    ${IMPDU_HEADERS}   ${IMPDU_SOURCES}
    ${UTILITY_HEADERS} ${UTILITY_SOURCES}
    ${SECURITY_HEADERS}${SECURITY_SOURCES}
    ${JSONCPP_HEADERS} ${JSONCPP_SOURCES}
    ${SLOG_HEADERS}    ${SLOG_SOURCES}
    ${PB_HEADERS}      ${PB_SOURCES}
)

# VS 项目文件分组（仅影响 IDE 显示）
source_group("NetCore"   FILES ${NETCORE_HEADERS}  ${NETCORE_SOURCES})
source_group("Protocol"  FILES ${IMPDU_HEADERS}    ${IMPDU_SOURCES})
source_group("Utility"   FILES ${UTILITY_HEADERS}  ${UTILITY_SOURCES})
source_group("Security"  FILES ${SECURITY_HEADERS} ${SECURITY_SOURCES})
source_group("Jsoncpp"   FILES ${JSONCPP_HEADERS}  ${JSONCPP_SOURCES})
source_group("Slog"      FILES ${SLOG_HEADERS}     ${SLOG_SOURCES})
source_group("Generated" FILES ${PB_HEADERS}       ${PB_SOURCES})

# 输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

target_link_libraries(imcore ${PLATFORM_LIBS})

message(STATUS "imcore build config:")
message(STATUS "  Platform : ${CMAKE_SYSTEM_NAME}")
message(STATUS "  spdlog   : ${THIRD_DIR}/spdlog/include")
message(STATUS "  protobuf : ${THIRD_DIR}/protobuf/include")
