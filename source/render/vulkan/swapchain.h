#pragma once

#include "device.h"

#include <vulkan/vulkan.h>

namespace hs::ren::vk {
    class Swapchain {
        Device& device;

        VkSwapchainKHR swapchain = nullptr;
        std::vector<VkImage> swapchain_images;
        std::vector<VkImageView> image_views;
        VkRenderPass render_pass = nullptr;
        std::vector<VkFramebuffer> framebuffers;
        VkCommandPool command_pool = nullptr;
        VkCommandBuffer command_buffer = nullptr;

    public:
        Swapchain(const Swapchain&) = delete;

        explicit Swapchain(Device& device);
        ~Swapchain();

        VkSwapchainKHR operator*();

        VkCommandBuffer command();
        std::vector<VkFramebuffer>& frames();
        VkRenderPass pass();
        VkCommandPool pool();
    };
}
