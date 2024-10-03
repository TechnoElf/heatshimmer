/*
 * Created by janis on 2023-09-30
 */

#pragma once

#include <concepts>
#include <utility>
#include <vector>
#include <tuple>

#include <vulkan/vulkan.h>

#include "math/vec.h"

namespace hs::ren::vk {
    struct Vertex {
        m::Vec2<float> pos;
        m::Vec3<float> col;
        m::Vec2<float> uv;
    };

    template<typename T>
    static constexpr VkVertexInputBindingDescription binding_desc_from_vert_type() noexcept {
        VkVertexInputBindingDescription desc = VkVertexInputBindingDescription();
        desc.binding = 0;
        desc.stride = sizeof(T);
        desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return desc;
    }

    template<typename T>
    static constexpr std::vector<VkVertexInputAttributeDescription> attribute_descs_from_vert_type() noexcept {
        auto descs = std::vector<VkVertexInputAttributeDescription>();

        if constexpr (std::convertible_to<Vertex, T>) {
            descs.emplace_back();
            descs.emplace_back();
            descs.emplace_back();

            descs[0].binding = 0;
            descs[0].location = 0;
            descs[0].format = VK_FORMAT_R32G32_SFLOAT;
            descs[0].offset = offsetof(T, pos);
            descs[1].binding = 0;
            descs[1].location = 1;
            descs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            descs[1].offset = offsetof(T, col);
            descs[2].binding = 0;
            descs[2].location = 2;
            descs[2].format = VK_FORMAT_R32G32_SFLOAT;
            descs[2].offset = offsetof(T, uv);
        }

        return descs;
    }
}