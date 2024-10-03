#pragma once

#include <vector>
#include <SDL.h>
#include <vulkan/vulkan.h>

namespace hs::ren {
    enum HidEvent {
        HID_NONE,
        HID_QUIT
    };

    class Hid {
        SDL_Window* window = nullptr;
        VkExtent2D win_extent = {800, 800};

        std::vector<const char*> vk_extensions = {};

    public:
        Hid();
        ~Hid();

        HidEvent poll();

        const std::vector<const char*>& get_vk_extensions();
        VkSurfaceKHR create_vk_surface(VkInstance instance);
        const VkExtent2D& get_win_extent();
        bool is_pressed(SDL_Scancode key);
    };
}