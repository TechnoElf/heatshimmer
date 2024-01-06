/*
 * Created by janis on 2024-01-06
 */

#pragma once

#include <algorithm>

#include "render/model.h"

namespace hs::str {
    class Object {
    public:
        Object(ren::Model model) : model(std::move(model)) {}
        ~Object() = default;

        Object(const Object&) = delete;
        Object& operator=(const Object&) = delete;
        Object(Object&& other) = default;
        Object& operator=(Object&& other) = default;

        [[nodiscard]] const ren::Model& get_model() const;
    private:
        ren::Model model;
    };
}