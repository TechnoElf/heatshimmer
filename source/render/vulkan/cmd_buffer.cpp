/*
 * Created by janis on 2023-10-01
 */

#include "cmd_buffer.h"
#include "render/util.h"

namespace hs::ren::vk {
    CommandBuffer::CommandBuffer(Device& device, Swapchain& swapchain) : device(device), swapchain(swapchain) {
        VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
        command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        command_buffer_allocate_info.commandPool = this->swapchain.pool();
        command_buffer_allocate_info.commandBufferCount = 1;

        VK_CHECK(vkAllocateCommandBuffers(*this->device, &command_buffer_allocate_info, &this->command_buffer), "vkAllocateCommandBuffers");

        VkCommandBufferBeginInfo command_buffer_begin_info = {};
        command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(this->command_buffer, &command_buffer_begin_info);
    }

    CommandBuffer::~CommandBuffer() {
        vkFreeCommandBuffers(*this->device, this->swapchain.pool(), 1, &command_buffer);
    }

    void CommandBuffer::submit() {
        VK_CHECK(vkEndCommandBuffer(this->command_buffer), "vkEndCommandBuffer");

        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &this->command_buffer;

        VK_CHECK(vkQueueSubmit(this->device.queue(), 1, &submit_info, VK_NULL_HANDLE), "vkQueueSubmit");
        VK_CHECK(vkQueueWaitIdle(this->device.queue()), "vkQueueWaitIdle");
    }

    void CommandBuffer::transition_image_layout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout) {
        VkImageMemoryBarrier image_memory_barrier = {};
        image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        image_memory_barrier.oldLayout = old_layout;
        image_memory_barrier.newLayout = new_layout;
        image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        image_memory_barrier.image = image;
        image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_memory_barrier.subresourceRange.baseMipLevel = 0;
        image_memory_barrier.subresourceRange.levelCount = 1;
        image_memory_barrier.subresourceRange.baseArrayLayer = 0;
        image_memory_barrier.subresourceRange.layerCount = 1;
        image_memory_barrier.srcAccessMask = 0;
        image_memory_barrier.dstAccessMask = 0;

        VkPipelineStageFlags src_stage_mask = 0;
        VkPipelineStageFlags dst_stage_mask = 0;

        if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            src_stage_mask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dst_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            src_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dst_stage_mask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }

        vkCmdPipelineBarrier(this->command_buffer, src_stage_mask, dst_stage_mask, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);
    }

    void CommandBuffer::copy_buffer_to_image(VkBuffer src, VkImage dst, uint32_t width, uint32_t height) {
        VkBufferImageCopy buffer_image_copy = {};
        buffer_image_copy.bufferOffset = 0;
        buffer_image_copy.bufferRowLength = 0;
        buffer_image_copy.bufferImageHeight = 0;
        buffer_image_copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        buffer_image_copy.imageSubresource.mipLevel = 0;
        buffer_image_copy.imageSubresource.baseArrayLayer = 0;
        buffer_image_copy.imageSubresource.layerCount = 1;
        buffer_image_copy.imageOffset = {0, 0, 0};
        buffer_image_copy.imageExtent = {width, height, 1};

        vkCmdCopyBufferToImage(this->command_buffer, src, dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &buffer_image_copy);
    }
}