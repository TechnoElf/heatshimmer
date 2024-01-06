/*
 * Created by janis on 2024-01-06
 */

#pragma once

#include <vector>

#include "object.h"
#include "render/model.h"


namespace hs::str {
    class World {
    public:
        World() = default;
        ~World() = default;

        World(const World&) = delete;
        World& operator=(const World&) = delete;
        World(World&& other) = delete;
        World& operator=(World&& other) = delete;

        void add_object(Object&& obj);

        [[nodiscard]] std::vector<std::reference_wrapper<const ren::Model>> get_models() const;

    private:
        std::vector<Object> objects = std::vector<Object>();
    };
}