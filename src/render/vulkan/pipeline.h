#pragma once

#include <vulkan/vulkan.h>
#include <string>

#include "device.h"
#include "swapchain.h"

namespace hs::ren::vk {
    class Pipeline {
        Device& device;

        VkShaderModule vert_shader = nullptr;
        VkShaderModule frag_shader = nullptr;
        VkDescriptorSetLayout descriptor_set_layout = nullptr;
        VkPipelineLayout pipeline_layout = nullptr;
        VkPipeline pipeline = nullptr;

    public:
        Pipeline(const Swapchain&) = delete;

        explicit Pipeline(Device& device, Swapchain& swapchain, std::string shader);
        ~Pipeline();

        VkPipeline operator*();

        VkPipelineLayout layout();
        VkDescriptorSetLayout descriptor_layout();
    };
}
