cmake_minimum_required(VERSION 3.16)  # CMake version check
project(zengine)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(glog CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

if(WIN32)
    find_package(dlfcn-win32 CONFIG REQUIRED)
    set(CMAKE_DL_LIBS dlfcn-win32::dl)
endif(WIN32)

include(FetchContent)

FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb
  GIT_TAG 5736b15f7ea0ffb08dd38af21067c314d6a3aae9
)

FetchContent_MakeAvailable(
    stb
)

################################################################################
file(GLOB_RECURSE ZENGINE_SOURCES src/**.cpp)
add_library(zengine SHARED ${ZENGINE_SOURCES})
target_link_libraries(zengine PRIVATE
    ${CMAKE_DL_LIBS}
    Threads::Threads
    stdc++fs
    fmt::fmt
    glog::glog
    OpenGL::GL
    GLEW::GLEW
    glfw
)
target_include_directories(zengine PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/include
    ${CMAKE_SOURCE_DIR}/include
    ${stb_SOURCE_DIR}
)

################################################################################
file(GLOB_RECURSE EXAMPLES_SOURCES examples/**.cpp)
foreach(EXAMPLE_SOURCE ${EXAMPLES_SOURCES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE})
    target_link_libraries(${EXAMPLE_NAME} zengine)
    target_include_directories(${EXAMPLE_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
endforeach()
