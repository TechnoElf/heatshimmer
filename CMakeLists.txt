cmake_minimum_required(VERSION 3.25.3)

project(heatshimmer)

set(CMAKE_CXX_STANDARD 20)

find_program(GLSLC glslc)
function(add_shader TARGET SHADER)
    set(current-shader-path ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER})

    get_filename_component(current-output-file ${SHADER}.spv NAME)
    set(current-output-path ${CMAKE_BINARY_DIR}/${current-output-file})
    get_filename_component(current-output-dir ${current-output-path} DIRECTORY)
    file(MAKE_DIRECTORY ${current-output-dir})

    add_custom_command(
        OUTPUT ${current-output-path}
        COMMAND ${GLSLC} -o ${current-output-path} ${current-shader-path}
        DEPENDS ${current-shader-path}
        IMPLICIT_DEPENDS CXX ${current-shader-path}
        VERBATIM
    )

    set_source_files_properties(${current-output-path} PROPERTIES GENERATED TRUE)
    target_sources(${TARGET} PRIVATE ${current-output-path})
endfunction(add_shader)

# C++ sources

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/render/hid.cpp
    src/render/hid.h
    src/render/renderer.cpp
    src/render/renderer.h
    src/render/util.h
    src/render/util.cpp
    src/render/vulkan/instance.cpp
    src/render/vulkan/instance.h
    src/render/vulkan/device.cpp
    src/render/vulkan/device.h
    src/render/vulkan/pipeline.cpp
    src/render/vulkan/pipeline.h
    src/render/vulkan/swapchain.h
    src/render/vulkan/swapchain.cpp
    src/math/vec.cpp
    src/math/vec.h
    src/render/vulkan/vertex.h
    src/math/mat.cpp
    src/math/mat.h
    src/render/vulkan/uniform.h
    src/file/netpbm.cpp
    src/file/netpbm.h
    src/render/vulkan/cmd_buffer.cpp
    src/render/vulkan/cmd_buffer.h
    src/render/error.cpp
    src/render/error.h
    src/structure/world.cpp
    src/structure/world.h
    src/render/model.cpp
    src/render/model.h
    src/structure/object.cpp
    src/structure/object.h
    src/math/hash.cpp
    src/math/hash.h
)

include_directories(src)

# C++ dependencies

find_package(SDL2 REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

find_package(Vulkan REQUIRED)
message(STATUS "Vulkan_INCLUDE_DIRS=${Vulkan_INCLUDE_DIRS}")
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)

# Shaders

add_shader(${PROJECT_NAME} src/shader/base.frag)
add_shader(${PROJECT_NAME} src/shader/base.vert)

# Options

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror -Wpedantic)
