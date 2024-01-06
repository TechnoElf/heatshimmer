#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <map>

#include "hid.h"
#include "vulkan/instance.h"
#include "vulkan/device.h"
#include "vulkan/pipeline.h"
#include "vulkan/swapchain.h"
#include "vulkan/vertex.h"
#include "math/vec.h"
#include "render/vulkan/uniform.h"
#include "structure/world.h"

namespace hs::ren {
    struct ModelData {
        std::pair<VkBuffer, VkDeviceMemory> vert_buf;
        std::pair<VkBuffer, VkDeviceMemory> ind_buf;
        uint32_t ind_count;
    };

    class Renderer {
    public:
        explicit Renderer(Hid& hid);
        ~Renderer();

        void draw(bool pressed, const str::World& world);
        void sync(const str::World& world);

    private:
        std::unique_ptr<vk::Instance> instance;
        std::unique_ptr<vk::Device> device;
        std::unique_ptr<vk::Swapchain> swapchain;
        std::vector<std::unique_ptr<vk::Pipeline>> pipelines;

        VkSemaphore image_available_semaphore = nullptr;
        VkSemaphore render_done_semaphore = nullptr;
        VkFence frame_done_fence = nullptr;

        std::map<uint64_t, ModelData> model_data;

        std::vector<std::pair<VkBuffer, VkDeviceMemory>> uniform_buf;

        VkDescriptorPool descriptor_pool;
        std::vector<VkDescriptorSet> descriptor_sets;

        std::pair<VkImage, VkDeviceMemory> texture_image;
        VkImageView image_view;
        VkSampler image_sampler;

        vk::UniformBufferObject ubo = {
            m::Mat4::transform(0.0f, 0.0f, 1.0f) * m::Mat4::rotz(0.1),
            m::Mat4::roty(-0.1),
            m::Mat4::perspective()
        };

        float y = 0.0;
    };
}