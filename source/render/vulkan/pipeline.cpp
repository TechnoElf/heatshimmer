#include "pipeline.h"

#include <iostream>
#include <array>

#include "../util.h"
#include "vertex.h"

namespace hs::ren::vk {
    Pipeline::Pipeline(Device& device, Swapchain& swapchain, std::string shader) : device(device) {
        std::vector<char> vert_shader_code = read_file(shader + ".vert.spv");

        VkShaderModuleCreateInfo vert_shader_create_info = {};
        vert_shader_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        vert_shader_create_info.codeSize = vert_shader_code.size();
        vert_shader_create_info.pCode = reinterpret_cast<const uint32_t*>(vert_shader_code.data());

        VK_CHECK(vkCreateShaderModule(*this->device, &vert_shader_create_info, nullptr, &this->vert_shader), "vkCreateShaderModule");

        std::vector<char> frag_shader_code = read_file(shader + ".frag.spv");

        VkShaderModuleCreateInfo frag_shader_create_info = {};
        frag_shader_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        frag_shader_create_info.codeSize = frag_shader_code.size();
        frag_shader_create_info.pCode = reinterpret_cast<const uint32_t*>(frag_shader_code.data());

        VK_CHECK(vkCreateShaderModule(*this->device, &frag_shader_create_info, nullptr, &this->frag_shader), "vkCreateShaderModule");

        VkPipelineShaderStageCreateInfo vert_stage_create_info = {};
        vert_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vert_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vert_stage_create_info.module = this->vert_shader;
        vert_stage_create_info.pName = "main";

        VkPipelineShaderStageCreateInfo frag_stage_create_info = {};
        frag_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        frag_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        frag_stage_create_info.module = this->frag_shader;
        frag_stage_create_info.pName = "main";

        VkPipelineShaderStageCreateInfo stages[] = {vert_stage_create_info, frag_stage_create_info};

        auto vertex_binding_desc = binding_desc_from_vert_type<Vertex>();
        auto vertex_attribute_descs = attribute_descs_from_vert_type<Vertex>();

        VkPipelineVertexInputStateCreateInfo vertex_input_create_info = {};
        vertex_input_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_create_info.vertexBindingDescriptionCount = 1;
        vertex_input_create_info.pVertexBindingDescriptions = &vertex_binding_desc;
        vertex_input_create_info.vertexAttributeDescriptionCount = vertex_attribute_descs.size();
        vertex_input_create_info.pVertexAttributeDescriptions = vertex_attribute_descs.data();

        VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info = {};
        input_assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly_create_info.primitiveRestartEnable = VK_FALSE;

        std::vector<VkDynamicState> dynamic_states = {};

        VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {};
        dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state_create_info.dynamicStateCount = dynamic_states.size();
        dynamic_state_create_info.pDynamicStates = dynamic_states.data();

        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) this->device.details().swap_extent.width;
        viewport.height = (float) this->device.details().swap_extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor = {};
        scissor.offset = {0, 0};
        scissor.extent = this->device.details().swap_extent;

        VkPipelineViewportStateCreateInfo viewport_state_create_info = {};
        viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state_create_info.viewportCount = 1;
        viewport_state_create_info.pViewports = &viewport;
        viewport_state_create_info.scissorCount = 1;
        viewport_state_create_info.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {};
        rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterization_state_create_info.depthClampEnable = VK_FALSE;
        rasterization_state_create_info.depthBiasEnable = VK_FALSE;
        rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
        rasterization_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
        rasterization_state_create_info.lineWidth = 1.0f;
        rasterization_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterization_state_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;

        VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {};
        multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisample_state_create_info.sampleShadingEnable = VK_FALSE;
        multisample_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState color_blend_attachment_state = {};
        color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment_state.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo color_blend_create_info = {};
        color_blend_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blend_create_info.logicOpEnable = VK_FALSE;
        color_blend_create_info.attachmentCount = 1;
        color_blend_create_info.pAttachments = &color_blend_attachment_state;

        VkDescriptorSetLayoutBinding descriptor_set_layout_binding_uniform = {};
        descriptor_set_layout_binding_uniform.binding = 0;
        descriptor_set_layout_binding_uniform.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_set_layout_binding_uniform.descriptorCount = 1;
        descriptor_set_layout_binding_uniform.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        descriptor_set_layout_binding_uniform.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding descriptor_set_layout_binding_sampler = {};
        descriptor_set_layout_binding_sampler.binding = 1;
        descriptor_set_layout_binding_sampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_set_layout_binding_sampler.descriptorCount = 1;
        descriptor_set_layout_binding_sampler.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        descriptor_set_layout_binding_sampler.pImmutableSamplers = nullptr;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = {descriptor_set_layout_binding_uniform, descriptor_set_layout_binding_sampler};
        VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {};
        descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptor_set_layout_create_info.bindingCount = bindings.size();
        descriptor_set_layout_create_info.pBindings = bindings.data();

        VK_CHECK(vkCreateDescriptorSetLayout(*this->device, &descriptor_set_layout_create_info, nullptr, &this->descriptor_set_layout), "vkCreateDescriptorSetLayout");

        VkPipelineLayoutCreateInfo pipeline_layout_create_info = {};
        pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_create_info.setLayoutCount = 1;
        pipeline_layout_create_info.pSetLayouts = &this->descriptor_set_layout;

        VK_CHECK(vkCreatePipelineLayout(*this->device, &pipeline_layout_create_info, nullptr, &this->pipeline_layout), "vkCreatePipelineLayout");

        VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {};
        graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphics_pipeline_create_info.stageCount = 2;
        graphics_pipeline_create_info.pStages = stages;
        graphics_pipeline_create_info.pVertexInputState = &vertex_input_create_info;
        graphics_pipeline_create_info.pInputAssemblyState = &input_assembly_create_info;
        graphics_pipeline_create_info.pViewportState = &viewport_state_create_info;
        graphics_pipeline_create_info.pRasterizationState = &rasterization_state_create_info;
        graphics_pipeline_create_info.pMultisampleState = &multisample_state_create_info;
        graphics_pipeline_create_info.pDepthStencilState = nullptr;
        graphics_pipeline_create_info.pColorBlendState = &color_blend_create_info;
        graphics_pipeline_create_info.pDynamicState = &dynamic_state_create_info;
        graphics_pipeline_create_info.layout = this->pipeline_layout;
        graphics_pipeline_create_info.renderPass = swapchain.pass();
        graphics_pipeline_create_info.subpass = 0;

        VK_CHECK(vkCreateGraphicsPipelines(*this->device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, nullptr, &this->pipeline), "vkCreateGraphicsPipelines");

        std::cout << "[VK] Pipeline created" << std::endl;
    }

    Pipeline::~Pipeline() {
        std::cout << "[VK] Pipeline destroyed" << std::endl;
        vkDestroyPipeline(*this->device, this->pipeline, nullptr);
        vkDestroyPipelineLayout(*this->device, this->pipeline_layout, nullptr);
        vkDestroyDescriptorSetLayout(*this->device, this->descriptor_set_layout, nullptr);
        vkDestroyShaderModule(*this->device, this->frag_shader, nullptr);
        vkDestroyShaderModule(*this->device, this->vert_shader, nullptr);
    }

    VkPipeline Pipeline::operator*() {
        return this->pipeline;
    }

    VkPipelineLayout Pipeline::layout() {
        return this->pipeline_layout;
    }

    VkDescriptorSetLayout Pipeline::descriptor_layout() {
        return this->descriptor_set_layout;
    }
}