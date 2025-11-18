cmake_minimum_required(VERSION 3.10)
project(network)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(THIRD_DIR third)

# 更精确地收集文件
file(GLOB_RECURSE MAIN_HEADERS "src/*.h" "src/*.hpp")
file(GLOB_RECURSE MAIN_SOURCES "src/*.cpp" "src/*.cc" "src/*.c")
file(GLOB_RECURSE NETCORE_HEADERS "src/core/*.h" "src/core/*.hpp")
file(GLOB_RECURSE NETCORE_SOURCES "src/core/*.cpp" "src/core/*.cc" "src/core/*.c")
file(GLOB_RECURSE UTILITY_HEADERS "src/utility/*.h" "src/utility/*.hpp")
file(GLOB_RECURSE UTILITY_SOURCES "src/utility/*.cpp" "src/utility/*.cc" "src/utility/*.c")
file(GLOB_RECURSE IMPDU_HEADERS "src/impdu/*.h" "src/impdu/*.hpp")
file(GLOB_RECURSE IMPDU_SOURCES "src/impdu/*.cpp" "src/impdu/*.cc" "src/impdu/*.c")
file(GLOB_RECURSE PB_HEADERS "src/pbgen/*.h" "src/pbgen/*.hpp")
file(GLOB_RECURSE PB_SOURCES "src/pbgen/*.cpp" "src/pbgen/*.cc" "src/pbgen/*.c")

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
        -DYAOLOG_EXPORTS
    )
    set(LOG_DIR ${THIRD_DIR}/yaolog)
    # Windows源文件
    file(GLOB YAOLOG_HEADERS "third/yaolog/*.h")
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
    set(LOG_DIR ${THIRD_DIR}/slog/include)
    # Liuux源文件
    file(GLOB YAOLOG_HEADERS "")
    file(GLOB YAOLOG_SOURCES "")
    # Linux编译选项
    add_compile_options(-g -W -Wall)
    # Linux特定库
    set(PLATFORM_LIBS protobuf-lite slog)
endif()

#################################################################################
# 头文件包含
include_directories(
    src/
    src/core
    src/impdu
    src/pbgen
    src/utility
    third/protobuf/include
    ${LOG_DIR}
)

#################################################################################
# 收集主源码
#aux_source_directory(src SRC_LIST1)
#aux_source_directory(src/impdu SRC_LIST2)
# 创建库目标
add_library(network STATIC
    ${MAIN_HEADERS} ${MAIN_SOURCES}
    ${NETCORE_HEADERS} ${NETCORE_SOURCES}
    ${UTILITY_HEADERS} ${UTILITY_SOURCES}
    ${IMPDU_HEADERS} ${IMPDU_SOURCES}
    ${PB_HEADERS} ${PB_SOURCES}
    ${YAOLOG_HEADERS} ${YAOLOG_SOURCES}
)

# 设置分类
source_group("Generated" FILES ${PB_SOURCES})
source_group("Generated" FILES ${PB_HEADERS})
source_group("NetCore" FILES ${NETCORE_HEADERS})
source_group("NetCore" FILES ${NETCORE_SOURCES})
source_group("Utility" FILES ${UTILITY_HEADERS})
source_group("Utility" FILES ${UTILITY_SOURCES})
source_group("Protocol" FILES ${IMPDU_HEADERS})
source_group("Protocol" FILES ${IMPDU_SOURCES})
source_group("Header Files" FILES ${MAIN_HEADERS})
source_group("Source Files" FILES ${MAIN_SOURCES})
if(WIN32)
    source_group("Yaolog" FILES ${YAOLOG_SOURCES})
    source_group("Yaolog" FILES ${YAOLOG_HEADERS})
endif()

# 设置输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

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

