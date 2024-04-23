cmake_minimum_required(VERSION 3.23)

option(BUILD_AS_SHARED "Build as dll skse plugin else build as static lib" ON)
set(MAJOR_VERSION 0)
set(MINOR_VERSION 1)

# VCPKG config
string(REPLACE "\\" "/" ENV_VCPKG_ROOT "$ENV{VCPKG_ROOT}")
set(ENV{VCPKG_ROOT} ${ENV_VCPKG_ROOT})

set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
set(VCPKG_TARGET_TRIPLET "x64-windows-static")
set(VCPKG_OVERLAY_TRIPLETS "${CMAKE_CURRENT_LIST_DIR}/overlay_triplets")
set(VCPKG_OVERLAY_PORTS "${CMAKE_CURRENT_LIST_DIR}/overlay_ports")

# Options
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True FORCE)
set(CMAKE_CXX_FLAGS "/EHsc /MP /W4")
#string(REPLACE "/Ob2" "/Ob3" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
#string(REPLACE "/Ob2" "/Ob3" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
add_compile_definitions(WINDOWS)

project(
    NirnLabUIPlatform
    LANGUAGES CXX
    VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.0
)

# Deps
find_package(directxtk CONFIG REQUIRED)
find_package(directxmath CONFIG REQUIRED)

file(
    GLOB_RECURSE
    project_src
        "src/**.h"
        "src/**.hpp"
        "src/**.cpp"
)
source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    FILES ${project_src}
)
list(APPEND project_src ".clang-format")

# Build type
if(BUILD_AS_SHARED)
    # Output
    set(OUTPUT_PATH ${CMAKE_BINARY_DIR}/dist)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_PATH})
    set(CMAKE_GENERATOR_PLATFORM x64)

    add_library("${PROJECT_NAME}" SHARED ${project_src})
else()
    add_library("${PROJECT_NAME}" STATIC ${project_src})
endif()

target_include_directories(
    "${PROJECT_NAME}" 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
    "${PROJECT_NAME}" 
    PUBLIC
        Microsoft::DirectXTK
)
