#include <iostream>
#include <vulkan/vulkan.h>
#include <fstream>
#include "util.h"

namespace hs::ren {
    void VK_CHECK(const VkResult res, const char* fn) {
        if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
            std::cerr << "[VK] Error (" << fn << "): " << VK_RESULT_CODE(res) << std::endl;
            abort();
        }
    }

    const char* VK_RESULT_CODE(const VkResult res) {
        const char* code = "";
        switch (res) {
            case VK_SUCCESS:
                code = "VK_SUCCESS";
                break;
            case VK_NOT_READY:
                code = "VK_NOT_READY";
                break;
            case VK_TIMEOUT:
                code = "VK_TIMEOUT";
                break;
            case VK_EVENT_SET:
                code = "VK_EVENT_SET";
                break;
            case VK_EVENT_RESET:
                code = "VK_EVENT_RESET";
                break;
            case VK_INCOMPLETE:
                code = "VK_INCOMPLETE";
                break;
            case VK_ERROR_OUT_OF_HOST_MEMORY:
                code = "VK_ERROR_OUT_OF_HOST_MEMORY";
                break;
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                code = "VK_ERROR_OUT_OF_DEVICE_MEMORY";
                break;
            case VK_ERROR_INITIALIZATION_FAILED:
                code = "VK_ERROR_INITIALIZATION_FAILED";
                break;
            case VK_ERROR_DEVICE_LOST:
                code = "VK_ERROR_DEVICE_LOST";
                break;
            case VK_ERROR_MEMORY_MAP_FAILED:
                code = "VK_ERROR_MEMORY_MAP_FAILED";
                break;
            case VK_ERROR_LAYER_NOT_PRESENT:
                code = "VK_ERROR_LAYER_NOT_PRESENT";
                break;
            case VK_ERROR_EXTENSION_NOT_PRESENT:
                code = "VK_ERROR_EXTENSION_NOT_PRESENT";
                break;
            case VK_ERROR_FEATURE_NOT_PRESENT:
                code = "VK_ERROR_FEATURE_NOT_PRESENT";
                break;
            case VK_ERROR_INCOMPATIBLE_DRIVER:
                code = "VK_ERROR_INCOMPATIBLE_DRIVER";
                break;
            case VK_ERROR_TOO_MANY_OBJECTS:
                code = "VK_ERROR_TOO_MANY_OBJECTS";
                break;
            case VK_ERROR_FORMAT_NOT_SUPPORTED:
                code = "VK_ERROR_FORMAT_NOT_SUPPORTED";
                break;
            case VK_ERROR_FRAGMENTED_POOL:
                code = "VK_ERROR_FRAGMENTED_POOL";
                break;
            case VK_ERROR_UNKNOWN:
                code = "VK_ERROR_UNKNOWN";
                break;
            case VK_ERROR_OUT_OF_POOL_MEMORY:
                code = "VK_ERROR_OUT_OF_POOL_MEMORY";
                break;
            case VK_ERROR_INVALID_EXTERNAL_HANDLE:
                code = "VK_ERROR_INVALID_EXTERNAL_HANDLE";
                break;
            case VK_ERROR_FRAGMENTATION:
                code = "VK_ERROR_FRAGMENTATION";
                break;
            case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
                code = "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
                break;
            case VK_PIPELINE_COMPILE_REQUIRED:
                code = "VK_PIPELINE_COMPILE_REQUIRED";
                break;
            case VK_ERROR_SURFACE_LOST_KHR:
                code = "VK_ERROR_SURFACE_LOST_KHR";
                break;
            case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
                code = "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
                break;
            case VK_SUBOPTIMAL_KHR:
                code = "VK_SUBOPTIMAL_KHR";
                break;
            case VK_ERROR_OUT_OF_DATE_KHR:
                code = "VK_ERROR_OUT_OF_DATE_KHR";
                break;
            case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
                code = "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
                break;
            case VK_ERROR_VALIDATION_FAILED_EXT:
                code = "VK_ERROR_VALIDATION_FAILED_EXT";
                break;
            case VK_ERROR_INVALID_SHADER_NV:
                code = "VK_ERROR_INVALID_SHADER_NV";
                break;
            case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:
                code = "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
                break;
            case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:
                code = "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
                break;
            case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:
                code = "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
                break;
            case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:
                code = "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
                break;
            case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:
                code = "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
                break;
            case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:
                code = "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
                break;
            case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
                code = "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
                break;
            case VK_ERROR_NOT_PERMITTED_KHR:
                code = "VK_ERROR_NOT_PERMITTED_KHR";
                break;
            case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
                code = "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
                break;
            case VK_THREAD_IDLE_KHR:
                code = "VK_THREAD_IDLE_KHR";
                break;
            case VK_THREAD_DONE_KHR:
                code = "VK_THREAD_DONE_KHR";
                break;
            case VK_OPERATION_DEFERRED_KHR:
                code = "VK_OPERATION_DEFERRED_KHR";
                break;
            case VK_OPERATION_NOT_DEFERRED_KHR:
                code = "VK_OPERATION_NOT_DEFERRED_KHR";
                break;
            case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
                code = "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
                break;
            case VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT:
                code = "VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT";
                break;
            case VK_RESULT_MAX_ENUM:
                code = "VK_RESULT_MAX_ENUM";
                break;
        }
        return code;
    }

    std::vector<char> read_file(const std::string& path) {
        std::ifstream file = std::ifstream(path, std::ios::ate | std::ios::binary);
        size_t size = file.tellg();
        std::vector<char> buf = std::vector<char>(size);
        file.seekg(0);
        file.read(buf.data(), size);
        file.close();
        return buf;
    }
}