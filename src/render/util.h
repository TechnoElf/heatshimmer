#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

#include "error.h"

namespace hs::ren {
    namespace vk {
        void check(VkResult res, const char* fn);
        const char* result_to_string(VkResult res);
        void check_layer(Severity sev, const char* msg);
        void info(const char* msg);
    }

    std::vector<char> read_file(const std::string& path);
}