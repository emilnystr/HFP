cmake_minimum_required(VERSION 3.15)
project(HFP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0 -Wall -Wextra)
    add_compile_definitions(DEBUG)
else()
    add_compile_options(-O2 -DNDEBUG)
endif()

if(WIN32)
    add_compile_definitions(NOMINMAX)
    add_compile_definitions(_USE_MATH_DEFINES)
endif()

file(GLOB HFP_SOURCES CONFIGURE_DEPENDS src/*.cpp)

set(GLFW_LIB_DIR "${CMAKE_SOURCE_DIR}/Dependencies/GLFW/lib")
set(GLFW_INC_DIR "${CMAKE_SOURCE_DIR}/Dependencies/GLFW/include")

add_executable(hfp ${HFP_SOURCES})

target_include_directories(hfp PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${GLFW_INC_DIR}
)

target_link_libraries(hfp PRIVATE
    ${GLFW_LIB_DIR}/libglfw3.a
    opengl32
)

if(WIN32)
    target_link_libraries(hfp PRIVATE
        gdi32
        shell32
    )
endif()

set_target_properties(hfp PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_NAME hfp
)
