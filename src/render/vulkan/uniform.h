/*
 * Created by janis on 2023-09-30
 */

#pragma once

#include "math/mat.h"

namespace hs::ren::vk {
    struct UniformBufferObject {
        m::Mat4 model;
        m::Mat4 view;
        m::Mat4 projection;
    };
}