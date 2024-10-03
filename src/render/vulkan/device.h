#pragma once

#include "../hid.h"
#include "instance.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace hs::ren::vk {
    struct DeviceDetails {
        VkPhysicalDevice device;
        VkPhysicalDeviceProperties properties;
        std::vector<VkExtensionProperties> extensions;
        std::vector<VkQueueFamilyProperties> queue_families;
        uint32_t selected_queue_family;
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> surface_formats;
        uint32_t selected_surface_format;
        std::vector<VkPresentModeKHR> present_modes;
        uint32_t selected_present_mode;
        VkExtent2D swap_extent;
        VkPhysicalDeviceMemoryProperties memory_properties;
    };

    class Device {
        Instance& instance;

        VkSurfaceKHR display_surface = nullptr;

        std::vector<const char*> required_device_extensions = {"VK_KHR_swapchain"};
        std::vector<const char*> required_device_layers = {"VK_LAYER_KHRONOS_validation"};
        std::vector<DeviceDetails> available_physical_devices;
        DeviceDetails physical_device;
        VkDevice device = nullptr;
        VkQueue graphics_and_display_queue = nullptr;

    public:
        Device(const Device&) = delete;

        explicit Device(Instance& instance, Hid& hid);
        ~Device();

        VkDevice operator*();

        DeviceDetails& details();
        VkQueue queue();
        VkSurfaceKHR surface();

        void wait_idle();

        VkSemaphore create_semaphore();
        VkFence create_fence(VkFenceCreateFlags flags);
        std::pair<VkBuffer, VkDeviceMemory> create_buffer(size_t size, VkBufferUsageFlags usage);
        std::pair<VkImage, VkDeviceMemory> create_image(uint32_t width, uint32_t height);
    };
}
