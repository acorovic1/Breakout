cmake_minimum_required(VERSION 3.20)
project(Dardaneli LANGUAGES C CXX)


# C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Source files
file(GLOB_RECURSE PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
  
)


file(GLOB_RECURSE SHADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.geom
)

set_source_files_properties(${SHADERS} PROPERTIES LANGUAGE NONE)
source_group("Shaders" FILES ${SHADERS})



include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libraries/include
)




# Create executable
add_executable(Breakout ${PROJECT_SOURCES} ${SHADERS})



# OpenGL
find_package(OpenGL REQUIRED)

target_link_libraries(Breakout PRIVATE  OpenGL::GL)

# GLFW (precompiled)
target_link_libraries(Breakout PRIVATE
    ${CMAKE_SOURCE_DIR}/libraries/lib/glfw3.lib
)

# Windows system libraries required by GLFW
if (WIN32)
    target_link_libraries(Breakout PRIVATE
        opengl32
        user32
        gdi32
        shell32
    )
endif()


# Compiler warnings
if (MSVC)
    target_compile_options(Breakout PRIVATE 
    /W4 
    /MP 
    /bigobj 
    /wd4201   # GLM warning
    /permissive-
    )
else()
    target_compile_options(Breakout PRIVATE -Wall -Wextra -pedantic)
endif()
