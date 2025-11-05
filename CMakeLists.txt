cmake_minimum_required(VERSION 3.10)
project(network)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#################################################################################
# 头文件包含
include_directories(
    src/
    src/impdu
    src/pbgen
    third/protobuf/include
)

########################################################################
# 平台差异化处理
if(WIN32)
    message(STATUS "Configuring for Windows")
    set(PLATFORM_WINDOWS 1)
    add_definitions(
        -DWIN32
        -D_WINDOWS
        -D_CRT_SECURE_NO_WARNINGS
        -D_WINSOCK_DEPRECATED_NO_WARNINGS
        -DTIXML_USE_STL
    )
    # Windows头文件包含
    include_directories(
        third/yaolog
    )
    # Windows源文件
    file(GLOB PB_SOURCES "src/pbgen/*.cc")
    file(GLOB YAOLOG_SOURCES "third/yaolog/*.cpp")
    # Windows编译选项
    if(MSVC)
        # 禁用MSVC特定警告
        add_compile_options(/W3)
    endif()
    # Windows特定库
    set(PLATFORM_LIBS protobuf-lite)
else()
    message(STATUS "Configuring for Linux")
    set(PLATFORM_LINUX 1)
    add_definitions(
        -DLINUX
        -D_UNIX
        -D_REENTRANT
        -D_FILE_OFFSET_BITS=64
        -DAC_HAS_INFO
        -DAC_HAS_WARNING
        -DAC_HAS_ERROR
        -DAC_HAS_CRITICAL
        -DAC_HAS_DEBUG
        -DTIXML_USE_STL
        -DLINUX_DAEMON
    )
    # Linux头文件包含
    include_directories(
        third/slog/include
    )
    # Liuux源文件
    file(GLOB PB_SOURCES "src/pbgen/*.cpp")
    file(GLOB YAOLOG_SOURCES "")
    # Linux编译选项
    add_compile_options(-g -W -Wall)
    # Linux特定库
    set(PLATFORM_LIBS protobuf-lite slog)
endif()

#################################################################################
# 收集主源码
aux_source_directory(src SRC_LIST1)
aux_source_directory(src/impdu SRC_LIST2)

# 设置输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# 创建库目标
add_library(network STATIC
    ${SRC_LIST1}
    ${SRC_LIST2}
    ${PB_SOURCES}
    ${YAOLOG_SOURCES}   # Windows下包含yaolog源代码，Linux下为空
)

# 链接库
target_link_libraries(network
    ${PLATFORM_LIBS}
)

# 输出配置信息
message(STATUS "Configuration complete:")
message(STATUS "  - Platform: ${CMAKE_SYSTEM_NAME}")
if(WIN32)
    message(STATUS "  - Using yaolog (source): ${YAOLOG_SOURCES}")
else()
    message(STATUS "  - Using slog (static library)")
endif()

