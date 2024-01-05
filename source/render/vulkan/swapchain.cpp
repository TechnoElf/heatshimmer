#include <iostream>
#include "swapchain.h"
#include "../util.h"

namespace hs::ren::vk {
    Swapchain::Swapchain(Device &device) : device(device) {
        uint32_t min_image_count = this->device.details().capabilities.minImageCount + 1;
        if (this->device.details().capabilities.maxImageCount > 0) {
            if (min_image_count > this->device.details().capabilities.maxImageCount) {
                min_image_count = this->device.details().capabilities.maxImageCount;
            }
        }

        VkSwapchainCreateInfoKHR swapchain_create_info = {};
        swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_create_info.surface = this->device.surface();
        swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchain_create_info.imageFormat = this->device.details().surface_formats[this->device.details().selected_surface_format].format;
        swapchain_create_info.imageColorSpace = this->device.details().surface_formats[this->device.details().selected_surface_format].colorSpace;
        swapchain_create_info.imageExtent = this->device.details().swap_extent;
        swapchain_create_info.imageArrayLayers = 1;
        swapchain_create_info.minImageCount = min_image_count;
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_create_info.queueFamilyIndexCount = 0;
        swapchain_create_info.pQueueFamilyIndices = nullptr;
        swapchain_create_info.preTransform = this->device.details().capabilities.currentTransform;
        swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchain_create_info.presentMode = this->device.details().present_modes[this->device.details().selected_present_mode];
        swapchain_create_info.clipped = VK_TRUE;
        swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

        check(vkCreateSwapchainKHR(*this->device, &swapchain_create_info, nullptr, &this->swapchain), "vkCreateSwapchainKHR");

        uint32_t image_count = 0;
        check(vkGetSwapchainImagesKHR(*this->device, this->swapchain, &image_count, nullptr), "vkGetSwapchainImagesKHR");
        this->swapchain_images = std::vector<VkImage>(image_count);
        check(vkGetSwapchainImagesKHR(*this->device, this->swapchain, &image_count, this->swapchain_images.data()), "vkGetSwapchainImagesKHR");

        this->image_views = std::vector<VkImageView>(image_count);
        for (uint32_t i = 0; i < image_count; i++) {
            VkImageViewCreateInfo image_view_create_info = {};
            image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_create_info.image = this->swapchain_images[i];
            image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            image_view_create_info.format = this->device.details().surface_formats[this->device.details().selected_surface_format].format;
            image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_view_create_info.subresourceRange.baseMipLevel = 0;
            image_view_create_info.subresourceRange.levelCount = 1;
            image_view_create_info.subresourceRange.baseArrayLayer = 0;
            image_view_create_info.subresourceRange.layerCount = 1;
            image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            check(vkCreateImageView(*this->device, &image_view_create_info, nullptr, &this->image_views[i]), "vkCreateImageView");
        }

        VkAttachmentDescription color_attachment = {};
        color_attachment.format = this->device.details().surface_formats[this->device.details().selected_surface_format].format;
        color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment_ref = {};
        color_attachment_ref.attachment = 0;
        color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass_description = {};
        subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass_description.colorAttachmentCount = 1;
        subpass_description.pColorAttachments = &color_attachment_ref;

        VkSubpassDependency subpass_dependency = {};
        subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpass_dependency.srcAccessMask = 0;
        subpass_dependency.dstSubpass = 0;
        subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo render_pass_create_info = {};
        render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_create_info.attachmentCount = 1;
        render_pass_create_info.pAttachments = &color_attachment;
        render_pass_create_info.subpassCount = 1;
        render_pass_create_info.pSubpasses = &subpass_description;
        render_pass_create_info.dependencyCount = 1;
        render_pass_create_info.pDependencies = &subpass_dependency;

        check(vkCreateRenderPass(*this->device, &render_pass_create_info, nullptr, &this->render_pass), "vkCreateRenderPass");

        std::cout << "[VK] Swapchain created" << std::endl;

        this->framebuffers = std::vector<VkFramebuffer>(image_views.size());

        for (uint32_t i = 0; i < image_views.size(); ++i) {
            VkFramebufferCreateInfo framebuffer_create_info = {};
            framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_create_info.renderPass = this->render_pass;
            framebuffer_create_info.attachmentCount = 1;
            framebuffer_create_info.pAttachments = &image_views[i];
            framebuffer_create_info.width = this->device.details().swap_extent.width;
            framebuffer_create_info.height = this->device.details().swap_extent.height;
            framebuffer_create_info.layers = 1;

            check(vkCreateFramebuffer(*this->device, &framebuffer_create_info, nullptr, &this->framebuffers[i]), "vkCreateFramebuffer");
        }

        VkCommandPoolCreateInfo command_pool_create_info = {};
        command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        command_pool_create_info.queueFamilyIndex = this->device.details().selected_queue_family;

        check(vkCreateCommandPool(*this->device, &command_pool_create_info, nullptr, &this->command_pool), "vkCreateCommandPool");

        VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
        command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        command_buffer_allocate_info.commandPool = this->command_pool;
        command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        command_buffer_allocate_info.commandBufferCount = 1;

        check(vkAllocateCommandBuffers(*this->device, &command_buffer_allocate_info, &this->command_buffer), "vkAllocateCommandBuffers");

        std::cout << "[VK] Command buffer created" << std::endl;
    }

    Swapchain::~Swapchain() {
        vkDestroyRenderPass(*this->device, this->render_pass, nullptr);
        vkDestroyCommandPool(*this->device, this->command_pool, nullptr);
        for (VkFramebuffer& framebuffer : this->framebuffers) {
            vkDestroyFramebuffer(*this->device, framebuffer, nullptr);
        }
        for (VkImageView& view : this->image_views) {
            vkDestroyImageView(*this->device, view, nullptr);
        }
        vkDestroySwapchainKHR(*this->device, this->swapchain, nullptr);
    }

    VkSwapchainKHR Swapchain::operator*() {
        return this->swapchain;
    }

    VkCommandBuffer Swapchain::command() {
        return this->command_buffer;
    }

    std::vector<VkFramebuffer>& Swapchain::frames() {
        return this->framebuffers;
    }

    VkRenderPass Swapchain::pass() {
        return this->render_pass;
    }

    VkCommandPool Swapchain::pool() {
        return this->command_pool;
    }
}