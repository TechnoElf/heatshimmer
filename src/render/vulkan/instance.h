#pragma once

#include "../hid.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace hs::ren::vk {
    class Instance {
        std::vector<const char*> required_instance_extensions = {"VK_KHR_portability_enumeration"};
        std::vector<const char*> required_instance_layers = {"VK_LAYER_KHRONOS_validation"};
        std::vector<VkExtensionProperties> available_instance_extensions;
        std::vector<VkLayerProperties> available_instance_layers;
        VkInstance instance = nullptr;

        VkDebugUtilsMessengerEXT debugMessenger = nullptr;

        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
                VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
                VkDebugUtilsMessageTypeFlagsEXT msg_type,
                const VkDebugUtilsMessengerCallbackDataEXT* msg_data,
                void* user_data
        );

    public:
        Instance(const Instance&) = delete;

        explicit Instance(Hid& hid);
        ~Instance();

        VkInstance operator*();
    };
}