/*
 * Created by janis on 2024-01-06
 */

#pragma once

#include <vector>
#include <iterator>

#include "object.h"
#include "render/model.h"
#include "render/hid.h"

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
        [[nodiscard]] const std::vector<Object>& get_objects() const;
        [[nodiscard]] m::Mat4 get_camera() const;

        void update(ren::Hid& hid);

    private:
        std::vector<Object> objects = std::vector<Object>();

        m::Vec3<float> camera_pos = {0, 0, 0};
        m::Vec3<float> camera_rot = {0, 0, 0};
    };
}