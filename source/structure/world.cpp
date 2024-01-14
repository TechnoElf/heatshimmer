/*
 * Created by janis on 2024-01-06
 */

#include "world.h"

namespace hs::str {

    std::vector<std::reference_wrapper<const ren::Model>> World::get_models() const {
        auto vec = std::vector<std::reference_wrapper<const ren::Model>>();
        for (const Object& obj : this->objects) {
            vec.push_back(std::ref(obj.get_model()));
        }
        return vec;
    }

    void World::add_object(Object&& obj) {
        this->objects.push_back(std::move(obj));
    }

    const std::vector<Object>& World::get_objects() const {
        return objects;
    }

    m::Mat4 World::get_camera() const {
        return m::Mat4::translate(-this->camera_pos) *
            m::Mat4::rotz(-this->camera_rot.z) *
            m::Mat4::roty(-this->camera_rot.y) *
            m::Mat4::rotx(-this->camera_rot.x);
    }

    void World::update(ren::Hid& hid) {
        if (hid.is_pressed(SDL_SCANCODE_W)) {
            camera_pos.z += 0.01;
        }

        if (hid.is_pressed(SDL_SCANCODE_S)) {
            camera_pos.z -= 0.01;
        }

        if (hid.is_pressed(SDL_SCANCODE_D)) {
            camera_pos.x += 0.01;
        }

        if (hid.is_pressed(SDL_SCANCODE_A)) {
            camera_pos.x -= 0.01;
        }
    }
}
