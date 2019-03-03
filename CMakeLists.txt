cmake_minimum_required(VERSION 2.8)

project(openglnoel)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(
        -std=c++14
    )
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(GLMLV_USE_BOOST_FILESYSTEM "Use boost for filesystem library instead of experimental std lib" OFF)
option(GLMLV_USE_ASSIMP "Compile assimp and link glmlv with it" OFF)

set(IMGUI_DIR imgui-1.66b)
set(GLFW_DIR glfw-3.2.1)
set(GLM_DIR glm-0.9.9.3)
set(GLAD_DIR glad)
set(JSON_DIR json-3.5.0)
set(STB_DIR stb)
set(ASSIMP_DIR assimp-4.1.0)
set(TINYGLTF_DIR tiny_gltf)
set(TINYOBJLOADER_DIR tiny_obj_loader)

add_subdirectory(third-party/${GLFW_DIR})

if (GLMLV_USE_ASSIMP)
    add_subdirectory(third-party/${ASSIMP_DIR})
endif()

find_package(OpenGL REQUIRED)

if(GLMLV_USE_BOOST_FILESYSTEM)
    find_package(Boost COMPONENTS system filesystem REQUIRED)
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(c2ba-glsl-shaders)
include(c2ba-copy-dll-post-build)
include(c2ba-assets-directory)

if(MSVC)
    set(SHADER_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/\$\(Configuration\)/shaders)
    set(ASSET_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/\$\(Configuration\)/assets)
else()
    set(SHADER_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/shaders)
    set(ASSET_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/assets)
endif()

file(
    GLOB_RECURSE
    GLMLV_LIB_FILES
    lib/*.cpp lib/*.hpp lib/*.glsl lib/assets/*
)

add_library(
    glmlv
    ${GLMLV_LIB_FILES}
)

target_include_directories(
    glmlv
    PUBLIC
    ${OPENGL_INCLUDE_DIRS}
    third-party/${GLFW_DIR}/include
    third-party/${GLM_DIR}
    third-party/${GLAD_DIR}/include
    third-party/${JSON_DIR}
    third-party/${IMGUI_DIR}
    third-party/${IMGUI_DIR}/examples/
    third-party/${ASSIMP_DIR}/include
    ${CMAKE_BINARY_DIR}/third-party/${ASSIMP_DIR}/include
    third-party/${STB_DIR}
    third-party/${TINYGLTF_DIR}
    third-party/${TINYOBJLOADER_DIR}
    lib/include
)

target_compile_definitions(
    glmlv
    PUBLIC
    IMGUI_IMPL_OPENGL_LOADER_GLAD
    GLM_ENABLE_EXPERIMENTAL
)

if(GLMLV_USE_BOOST_FILESYSTEM)
    include_directories (
        ${Boost_INCLUDE_DIRS}
    )
    target_compile_definitions(
        glmlv
        PUBLIC
        GLMLV_USE_BOOST_FILESYSTEM
    )
endif()

c2ba_add_shader_directory(${CMAKE_CURRENT_SOURCE_DIR}/lib/shaders ${SHADER_OUTPUT_PATH}/glmlv)
c2ba_add_assets_directory(${CMAKE_CURRENT_SOURCE_DIR}/lib/assets ${ASSET_OUTPUT_PATH}/glmlv)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/shaders/
    DESTINATION shaders/glmlv
    FILES_MATCHING PATTERN "*.glsl"
)

set(
    IMGUI_SRC_FILES
    third-party/${IMGUI_DIR}/imgui.cpp
    third-party/${IMGUI_DIR}/imgui_demo.cpp
    third-party/${IMGUI_DIR}/imgui_draw.cpp
    third-party/${IMGUI_DIR}/imgui_widgets.cpp
    third-party/${IMGUI_DIR}/examples/imgui_impl_opengl3.cpp
    third-party/${IMGUI_DIR}/examples/imgui_impl_glfw.cpp
)

set(
    THIRD_PARTY_SRC_FILES
    ${IMGUI_SRC_FILES}
    third-party/${GLAD_DIR}/src/glad.c
)

set(
    LIBRARIES
    ${OPENGL_LIBRARIES}
    glfw
    glmlv
)

if (GLMLV_USE_ASSIMP)
    set(LIBRARIES ${LIBRARIES} assimp)
    target_compile_definitions(
        glmlv
        PUBLIC
        GLMLV_USE_ASSIMP
    )
endif()

if(CMAKE_COMPILER_IS_GNUCXX AND NOT GLMLV_USE_BOOST_FILESYSTEM)
    set(LIBRARIES ${LIBRARIES} stdc++fs)
endif()

if (GLMLV_USE_BOOST_FILESYSTEM)
    set(LIBRARIES ${LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})
endif()

source_group ("glsl" REGULAR_EXPRESSION "*/*.glsl")
source_group ("third-party" REGULAR_EXPRESSION "third-party/*.*")

file(GLOB APP_DIRECTORIES "apps/*")
foreach(DIR ${APP_DIRECTORIES})
    get_filename_component(APP ${DIR} NAME)

    file(
        GLOB_RECURSE
        SRC_FILES
        apps/${APP}/*.cpp apps/${APP}/*.hpp apps/${APP}/*.glsl apps/${APP}/assets/*
    )

    add_executable(
        ${APP}
        ${SRC_FILES}
        ${THIRD_PARTY_SRC_FILES}
    )

    target_link_libraries(
        ${APP}
        ${LIBRARIES}
    )

    install(
        TARGETS ${APP}
        DESTINATION .
    )

    c2ba_add_shader_directory(${DIR}/shaders ${SHADER_OUTPUT_PATH}/${APP})
    c2ba_add_assets_directory(${DIR}/assets ${ASSET_OUTPUT_PATH}/${APP})

    install(
        DIRECTORY ${DIR}/shaders/
        DESTINATION shaders/${APP}
        FILES_MATCHING PATTERN "*.glsl"
    )

    if(EXISTS ${DIR}/assets/)
        install(
            DIRECTORY ${DIR}/assets/
            DESTINATION assets/${APP}
        )
    endif()
endforeach()