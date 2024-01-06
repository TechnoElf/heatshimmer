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
}
