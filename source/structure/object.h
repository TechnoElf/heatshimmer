/*
 * Created by janis on 2024-01-06
 */

#pragma once

#include <algorithm>

#include "render/model.h"
#include "math/mat.h"

namespace hs::str {
    class Object {
    public:
        m::Vec3<float> position = {0, 0, 0};
        m::Vec3<float> rotation = {0, 0, 0};
        m::Vec3<float> scale = {1, 1, 1};

        Object(ren::Model model) : model(std::move(model)) {}
        ~Object() = default;

        Object(const Object&) = delete;
        Object& operator=(const Object&) = delete;
        Object(Object&& other) = default;
        Object& operator=(Object&& other) = default;

        [[nodiscard]] const ren::Model& get_model() const;
        [[nodiscard]] m::Mat4 get_transform() const;
    private:
        ren::Model model;
    };
}