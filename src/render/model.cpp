/*
 * Created by janis on 2024-01-06
 */

#include "model.h"
#include "math/hash.h"

namespace hs::ren {
    Model Model::init(std::vector<vk::Vertex>&& v, std::vector<uint16_t>&& i) {
        Model model = Model();
        model.v = v;
        model.i = i;

        m::Sip13 hasher = m::Sip13::init();

        for (const vk::Vertex& vert : model.v) {
            hasher.write(static_cast<uint64_t>(*((const uint32_t*) &vert.pos.x)));
            hasher.write(static_cast<uint64_t>(*((const uint32_t*) &vert.pos.y)));
            hasher.write(static_cast<uint64_t>(*((const uint32_t*) &vert.col.x)));
            hasher.write(static_cast<uint64_t>(*((const uint32_t*) &vert.col.y)));
            hasher.write(static_cast<uint64_t>(*((const uint32_t*) &vert.col.z)));
            hasher.write(static_cast<uint64_t>(*((const uint32_t*) &vert.uv.x)));
            hasher.write(static_cast<uint64_t>(*((const uint32_t*) &vert.uv.y)));
        }

        for (uint16_t ind : model.i) {
            hasher.write(static_cast<uint64_t>(ind));
        }

        model.h = std::move(hasher).finalise();

        return model;
    }

    uint64_t Model::hash() const {
        return h;
    }

    const std::vector<vk::Vertex>& Model::vertices() const {
        return this->v;
    }

    const std::vector<uint16_t>& Model::indices() const {
        return this->i;
    }
}