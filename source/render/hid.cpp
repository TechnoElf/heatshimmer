#include "hid.h"

#include <vector>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <iostream>

namespace hs::ren {
    Hid::Hid() {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        auto window_flags = static_cast<SDL_WindowFlags>(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

        this->window = SDL_CreateWindow("Heatshimmer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, win_extent.width, win_extent.height, window_flags);

        uint32_t extensionCount = 0;
        SDL_Vulkan_GetInstanceExtensions(this->window, &extensionCount, nullptr);
        this->vk_extensions = std::vector<const char*>(extensionCount);
        SDL_Vulkan_GetInstanceExtensions(this->window, &extensionCount, this->vk_extensions.data());
        this->vk_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    Hid::~Hid() {
        SDL_DestroyWindow(this->window);
    }

    HidEvent Hid::poll() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    return HID_QUIT;
                }
                case SDL_WINDOWEVENT: {
                    switch (event.window.event) {
                        default: {
                            break;
                        }
                    }
                    break;
                }
                case SDL_KEYDOWN:
                case SDL_KEYUP: {
                    if (!event.key.repeat) {
                        if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
                            pressed = event.key.state == SDL_PRESSED;
                        }
                    }
                    break;
                }
                default: {
                    break;
                }
            }
        }

        return HID_NONE;
    }

    const std::vector<const char*>& Hid::get_vk_extensions() {
        return this->vk_extensions;
    }

    VkSurfaceKHR Hid::create_vk_surface(VkInstance instance) {
        VkSurfaceKHR surface;
        SDL_Vulkan_CreateSurface(this->window, instance, &surface);
        return surface;
    }

    const VkExtent2D& Hid::get_win_extent() {
        return this->win_extent;
    }

    bool Hid::is_pressed() {
        return this->pressed;
    }
}