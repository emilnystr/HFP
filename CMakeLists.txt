cmake_minimum_required(VERSION 3.15)
project(HFP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Optimization: DEBUG (O0)")
    add_compile_options(-g -O0 -Wall -Wextra -Wshadow)
    add_compile_definitions(DEBUG _DEBUG)

elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Optimization: MAX SPEED (O3)")
    add_compile_options(
        -O3
        -march=native
        -ffast-math
        -funroll-loops
        -flto
        -DNDEBUG
        -fomit-frame-pointer
    )
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        add_compile_options(
            -fdevirtualize-speculatively
            -fno-semantic-interposition
        )
    endif()
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    message(STATUS "Optimization: BALANCED (O2 with debug info)")
    add_compile_options(-O2 -g -DNDEBUG)

elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    message(STATUS "Optimization: MINIMAL SIZE (Os)")
    add_compile_options(-Os -DNDEBUG)
endif()

file(GLOB HFP_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")

list(LENGTH HFP_SOURCES NUM_SOURCES)
if(NUM_SOURCES EQUAL 0)
    message(WARNING "No .cpp files found in src/ directory!")
    file(GLOB HFP_SOURCES CONFIGURE_DEPENDS "*.cpp")
endif()

set(GLFW_LIB_DIR "${CMAKE_SOURCE_DIR}/Dependencies/GLFW/lib")
set(GLFW_INC_DIR "${CMAKE_SOURCE_DIR}/Dependencies/GLFW/include")

add_executable(hfp ${HFP_SOURCES})

target_include_directories(hfp PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${GLFW_INC_DIR}"
)

if(WIN32)
    add_compile_definitions(
        NOMINMAX
        _USE_MATH_DEFINES
    )
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_link_options(hfp PRIVATE
            -static
            -static-libgcc
            -static-libstdc++
        )
    endif()
endif()

target_link_libraries(hfp PRIVATE
    "${GLFW_LIB_DIR}/libglfw3.a"
    opengl32
)

if(WIN32)
    target_link_libraries(hfp PRIVATE
        gdi32
        shell32
    )
endif()

set_target_properties(hfp PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_NAME "hfp"
)
