cmake_minimum_required(VERSION 3.16)  # CMake version check
project(zengine)
set(CMAKE_CXX_STANDARD 20)

################################################################################
# libraries installed by vcpkg are visible by all targets
include_directories(${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/include)
#link_directories(${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/lib)

################################################################################
# project dependencies, see vcpkg settings
find_package(Threads REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Freetype REQUIRED)

# vcpkg does not provide a classic cmake config file, so we need to use find_path and find_library
find_path(CAIRO_INCLUDE_DIRS cairo/cairo.h)
find_library(CAIRO_LIBRARIES cairo)

################################################################################
# windows specific settings
if(WIN32)

    # build non-console apps
    set(CMAKE_WIN32_EXECUTABLE ON)

    # add dlfcn-win32, simulates dlopen, dlsym, dlclose and dlerror on Windows
    find_package(dlfcn-win32 CONFIG REQUIRED)
    set(CMAKE_DL_LIBS dlfcn-win32::dl)

endif(WIN32)

################################################################################
# some libraries are such as stb are not available in vcpkg
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
# build zengine library
file(GLOB_RECURSE ZENGINE_SOURCES src/**.cpp)
add_library(zengine SHARED ${ZENGINE_SOURCES})
target_link_libraries(zengine PRIVATE
    ${CMAKE_DL_LIBS}
    Threads::Threads
    stdc++fs
    fmt::fmt
    OpenGL::GL
    GLEW::GLEW
    glfw
    Freetype::Freetype
    ${CAIRO_LIBRARIES}
)
if(WIN32)
  target_link_libraries(zengine PRIVATE wsock32 ws2_32)
endif()

target_include_directories(zengine PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${stb_SOURCE_DIR}
)

################################################################################
# copy assets directory to build directory
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_BINARY_DIR}/assets
    DEPENDS ${CMAKE_SOURCE_DIR}/assets
)
add_dependencies(zengine copy_assets)

################################################################################
# build examples
file(GLOB_RECURSE EXAMPLES_SOURCES examples/**.cpp)
foreach(EXAMPLE_SOURCE ${EXAMPLES_SOURCES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE})
    target_link_libraries(${EXAMPLE_NAME} zengine)
    target_include_directories(${EXAMPLE_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
endforeach()
