#include "renderer.h"
#include "util.h"
#include "hid.h"
#include "vulkan/uniform.h"
#include "vulkan/cmd_buffer.h"
#include "file/netpbm.h"

#include <vector>
#include <vulkan/vulkan.h>
#include <memory>
#include <array>

namespace hs::ren {
    Renderer::Renderer(Hid& hid) :
        instance(std::make_unique<vk::Instance>(hid)),
        device(std::make_unique<vk::Device>(*instance, hid)),
        swapchain(std::make_unique<vk::Swapchain>(*device)) {
        this->pipelines.push_back(std::make_unique<vk::Pipeline>(*device, *swapchain, "base"));

        this->image_available_semaphore = this->device->create_semaphore();
        this->render_done_semaphore = this->device->create_semaphore();
        this->frame_done_fence = this->device->create_fence(VK_FENCE_CREATE_SIGNALED_BIT);

        void* data;

        this->vert_buf = this->device->create_buffer(vertices.size() * sizeof(vertices[0]), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        vk::check(vkMapMemory(**this->device, this->vert_buf.second, 0, vertices.size() * sizeof(vertices[0]), 0, &data), "vkMapMemory");
        memcpy(data, vertices.data(), vertices.size() * sizeof(vertices[0]));
        vkUnmapMemory(**this->device, this->vert_buf.second);

        this->ind_buf = this->device->create_buffer(indices.size() * sizeof(indices[0]), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
        vk::check(vkMapMemory(**this->device, this->ind_buf.second, 0, indices.size() * sizeof(indices[0]), 0, &data), "vkMapMemory");
        memcpy(data, indices.data(), indices.size() * sizeof(indices[0]));
        vkUnmapMemory(**this->device, this->ind_buf.second);

        for (uint32_t i = 0; i < this->swapchain->frames().size(); i++) {
            this->uniform_buf.push_back(this->device->create_buffer(sizeof(vk::UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT));
            vk::check(vkMapMemory(**this->device, this->uniform_buf[i].second, 0, sizeof(vk::UniformBufferObject), 0, &data), "vkMapMemory");
            memcpy(data, &ubo, sizeof(vk::UniformBufferObject));
            vkUnmapMemory(**this->device, this->uniform_buf[i].second);
        }

        auto [img, img_w, img_h] = hs::f::load_ppm("../img.ppm");

        auto image_staging_buffer = this->device->create_buffer(img_w * img_h * 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
        vk::check(vkMapMemory(**this->device, image_staging_buffer.second, 0, img_w * img_h * 4, 0, &data), "vkMapMemory");
        memcpy(data, img.data(), img_w * img_h * 4);
        vkUnmapMemory(**this->device, image_staging_buffer.second);

        this->texture_image = this->device->create_image(img_w, img_h);

        vk::CommandBuffer cmd = vk::CommandBuffer(*this->device, *this->swapchain);
        cmd.transition_image_layout(texture_image.first, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        cmd.copy_buffer_to_image(image_staging_buffer.first, texture_image.first, img_w, img_h);
        cmd.transition_image_layout(texture_image.first, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        cmd.submit();

        vkFreeMemory(**this->device, image_staging_buffer.second, nullptr);
        vkDestroyBuffer(**this->device, image_staging_buffer.first, nullptr);

        VkImageViewCreateInfo image_view_create_info = {};
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.image = this->texture_image.first;
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = VK_FORMAT_R8G8B8A8_SRGB;
        image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;

        vk::check(vkCreateImageView(**this->device, &image_view_create_info, nullptr, &this->image_view), "vkCreateImageView");

        VkSamplerCreateInfo sampler_create_info = {};
        sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_create_info.magFilter = VK_FILTER_LINEAR;
        sampler_create_info.minFilter = VK_FILTER_LINEAR;
        sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_create_info.anisotropyEnable = VK_FALSE;
        sampler_create_info.maxAnisotropy = 1.0f; // TODO
        sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        sampler_create_info.unnormalizedCoordinates = VK_FALSE;
        sampler_create_info.compareEnable = VK_FALSE;
        sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
        sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_create_info.mipLodBias = 0.0f;
        sampler_create_info.minLod = 0.0f;
        sampler_create_info.maxLod = 0.0f;

        vk::check(vkCreateSampler(**this->device, &sampler_create_info, nullptr, &this->image_sampler), "vkCreateSampler");

        auto descriptor_pool_sizes = std::array<VkDescriptorPoolSize, 2>();
        descriptor_pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_pool_sizes[0].descriptorCount = this->uniform_buf.size();
        descriptor_pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_pool_sizes[1].descriptorCount = this->uniform_buf.size();

        VkDescriptorPoolCreateInfo descriptor_pool_create_info = {};
        descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptor_pool_create_info.poolSizeCount = descriptor_pool_sizes.size();
        descriptor_pool_create_info.pPoolSizes = descriptor_pool_sizes.data();
        descriptor_pool_create_info.maxSets = this->uniform_buf.size();

        vk::check(vkCreateDescriptorPool(**this->device, &descriptor_pool_create_info, nullptr, &this->descriptor_pool), "vkCreateDescriptorPool");

        std::vector<VkDescriptorSetLayout> layouts = std::vector<VkDescriptorSetLayout>(this->uniform_buf.size(), this->pipelines[0]->descriptor_layout());
        VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {};
        descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptor_set_allocate_info.descriptorPool = this->descriptor_pool;
        descriptor_set_allocate_info.descriptorSetCount = this->uniform_buf.size();
        descriptor_set_allocate_info.pSetLayouts = layouts.data();

        this->descriptor_sets.resize(this->uniform_buf.size());
        vk::check(vkAllocateDescriptorSets(**this->device, &descriptor_set_allocate_info, this->descriptor_sets.data()), "vkAllocateDescriptorSets");

        for (uint32_t i = 0; i < this->uniform_buf.size(); i++) {
            VkDescriptorBufferInfo descriptor_buffer_info = {};
            descriptor_buffer_info.buffer = uniform_buf[i].first;
            descriptor_buffer_info.offset = 0;
            descriptor_buffer_info.range = sizeof(vk::UniformBufferObject);

            VkDescriptorImageInfo descriptor_image_info = {};
            descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            descriptor_image_info.imageView = image_view;
            descriptor_image_info.sampler = image_sampler;

            auto write_descriptor_set = std::array<VkWriteDescriptorSet, 2>();
            write_descriptor_set[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write_descriptor_set[0].dstSet = this->descriptor_sets[i];
            write_descriptor_set[0].dstBinding = 0;
            write_descriptor_set[0].dstArrayElement = 0;
            write_descriptor_set[0].descriptorCount = 1;
            write_descriptor_set[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write_descriptor_set[0].pBufferInfo = &descriptor_buffer_info;
            write_descriptor_set[0].pTexelBufferView = nullptr;
            write_descriptor_set[0].pImageInfo = nullptr;
            write_descriptor_set[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write_descriptor_set[1].dstSet = this->descriptor_sets[i];
            write_descriptor_set[1].dstBinding = 1;
            write_descriptor_set[1].dstArrayElement = 0;
            write_descriptor_set[1].descriptorCount = 1;
            write_descriptor_set[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            write_descriptor_set[1].pBufferInfo = nullptr;
            write_descriptor_set[1].pTexelBufferView = nullptr;
            write_descriptor_set[1].pImageInfo = &descriptor_image_info;

            vkUpdateDescriptorSets(**this->device, write_descriptor_set.size(), write_descriptor_set.data(), 0, nullptr);
        }
    }

    Renderer::~Renderer() {
        this->device->wait_idle();

        vkDestroySampler(**this->device, this->image_sampler, nullptr);

        vkDestroyImageView(**this->device, this->image_view, nullptr);

        vkFreeMemory(**this->device, this->texture_image.second, nullptr);
        vkDestroyImage(**this->device, this->texture_image.first, nullptr);

        vkDestroyDescriptorPool(**this->device, descriptor_pool, nullptr);

        for (auto [buf, mem] : this->uniform_buf) {
            vkFreeMemory(**this->device, mem, nullptr);
            vkDestroyBuffer(**this->device, buf, nullptr);
        }

        vkFreeMemory(**this->device, this->ind_buf.second, nullptr);
        vkDestroyBuffer(**this->device, this->ind_buf.first, nullptr);

        vkFreeMemory(**this->device, this->vert_buf.second, nullptr);
        vkDestroyBuffer(**this->device, this->vert_buf.first, nullptr);

        vkDestroyFence(**this->device, this->frame_done_fence, nullptr);
        vkDestroySemaphore(**this->device, this->render_done_semaphore, nullptr);
        vkDestroySemaphore(**this->device, this->image_available_semaphore, nullptr);
    }

    void Renderer::draw(bool pressed) {
        if (pressed) {
            this->y += 0.01;
        }
        this->ubo.view = m::Mat4::roty(y);

        vk::check(vkWaitForFences(**this->device, 1, &this->frame_done_fence, VK_TRUE, UINT64_MAX), "vkWaitForFences");
        vk::check(vkResetFences(**this->device, 1, &this->frame_done_fence), "vkResetFences");

        uint32_t image_index;
        vk::check(vkAcquireNextImageKHR(**this->device, **this->swapchain, UINT64_MAX, this->image_available_semaphore,
                                       VK_NULL_HANDLE, &image_index), "vkAcquireNextImageKHR");

        void* data;
        vk::check(vkMapMemory(**this->device, this->uniform_buf[image_index].second, 0, sizeof(vk::UniformBufferObject), 0, &data), "vkMapMemory");
        memcpy(data, &ubo, sizeof(vk::UniformBufferObject));
        vkUnmapMemory(**this->device, this->uniform_buf[image_index].second);

        vk::check(vkResetCommandBuffer(this->swapchain->command(), 0), "vkResetCommandBuffer");

        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        vk::check(vkBeginCommandBuffer(this->swapchain->command(), &begin_info), "vkBeginCommandBuffer");

        VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

        VkRenderPassBeginInfo render_pass_begin_info = {};
        render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_begin_info.renderPass = this->swapchain->pass();
        render_pass_begin_info.framebuffer = this->swapchain->frames()[image_index];
        render_pass_begin_info.renderArea.offset = {0, 0};
        render_pass_begin_info.renderArea.extent = this->device->details().swap_extent;
        render_pass_begin_info.clearValueCount = 1;
        render_pass_begin_info.pClearValues = &clear_color;

        vkCmdBeginRenderPass(this->swapchain->command(), &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

        for (auto& pipeline : this->pipelines) {
            vkCmdBindPipeline(this->swapchain->command(), VK_PIPELINE_BIND_POINT_GRAPHICS, **pipeline);

            VkBuffer buffers[] = {this->vert_buf.first};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(this->swapchain->command(), 0, 1, buffers, offsets);

            vkCmdBindIndexBuffer(this->swapchain->command(), this->ind_buf.first, 0, VK_INDEX_TYPE_UINT16);

            vkCmdBindDescriptorSets(this->swapchain->command(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->layout(), 0, 1, &descriptor_sets[image_index], 0, nullptr);

            vkCmdDrawIndexed(this->swapchain->command(), indices.size(), 1, 0, 0, 0);
        }

        vkCmdEndRenderPass(this->swapchain->command());

        vk::check(vkEndCommandBuffer(this->swapchain->command()), "vkEndCommandBuffer");

        VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        std::vector<VkCommandBuffer> commands = {this->swapchain->command()};

        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pWaitDstStageMask = wait_stages;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = &this->image_available_semaphore;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = commands.data();
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &this->render_done_semaphore;

        vk::check(vkQueueSubmit(this->device->queue(), 1, &submit_info, this->frame_done_fence), "vkQueueSubmit");

        std::vector<VkSwapchainKHR> swapchains = {**this->swapchain};

        VkPresentInfoKHR present_info = {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = &this->render_done_semaphore;
        present_info.swapchainCount = 1;
        present_info.pSwapchains = swapchains.data();
        present_info.pImageIndices = &image_index;

        vk::check(vkQueuePresentKHR(this->device->queue(), &present_info), "vkQueuePresentKHR");
    }
}