cmake_minimum_required(VERSION 4.2.2)
set(CMAKE_CXX_STANDARD 17)
project(cpp-gui-test)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(main 
    src/main.cpp
    src/setup.cpp
    src/renderer.cpp
)

# Compile shaders at build time if glslc is available
find_program(GLSLC glslc)
if(GLSLC)
    set(SHADERS_OUT_DIR ${CMAKE_BINARY_DIR}/shaders)
    file(MAKE_DIRECTORY ${SHADERS_OUT_DIR})

    add_custom_command(
        OUTPUT ${SHADERS_OUT_DIR}/fullscreen.vert.spv
        COMMAND ${GLSLC} ${CMAKE_SOURCE_DIR}/shaders/fullscreen.vert -o ${SHADERS_OUT_DIR}/fullscreen.vert.spv
        DEPENDS ${CMAKE_SOURCE_DIR}/shaders/fullscreen.vert
        COMMENT "Compiling fullscreen.vert -> fullscreen.vert.spv"
    )

    add_custom_command(
        OUTPUT ${SHADERS_OUT_DIR}/balatro.frag.spv
        COMMAND ${GLSLC} ${CMAKE_SOURCE_DIR}/shaders/balatro.frag -o ${SHADERS_OUT_DIR}/balatro.frag.spv
        DEPENDS ${CMAKE_SOURCE_DIR}/shaders/balatro.frag
        COMMENT "Compiling balatro.frag -> balatro.frag.spv"
    )

    add_custom_target(shaders ALL DEPENDS ${SHADERS_OUT_DIR}/fullscreen.vert.spv ${SHADERS_OUT_DIR}/balatro.frag.spv)
    add_dependencies(main shaders)

    target_compile_definitions(main PRIVATE SHADERS_DIR="${SHADERS_OUT_DIR}")
else()
    message(WARNING "glslc not found: shaders will not be compiled automatically. Place SPIR-V files in build/shaders/")
    target_compile_definitions(main PRIVATE SHADERS_DIR="${CMAKE_SOURCE_DIR}/shaders")
endif()

target_sources(main PRIVATE
    include/setup.h
    include/constants.h
    include/debug_callback.h
)

target_include_directories(main PRIVATE include)

target_link_libraries(main PRIVATE Vulkan::Vulkan glfw)