cmake_minimum_required(VERSION 2.6)
project(network)

aux_source_directory(. SRC_LIST1)
aux_source_directory(./impdu SRC_LIST2)
aux_source_directory(./pbgen SRC_LIST3)

set(THIRD_DIR ./third)

set(PB_LIB_DIR ${THIRD_DIR}/protobuf/lib)
set(SLOG_LIB_DIR ${THIRD_DIR}/slog/lib)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

add_definitions(
    -g -W -Wall
    -D_REENTRANT
    -D_FILE_OFFSET_BITS=64
    -DAC_HAS_INFO
    -DAC_HAS_WARNING
    -DAC_HAS_ERROR
    -DAC_HAS_CRITICAL
    -DAC_HAS_DEBUG
    -DTIXML_USE_STL
    -DLINUX_DAEMON)

include_directories(
    ./
    ./impdu
    ./pbgen
    ${THIRD_DIR}/slog/include
    ${THIRD_DIR}/protobuf/include)

link_directories(
    ${PB_LIB_DIR}
    ${SLOG_LIB_DIR})

add_library(network STATIC
    ${SRC_LIST1}
    ${SRC_LIST2}
    ${SRC_LIST3})

target_link_libraries(network protobuf slog)
