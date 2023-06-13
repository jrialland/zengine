cmake_minimum_required(VERSION 3.16)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project(zengine)
set(CMAKE_CXX_STANDARD 20)

################################################################################
# libraries installed by vcpkg are visible by all targets
include_directories(
    ${Backtrace_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/include
)

################################################################################
# project dependencies, see vcpkg settings
find_package(Catch2 CONFIG REQUIRED)
find_package(Backtrace REQUIRED)
find_package(Threads REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

################################################################################
# linux specific stuff
if(UNIX)

    find_package(Libunwind REQUIRED)
    list(APPEND ZENGINE_LIBRARIES ${LIBUNWIND_LIBRARIES})

    find_package(LibBfd REQUIRED)
    list(APPEND ZENGINE_LIBRARIES ${LIBBFD_LIBRARIES})
    set(BACKWARD_HAS_BFD ON)

    list(APPEND ZENGINE_LIBRARIES ${CMAKE_DL_LIBS})

    find_library(LIBRT rt)
    if(LIBRT)
        list(APPEND ZENGINE_LIBRARIES ${LIBRT})
    endif()

endif()

################################################################################
# some libraries are such as stb are not available in vcpkg
include(FetchContent)

FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb
  GIT_TAG 5736b15f7ea0ffb08dd38af21067c314d6a3aae9
)

FetchContent_Declare(
    backward
    GIT_REPOSITORY https://github.com/bombela/backward-cpp
    GIT_TAG v1.6
)

FetchContent_MakeAvailable(
    stb
    backward
)

################################################################################
# IPO support

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT error)

################################################################################
# libraries used by zengine
set(ZENGINE_LIBRARIES
    Threads::Threads
    ${Backtrace_LIBRARY}
    stdc++fs
    fmt::fmt
    OpenGL::GL
    GLEW::GLEW
    glfw
)

################################################################################
# windows specific stuff
if(WIN32)
    # add dlfcn-win32, simulates dlopen, dlsym, dlclose and dlerror on Windows
    find_package(dlfcn-win32 CONFIG REQUIRED)
    list(APPEND ZENGINE_LIBRARIES dlfcn-win32::dl)

    # sockets
    list(APPEND ZENGINE_LIBRARIES wsock32 ws2_32)

endif(WIN32)

################################################################################
# OpenMP support
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    list(APPEND ZENGINE_LIBRARIES OpenMP::OpenMP_CXX)
endif()

################################################################################
# build zengine library
file(GLOB_RECURSE ZENGINE_SOURCES CONFIGURE_DEPENDS src/**.cpp)

add_library(zengine SHARED ${ZENGINE_SOURCES})
target_link_libraries(zengine PRIVATE ${ZENGINE_LIBRARIES})
target_include_directories(zengine PRIVATE
    ${stb_SOURCE_DIR}
    ${BACKWARD_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
)

if(UNIX)
    target_link_libraries(zengine PRIVATE 
    ${BACKWARD_LIBRARIES}
    )
endif()

if(ipo_supported)
    set_target_properties(zengine PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

################################################################################
# copy assets directory to build directory (used by examples)
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
    if(WIN32)
        # non console applications on windows (CMAKE_WIN32_EXECUTABLE)
        set_target_properties(${EXAMPLE_NAME} PROPERTIES CMAKE_WIN32_EXECUTABLE ON)
    endif()
endforeach()

################################################################################
# build unit tests
file(GLOB_RECURSE TESTS_SOURCES tests/**.cpp)
add_executable(tests ${TESTS_SOURCES} ${ZENGINE_SOURCES})
target_link_libraries(tests PRIVATE ${ZENGINE_LIBRARIES} Catch2::Catch2)

if(UNIX)
    target_link_libraries(tests PRIVATE 
    ${BACKWARD_LIBRARIES}
    )
endif()

target_include_directories(tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${stb_SOURCE_DIR}
    ${BACKWARD_INCLUDE_DIRS}
)

# catch2 / ctest integration
include(CTest)
include(Catch)
catch_discover_tests(tests)

################################################################################
# a function that dumps all variables
function(dump_all_variables)
    get_cmake_property(_variableNames VARIABLES)
    list(SORT _variableNames)
    foreach(_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endfunction()

#dump_all_variables()