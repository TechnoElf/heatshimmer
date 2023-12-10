/*
 * Created by janis on 2023-10-01
 */

#pragma once

#include "swapchain.h"

namespace hs::ren::vk {
    class CommandBuffer {
        Device& device;
        Swapchain& swapchain;

        VkCommandBuffer command_buffer = nullptr;

    public:
        CommandBuffer(const CommandBuffer&) = delete;

        void submit();

        void transition_image_layout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);
        void copy_buffer_to_image(VkBuffer src, VkImage dst, uint32_t width, uint32_t height);

        explicit CommandBuffer(Device& device, Swapchain& swapchain);
        ~CommandBuffer();
    };
}