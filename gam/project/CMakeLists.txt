# Global Project Configuration
cmake_minimum_required(VERSION 3.26)
project(OpenGL-Engine)

# Find Packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Sources, Includes & Libraries
set(LIBRARY_SOURCES
    lib/glad/src/glad.c
)

set(SOURCES
    src/Mesh.cpp

    # Assets Module
    src/assets/Camera.cpp
    src/assets/Shader.cpp

    # Engine Module
    src/engine/callbacks.cpp
    src/engine/EventHandler.cpp
    src/engine/Window.cpp

    # Maths Module
    src/maths/functions.cpp
    src/maths/geometry.cpp
    src/maths/mat3.cpp
    src/maths/mat4.cpp
    src/maths/transforms.cpp
    src/maths/trigonometry.cpp

    # Utility Module
    include/utility/ansi.hpp
    include/utility/HeapArray.hpp
    src/utility/gl_enums.cpp
    src/utility/LifetimeLogger.cpp
    src/utility/Random.cpp

    # Libraries
    ${LIBRARY_SOURCES}
)

set(INCLUDES
    include

    # Libraries
    lib/glad/include
    lib/imgui
    lib/imgui/backends
    lib/imgui/misc/cpp
    lib/stb
    lib/tinygltf
)

set(LIBRARIES
    glfw
    dl
    pthread
    X11
    Xxf86vm
    Xrandr
    Xi
)

# Compiler Options
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

set_source_files_properties(${LIBRARY_SOURCES} PROPERTIES COMPILE_FLAGS "-w") # Don't Show Warnings for Library Sources

# CMake Options
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Output compile_commands.json

# Set Directory for Binary Files
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# Executable
add_executable(${PROJECT_NAME} src/main.cpp ${SOURCES} src/applications/Application.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBRARIES})

