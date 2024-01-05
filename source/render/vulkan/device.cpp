#include "device.h"

#include "instance.h"
#include "../util.h"

#include <optional>
#include <algorithm>
#include <iostream>
#include <cstring>

namespace hs::ren::vk {
    Device::Device(Instance &instance, Hid &hid) : instance(instance) {
        this->display_surface = hid.create_vk_surface(*this->instance);

        uint32_t physical_device_count = 0;
        check(vkEnumeratePhysicalDevices(*this->instance, &physical_device_count, nullptr), "vkEnumeratePhysicalDevices");
        std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
        check(vkEnumeratePhysicalDevices(*this->instance, &physical_device_count, physical_devices.data()), "vkEnumeratePhysicalDevices");

        this->available_physical_devices = std::vector<DeviceDetails>(physical_device_count);
        for (VkPhysicalDevice dev : physical_devices) {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(dev, &properties);

            uint32_t extension_count = 0;
            check(vkEnumerateDeviceExtensionProperties(dev, nullptr, &extension_count, nullptr), "vkEnumerateDeviceExtensionProperties");
            std::vector<VkExtensionProperties> extensions(extension_count);
            check(vkEnumerateDeviceExtensionProperties(dev, nullptr, &extension_count, extensions.data()), "vkEnumerateDeviceExtensionProperties");

            uint32_t queue_family_count = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(dev, &queue_family_count, nullptr);
            std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
            vkGetPhysicalDeviceQueueFamilyProperties(dev, &queue_family_count, queue_families.data());

            VkSurfaceCapabilitiesKHR capabilities;
            check(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(dev, this->display_surface, &capabilities), "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");

            uint32_t formats_count = 0;
            check(vkGetPhysicalDeviceSurfaceFormatsKHR(dev, this->display_surface, &formats_count, nullptr), "vkGetPhysicalDeviceSurfaceFormatsKHR");
            std::vector<VkSurfaceFormatKHR> formats(formats_count);
            check(vkGetPhysicalDeviceSurfaceFormatsKHR(dev, this->display_surface, &formats_count, formats.data()), "vkGetPhysicalDeviceSurfaceFormatsKHR");

            uint32_t modes_count = 0;
            check(vkGetPhysicalDeviceSurfacePresentModesKHR(dev, this->display_surface, &modes_count, nullptr), "vkGetPhysicalDeviceSurfacePresentModesKHR");
            std::vector<VkPresentModeKHR> modes(modes_count);
            check(vkGetPhysicalDeviceSurfacePresentModesKHR(dev, this->display_surface, &modes_count, modes.data()), "vkGetPhysicalDeviceSurfacePresentModesKHR");

            VkPhysicalDeviceMemoryProperties memory_properties;
            vkGetPhysicalDeviceMemoryProperties(dev, &memory_properties);

            this->available_physical_devices.push_back(DeviceDetails{
                .device = dev,
                .properties = properties,
                .extensions = extensions,
                .queue_families = queue_families,
                .selected_queue_family = 0,
                .capabilities = capabilities,
                .surface_formats = formats,
                .selected_surface_format = 0,
                .present_modes = modes,
                .selected_present_mode = 0,
                .swap_extent = VkExtent2D{0, 0},
                .memory_properties = memory_properties
            });
        }

        std::optional<DeviceDetails> selected_physical_device;
        for (DeviceDetails &dev : this->available_physical_devices) {
            std::optional<uint32_t> selected_queue_family;
            for (uint32_t i = 0; i < dev.queue_families.size(); i++) {
                VkBool32 surface_support = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(dev.device, i, this->display_surface, &surface_support);
                if (!surface_support) { continue; }

                if (!(dev.queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) { continue; }

                selected_queue_family = i;
            }
            if (selected_queue_family.has_value()) {
                dev.selected_queue_family = selected_queue_family.value();
            } else {
                continue;
            }

            if (dev.properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && dev.properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) { continue; }

            for (const char *extension : this->required_device_extensions) {
                if (std::find_if(dev.extensions.begin(), dev.extensions.end(), [&extension](VkExtensionProperties other) { return std::strcmp(extension, other.extensionName); })
                    == dev.extensions.end()) {
                    continue;
                }
            }

            std::optional<uint32_t> selected_surface_format;
            for (uint32_t i = 0; i < dev.surface_formats.size(); i++) {
                if (dev.surface_formats[i].format != VK_FORMAT_B8G8R8A8_SRGB) { continue; }
                if (dev.surface_formats[i].colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) { continue; }

                selected_surface_format = i;
            }
            if (selected_surface_format.has_value()) {
                dev.selected_surface_format = selected_surface_format.value();
            } else {
                continue;
            }

            std::optional<uint32_t> selected_present_mode;
            for (uint32_t i = 0; i < dev.present_modes.size(); i++) {
                if (dev.present_modes[i] != VK_PRESENT_MODE_MAILBOX_KHR) { continue; }

                selected_present_mode = i;
            }
            if (selected_present_mode.has_value()) {
                dev.selected_present_mode = selected_present_mode.value();
            } else {
                continue;
            }

            VkExtent2D swap_extent = hid.get_win_extent();
            swap_extent.width = std::clamp(swap_extent.width, dev.capabilities.minImageExtent.width, dev.capabilities.maxImageExtent.width);
            swap_extent.height = std::clamp(swap_extent.height, dev.capabilities.minImageExtent.height, dev.capabilities.maxImageExtent.height);
            dev.swap_extent = swap_extent;

            selected_physical_device = dev;
        }

        if (selected_physical_device.has_value()) {
            this->physical_device = selected_physical_device.value();
        } else {
            std::cerr << "[VK] No suitable device found" << std::endl;
            std::cerr << "[VK] Available devices" << std::endl;
            for (DeviceDetails dev : this->available_physical_devices) {
                std::cerr << "[VK] \t" << dev.properties.deviceName << std::endl;
            }
            abort();
        }

        float queue_priority = 1.0f;
        VkDeviceQueueCreateInfo queue_create_info = {};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueCount = 1;
        queue_create_info.queueFamilyIndex = selected_physical_device->selected_queue_family;
        queue_create_info.pQueuePriorities = &queue_priority;

        VkPhysicalDeviceFeatures deviceFeatures = {};

        VkDeviceCreateInfo device_create_info = {};
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.queueCreateInfoCount = 1;
        device_create_info.pQueueCreateInfos = &queue_create_info;
        device_create_info.pEnabledFeatures = &deviceFeatures;
        device_create_info.enabledExtensionCount = this->required_device_extensions.size();
        device_create_info.ppEnabledExtensionNames = this->required_device_extensions.data();
        device_create_info.enabledLayerCount = this->required_device_layers.size();
        device_create_info.ppEnabledLayerNames = this->required_device_layers.data();

        check(vkCreateDevice(this->physical_device.device, &device_create_info, nullptr, &this->device), "vkCreateDevice");

        vkGetDeviceQueue(this->device, selected_physical_device->selected_queue_family, 0, &this->graphics_and_display_queue);

        std::cout << "[VK] Selected device " << this->physical_device.properties.deviceName << std::endl;
    }

    Device::~Device() {
        vkDestroyDevice(this->device, nullptr);
        vkDestroySurfaceKHR(*this->instance, this->display_surface, nullptr);
    }

    VkDevice Device::operator*() {
        return this->device;
    }

    DeviceDetails &Device::details() {
        return this->physical_device;
    }

    VkQueue Device::queue() {
        return this->graphics_and_display_queue;
    }

    VkSurfaceKHR Device::surface() {
        return this->display_surface;
    }

    void Device::wait_idle() {
        vkDeviceWaitIdle(this->device);
    }

    VkSemaphore Device::create_semaphore() {
        VkSemaphoreCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkSemaphore semaphore;
        check(vkCreateSemaphore(this->device, &create_info, nullptr, &semaphore), "vkCreateSemaphore");

        return semaphore;
    }

    VkFence Device::create_fence(VkFenceCreateFlags flags) {
        VkFenceCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        create_info.flags = flags;

        VkFence fence;
        check(vkCreateFence(this->device, &create_info, nullptr, &fence), "vkCreateFence");

        return fence;
    }

    std::pair<VkBuffer, VkDeviceMemory> Device::create_buffer(size_t size, VkBufferUsageFlags usage) {
        VkBufferCreateInfo buffer_create_info = {};
        buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_create_info.usage = usage;
        buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        buffer_create_info.size = size;

        VkBuffer buffer;
        check(vkCreateBuffer(this->device, &buffer_create_info, nullptr, &buffer), "vkCreateBuffer");

        VkMemoryRequirements memory_requirements;
        vkGetBufferMemoryRequirements(this->device, buffer, &memory_requirements);

        std::optional<uint32_t> memory_type = {};
        for (uint32_t i = 0; i < this->physical_device.memory_properties.memoryTypeCount; i++) {
            if (memory_requirements.memoryTypeBits & (1 << i)) {
                if (this->physical_device.memory_properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT &&
                    this->physical_device.memory_properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
                    memory_type = i;
                }
            }
        }

        if (memory_type) {
            VkMemoryAllocateInfo allocate_info = {};
            allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocate_info.allocationSize = memory_requirements.size;
            allocate_info.memoryTypeIndex = memory_type.value();

            VkDeviceMemory memory;
            check(vkAllocateMemory(this->device, &allocate_info, nullptr, &memory), "vkAllocateMemory");

            check(vkBindBufferMemory(this->device, buffer, memory, 0), "vkBindBufferMemory");

            return {buffer, memory};
        } else {
            vkDestroyBuffer(this->device, buffer, nullptr);
            check(VK_ERROR_UNKNOWN, "vkGetBufferMemoryRequirements");
            return {nullptr, nullptr};
        }
    }

    std::pair<VkImage, VkDeviceMemory> Device::create_image(uint32_t width, uint32_t height) {
        VkImageCreateInfo image_create_info = {};
        image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_create_info.imageType = VK_IMAGE_TYPE_2D;
        image_create_info.extent.width = width;
        image_create_info.extent.height = height;
        image_create_info.extent.depth = 1;
        image_create_info.mipLevels = 1;
        image_create_info.arrayLayers = 1;
        image_create_info.format = VK_FORMAT_R8G8B8A8_SRGB;
        image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_create_info.flags = 0;

        VkImage image;
        check(vkCreateImage(this->device, &image_create_info, nullptr, &image), "vkCreateImage");

        VkMemoryRequirements memory_requirements;
        vkGetImageMemoryRequirements(this->device, image, &memory_requirements);

        std::optional<uint32_t> memory_type = {};
        for (uint32_t i = 0; i < this->physical_device.memory_properties.memoryTypeCount; i++) {
            if (memory_requirements.memoryTypeBits & (1 << i)) {
                if (this->physical_device.memory_properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                    memory_type = i;
                }
            }
        }

        if (memory_type) {
            VkMemoryAllocateInfo allocate_info = {};
            allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocate_info.allocationSize = memory_requirements.size;
            allocate_info.memoryTypeIndex = memory_type.value();

            VkDeviceMemory memory;
            check(vkAllocateMemory(this->device, &allocate_info, nullptr, &memory), "vkAllocateMemory");

            check(vkBindImageMemory(this->device, image, memory, 0), "vkBindImageMemory");

            return {image, memory};
        } else {
            vkDestroyImage(this->device, image, nullptr);
            check(VK_ERROR_UNKNOWN, "vkGetBufferMemoryRequirements");
            return {nullptr, nullptr};
        }
    }
}