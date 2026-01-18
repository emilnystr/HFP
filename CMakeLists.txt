cmake_minimum_required(VERSION 3.15)
project(HFP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0 -Wall -Wextra)
    add_compile_definitions(DEBUG)
else()
    add_compile_options(-O2 -DNDEBUG)
endif()



file(GLOB HFP_SOURCES CONFIGURE_DEPENDS src/*.cpp)

set(GLFW_LIB_DIR "${CMAKE_SOURCE_DIR}/Dependencies/GLFW/lib")
set(GLFW_INC_DIR "${CMAKE_SOURCE_DIR}/Dependencies/GLFW/include")

add_executable(hfp ${HFP_SOURCES})

target_include_directories(hfp PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${GLFW_INC_DIR}
)

target_link_libraries(hfp PRIVATE
    ${GLFW_LIB_DIR}/libglfw3.a
    opengl32
)

if(WIN32)
    target_link_libraries(hfp PRIVATE
        gdi32
    )
endif()

set_target_properties(hfp PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_NAME hfp
)
