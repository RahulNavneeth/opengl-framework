cmake_minimum_required(VERSION 3.12)
project(OpenGLProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(VENDOR_DIR ${CMAKE_SOURCE_DIR}/vendor)

find_package(OpenGL REQUIRED)

find_package(GLEW REQUIRED)

set(GLEW_LIB_PATH "/usr/local/lib" CACHE PATH "Path to GLEW library")
set(GLEW_INCLUDE_PATH "/usr/local/include" CACHE PATH "Path to GLEW headers")
set(GLFW_PATH ${CMAKE_SOURCE_DIR}/vendor/glfw-3.4/build/src)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${VENDOR_DIR}/glad/include)
include_directories(${VENDOR_DIR}/glm)
include_directories(${GLEW_INCLUDE_PATH})
include_directories(${CMAKE_SOURCE_DIR}/vendor/glfw-3.4/include)

add_executable(main main.cpp src/OpenGLWrapper.cpp ${VENDOR_DIR}/glad/src/gl.c)

target_link_libraries(main OpenGL::GL GLEW::GLEW ${GLFW_PATH}/libglfw3.a)

if (APPLE)
    target_link_libraries(main "-framework IOKit" "-framework Cocoa" "-framework OpenGL")
endif()
