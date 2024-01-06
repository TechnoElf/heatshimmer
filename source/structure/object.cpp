/*
 * Created by janis on 2024-01-06
 */

#include "object.h"

namespace hs::str {
    const ren::Model& Object::get_model() const {
        return this->model;
    }
}
