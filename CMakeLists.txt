
cmake_minimum_required(VERSION 3.15)
project(HFP LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0 -Wall -Wextra)
    add_compile_definitions(DEBUG)
    message(STATUS "DEBUG mode: gdb symbols enabled")
else()
    add_compile_options(-O2 -DNDEBUG)
    message(STATUS "RELEASE mode: optimizations enabled")
endif()

if(WIN32)
    add_compile_definitions(NOMINMAX)
    add_compile_definitions(_USE_MATH_DEFINES)  # För M_PI etc
    message(STATUS "Windows build detected")
endif()


set(HFP_SOURCES
    main.cpp
    config.cpp
    enthalpy.cpp
    enthalpy_solver.cpp
    fire.cpp
    layer.cpp
    material.cpp
    matrix.cpp
    matrix_solver.cpp
    mesh.cpp
    openGL_plotter.cpp
    solver.cpp
)

message(STATUS "Compiling ${HFP_SOURCES}")


set(GLFW_LIB_DIR "${CMAKE_SOURCE_DIR}/Dependencies/GLFW/lib")
set(GLFW_INC_DIR "${CMAKE_SOURCE_DIR}/Dependencies/GLFW/include")

if(NOT EXISTS "${GLFW_LIB_DIR}/libglfw3.a")
    message(FATAL_ERROR "GLFW library not found at: ${GLFW_LIB_DIR}/libglfw3.a")
endif()

if(NOT EXISTS "${GLFW_INC_DIR}/GLFW/glfw3.h")
    message(FATAL_ERROR "GLFW headers not found at: ${GLFW_INC_DIR}/GLFW/glfw3.h")
endif()

message(STATUS "GLFW found: ${GLFW_INC_DIR}")


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
        shell32  
    )
endif()

# Spara exe i rotmappen (som ditt gamla sätt)
set_target_properties(hfp PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_NAME hfp
)