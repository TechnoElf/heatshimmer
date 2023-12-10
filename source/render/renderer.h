#pragma once

#include <vulkan/vulkan.h>
#include <memory>

#include "hid.h"
#include "vulkan/instance.h"
#include "vulkan/device.h"
#include "vulkan/pipeline.h"
#include "vulkan/swapchain.h"
#include "vulkan/vertex.h"
#include "math/vec.h"
#include "render/vulkan/uniform.h"

namespace hs::ren {
    class Renderer {
        std::unique_ptr<vk::Instance> instance;
        std::unique_ptr<vk::Device> device;
        std::unique_ptr<vk::Swapchain> swapchain;
        std::vector<std::unique_ptr<vk::Pipeline>> pipelines;

        VkSemaphore image_available_semaphore = nullptr;
        VkSemaphore render_done_semaphore = nullptr;
        VkFence frame_done_fence = nullptr;

        std::pair<VkBuffer, VkDeviceMemory> vert_buf;
        std::pair<VkBuffer, VkDeviceMemory> ind_buf;

        std::vector<std::pair<VkBuffer, VkDeviceMemory>> uniform_buf;

        VkDescriptorPool descriptor_pool;
        std::vector<VkDescriptorSet> descriptor_sets;

        std::pair<VkImage, VkDeviceMemory> texture_image;
        VkImageView image_view;
        VkSampler image_sampler;

        const std::vector<vk::Vertex> vertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}
        };

        const std::vector<uint16_t> indices = {
            0, 1, 2, 0, 2, 3
        };

        vk::UniformBufferObject ubo = {
            m::Mat4::transform(0.0f, 0.0f, 1.0f) * m::Mat4::rotz(0.1),
            m::Mat4::roty(-0.1),
            m::Mat4::perspective()
        };

        float y = 0.0;

    public:
        explicit Renderer(Hid& hid);
        ~Renderer();

        void draw(bool pressed);
    };
}