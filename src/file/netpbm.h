/*
 * Created by janis on 2023-10-01
 */

#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace hs::f {
    std::tuple<std::vector<uint8_t>, uint32_t, uint32_t> load_ppm(const std::string& file);
}