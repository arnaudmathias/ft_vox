cmake_minimum_required(VERSION 3.5)
project(ft_vox)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

find_package(OpenGL REQUIRED)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(third-party/glfw)
include_directories(third-party/glfw/include)
include_directories(third-party/glad)
include_directories(third-party/glm)
include_directories(third-party/stb)
include_directories(third-party/tinyobjloader)
include_directories(third-party)
include_directories(${OPENGL_INCLUDE_DIR})

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(SOURCE_FILES
        src/main.cpp
        src/env.cpp
        src/chunk.cpp
        src/shader.cpp
        src/renderer.cpp
        src/camera.cpp
        third-party/glad/glad.c)

add_executable(ft_vox ${SOURCE_FILES})

target_link_libraries(ft_vox glfw ${GLFW_LIBRARIES})
target_link_libraries(ft_vox ${OPENCL_LIBRARIES})
