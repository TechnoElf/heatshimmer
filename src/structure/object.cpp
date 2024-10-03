/*
 * Created by janis on 2024-01-06
 */

#include "object.h"

namespace hs::str {
    const ren::Model& Object::get_model() const {
        return this->model;
    }
    m::Mat4 Object::get_transform() const {
        return m::Mat4::translate(this->position) *
            m::Mat4::rotz(this->rotation.z) *
            m::Mat4::roty(this->rotation.y) *
            m::Mat4::rotx(this->rotation.x) *
            m::Mat4::scale(this->scale);
    }
}
