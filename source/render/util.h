#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

namespace hs::ren {
    void VK_CHECK(VkResult res, const char* fn);
    const char* VK_RESULT_CODE(VkResult res);
    std::vector<char> read_file(const std::string& path);
}