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
        source/main.cpp
        source/render/hid.cpp
        source/render/hid.h
        source/render/renderer.cpp
        source/render/renderer.h
        source/render/util.h
        source/render/util.cpp
        source/render/vulkan/instance.cpp
        source/render/vulkan/instance.h
        source/render/vulkan/device.cpp
        source/render/vulkan/device.h
        source/render/vulkan/pipeline.cpp
        source/render/vulkan/pipeline.h
        source/render/vulkan/swapchain.h
        source/render/vulkan/swapchain.cpp
        source/math/vec.cpp
        source/math/vec.h
        source/render/vulkan/vertex.h
        source/math/mat.cpp
        source/math/mat.h
        source/render/vulkan/uniform.h
        source/file/netpbm.cpp
        source/file/netpbm.h
        source/render/vulkan/cmd_buffer.cpp
        source/render/vulkan/cmd_buffer.h
        source/render/error.cpp
        source/render/error.h
        source/structure/world.cpp
        source/structure/world.h
        source/render/model.cpp
        source/render/model.h
        source/structure/object.cpp
        source/structure/object.h
        source/math/hash.cpp
        source/math/hash.h
)

include_directories(source)

# C++ dependencies

find_package(SDL2 REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

find_package(Vulkan REQUIRED)
message(STATUS "Vulkan_INCLUDE_DIRS=${Vulkan_INCLUDE_DIRS}")
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)

# Shaders

add_shader(${PROJECT_NAME} source/shader/base.frag)
add_shader(${PROJECT_NAME} source/shader/base.vert)

# Options

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror)
