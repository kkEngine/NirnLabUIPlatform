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
set(CMAKE_GENERATOR_PLATFORM x64)

# Deps
find_package(directxtk CONFIG REQUIRED)
find_package(directxmath CONFIG REQUIRED)

# Paths
if (NOT DEFINED OUTPUT_PATH)
    set(OUTPUT_PATH ${CMAKE_BINARY_DIR}/dist/$<CONFIG>)
endif()
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_PATH})

set(SKSE_PLUGIN_REL_PATH "Data/SKSE/Plugins")
set(SKSE_PLUGIN_PATH ${OUTPUT_PATH}/${SKSE_PLUGIN_REL_PATH})

if(NOT DEFINED NL_UI_REL_PATH)
    set(NL_UI_REL_PATH "UI")
endif()
set(NL_UI_PATH ${OUTPUT_PATH}/${NL_UI_REL_PATH})

# ============================ CEF LIB ============================
add_library(cef INTERFACE)
find_library(CEF_LIBRARY_DEBUG libcef_dll_wrapper)
string(REPLACE "/debug/lib/" "/lib/" CEF_LIBRARY_RELEASE ${CEF_LIBRARY_DEBUG})
find_path(CEF_INCLUDE_DIR include/cef_app.h)
find_path(CEF_SHARE share/cef-prebuilt)
set(CEF_SHARE "${CEF_SHARE}/share/cef-prebuilt")
target_link_libraries(cef INTERFACE "$<IF:$<CONFIG:Debug>,${CEF_LIBRARY_DEBUG},${CEF_LIBRARY_RELEASE}>")
target_link_libraries(cef INTERFACE "${CEF_SHARE}/$<CONFIG>/cef_sandbox.lib")
target_link_libraries(cef INTERFACE "${CEF_SHARE}/$<CONFIG>/libcef.lib")
target_include_directories(cef INTERFACE ${CEF_INCLUDE_DIR})
set(cef_files_path "${CEF_SHARE}/$<IF:$<CONFIG:Debug>,Debug,Release>")
add_custom_target(
    CefLibraryFiles
    # Cef files
    COMMAND ${CMAKE_COMMAND} -E make_directory                                                  ${NL_UI_PATH}/
    COMMAND ${CMAKE_COMMAND} -E copy ${cef_files_path}/chrome_elf.dll                           ${NL_UI_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy ${cef_files_path}/d3dcompiler_47.dll                       ${NL_UI_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy ${cef_files_path}/libcef.dll                               ${NL_UI_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy ${cef_files_path}/libEGL.dll                               ${NL_UI_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy ${cef_files_path}/libGLESv2.dll                            ${NL_UI_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy ${cef_files_path}/snapshot_blob.bin                        ${NL_UI_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy ${cef_files_path}/v8_context_snapshot.bin                  ${NL_UI_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy ${cef_files_path}/vk_swiftshader.dll                       ${NL_UI_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy ${cef_files_path}/vk_swiftshader_icd.json                  ${NL_UI_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy ${cef_files_path}/vulkan-1.dll                             ${NL_UI_PATH}
    COMMAND ${CMAKE_COMMAND} -E copy ${cef_files_path}/../Resources/icudtl.dat                  ${NL_UI_PATH}
    COMMAND ${CMAKE_COMMAND} -E make_directory                                                  ${NL_UI_PATH}/CEF
    COMMAND ${CMAKE_COMMAND} -E copy ${cef_files_path}/../Resources/chrome_100_percent.pak      ${NL_UI_PATH}/CEF
    COMMAND ${CMAKE_COMMAND} -E copy ${cef_files_path}/../Resources/chrome_200_percent.pak      ${NL_UI_PATH}/CEF
    COMMAND ${CMAKE_COMMAND} -E copy ${cef_files_path}/../Resources/resources.pak               ${NL_UI_PATH}/CEF
    COMMAND ${CMAKE_COMMAND} -E make_directory                                                  ${NL_UI_PATH}/CEF/locales
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${cef_files_path}/../Resources/locales           ${NL_UI_PATH}/CEF/locales
    # Output Message
    COMMENT "Copying cef files..." VERBATIM
)

# ============================ CEF Subprocess ============================
set(CEFSubprocessProjectName "CEFSubprocess")
file(
    GLOB_RECURSE
    CEFSubprocessProjectName_src "src/CEFSubprocess/*"
)
source_group(" " FILES ${CEFSubprocessProjectName_src})
list(APPEND CEFSubprocessProjectName_src ".clang-format")
add_executable(
    ${CEFSubprocessProjectName}
    WIN32
        ${CEFSubprocessProjectName_src}
)
target_link_libraries(
    ${CEFSubprocessProjectName}
    PUBLIC
        cef
)
set_target_properties(
    ${CEFSubprocessProjectName}
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${NL_UI_PATH}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${NL_UI_PATH}
        LIBRARY_OUTPUT_DIRECTORY_DEBUG ${NL_UI_PATH}
        LIBRARY_OUTPUT_DIRECTORY_RELEASE ${NL_UI_PATH}
        #ARCHIVE_OUTPUT_DIRECTORY ${NL_UI_PATH}/archive
        #PDB_OUTPUT_DIRECTORY ${NL_UI_PATH}/pdb
)
add_dependencies(${CEFSubprocessProjectName} CefLibraryFiles)

# ============================ Platform ============================
set(PlatformProjectName "NirnLabUIPlatform")
file(
    GLOB_RECURSE
    PlatformProjectName_src
        "src/Platform/**.h"
        "src/Platform/**.hpp"
        "src/Platform/**.cpp"
)
source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}/src/Platform"
    FILES ${PlatformProjectName_src}
)
list(APPEND PlatformProjectName_src ".clang-format")

# Build type
if(BUILD_AS_SHARED)
    add_library(${PlatformProjectName} SHARED ${PlatformProjectName_src})
    set_target_properties(
        ${PlatformProjectName}
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_DEBUG ${SKSE_PLUGIN_PATH}
            RUNTIME_OUTPUT_DIRECTORY_RELEASE ${SKSE_PLUGIN_PATH}
            LIBRARY_OUTPUT_DIRECTORY_DEBUG ${SKSE_PLUGIN_PATH}
            LIBRARY_OUTPUT_DIRECTORY_RELEASE ${SKSE_PLUGIN_PATH}
            #ARCHIVE_OUTPUT_DIRECTORY ${SKSE_PLUGIN_PATH}/archive
            #PDB_OUTPUT_DIRECTORY ${SKSE_PLUGIN_PATH}/pdb
)
else()
    add_library(${PlatformProjectName} STATIC ${PlatformProjectName_src})
endif()

target_include_directories(
    ${PlatformProjectName}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Platform
)

target_link_libraries(
    ${PlatformProjectName}
    PUBLIC
        Microsoft::DirectXTK
)
