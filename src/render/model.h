/*
 * Created by janis on 2024-01-06
 */

#pragma once

#include <vector>

#include "render/vulkan/vertex.h"

namespace hs::ren {
    class Model {
    public:
        ~Model() = default;

        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;
        Model(Model&& other) = default;
        Model& operator=(Model&& other) { return *this; };

        static Model init(std::vector<vk::Vertex>&& v, std::vector<uint16_t>&& i);

        [[nodiscard]] uint64_t hash() const;
        [[nodiscard]] const std::vector<vk::Vertex>& vertices() const;
        [[nodiscard]] const std::vector<uint16_t>& indices() const;

    private:
        Model() = default;

        std::vector<vk::Vertex> v;
        std::vector<uint16_t> i;
        uint64_t h;
    };
}