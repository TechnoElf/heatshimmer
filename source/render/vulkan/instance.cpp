#include "instance.h"

#include "../util.h"

#include <iostream>
#include <algorithm>
#include <cstring>

namespace hs::ren::vk {
    VkBool32 vk::Instance::debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
                                          VkDebugUtilsMessageTypeFlagsEXT msg_type,
                                          const VkDebugUtilsMessengerCallbackDataEXT *msg_data, void *user_data) {
        std::cerr << "[VK] # " << msg_data->pMessage << std::endl;
        return VK_FALSE;
    }

    Instance::Instance(Hid &hid) {
        uint32_t extension_count = 0;
        VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr), "vkEnumerateInstanceExtensionProperties");
        this->available_instance_extensions = std::vector<VkExtensionProperties>(extension_count);
        VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, this->available_instance_extensions.data()), "vkEnumerateInstanceExtensionProperties");

        uint32_t layer_count = 0;
        VK_CHECK(vkEnumerateInstanceLayerProperties(&layer_count, nullptr), "vkEnumerateInstanceLayerProperties");
        this->available_instance_layers =  std::vector<VkLayerProperties>(layer_count);
        VK_CHECK(vkEnumerateInstanceLayerProperties(&layer_count, this->available_instance_layers.data()), "vkEnumerateInstanceLayerProperties");

        std::vector<const char*> hid_extensions = hid.get_vk_extensions();
        this->required_instance_extensions.insert(this->required_instance_extensions.end(), hid_extensions.begin(), hid_extensions.end());

        for (const char* extension: this->required_instance_extensions) {
            if(std::find_if(this->available_instance_extensions.begin(), this->available_instance_extensions.end(), [&extension](VkExtensionProperties other) { return std::strcmp(extension, other.extensionName); }) == this->available_instance_extensions.end()) {
                std::cerr << "[VK] Instance extension " << extension << " not supported" << std::endl;
                std::cerr << "[VK] Available extensions" << std::endl;
                for (const VkExtensionProperties &available : this->available_instance_extensions) {
                    std::cerr << "[VK] \t" << available.extensionName << std::endl;
                }
                abort();
            }
        }
        std::cout << "[VK] All required instance extensions supported" << std::endl;

        for (const char* layer: this->required_instance_layers) {
            if(std::find_if(this->available_instance_layers.begin(), this->available_instance_layers.end(), [&layer](VkLayerProperties other) { return std::strcmp(layer, other.layerName); }) == this->available_instance_layers.end()) {
                std::cerr << "[VK] Instance layer " << layer << " not supported" << std::endl;
                std::cerr << "[VK] Available layers" << std::endl;
                for (const VkLayerProperties &available : this->available_instance_layers) {
                    std::cerr << "[VK] \t" << available.layerName << std::endl;
                }
                abort();
            }
        }
        std::cout << "[VK] All required instance layers supported" << std::endl;

        VkApplicationInfo applicationInfo = {};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "Heatshimmer";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "Heatshimmer";
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledExtensionCount = this->required_instance_extensions.size();
        instanceCreateInfo.ppEnabledExtensionNames = this->required_instance_extensions.data();
        instanceCreateInfo.enabledLayerCount = this->required_instance_layers.size();
        instanceCreateInfo.ppEnabledLayerNames = this->required_instance_layers.data();

        VK_CHECK(vkCreateInstance(&instanceCreateInfo, nullptr, &this->instance), "vkCreateInstance");

        std::cout << "[VK] Instance created" << std::endl;

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = Instance::debug_callback;
        debugCreateInfo.pUserData = nullptr;

        auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(this->instance, "vkCreateDebugUtilsMessengerEXT");
        VK_CHECK(vkCreateDebugUtilsMessengerEXT(this->instance, &debugCreateInfo, nullptr, &this->debugMessenger), "vkCreateDebugUtilsMessengerEXT");
    }

    Instance::~Instance() {
        auto vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(this->instance, "vkDestroyDebugUtilsMessengerEXT");
        vkDestroyDebugUtilsMessengerEXT(this->instance, this->debugMessenger, nullptr);
        vkDestroyInstance(this->instance, nullptr);
    }

    VkInstance Instance::operator*() {
        return this->instance;
    }
}