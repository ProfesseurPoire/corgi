
#include <corgi/rendering/vulkan/VulkanConstants.h>
#include <corgi/rendering/vulkan/VulkanMaterial.h>
#include <corgi/rendering/vulkan/VulkanShader.h>
#include <corgi/rendering/vulkan/VulkanVertexBuffer.h>
#include <corgi/rendering/vulkan/VulkanTexture.h>

using namespace corgi;

VulkanMaterial::VulkanMaterial(VkDevice device, VkPhysicalDevice physical_device)
    : AbstractMaterial()
{
    device_          = device;
    physical_device_ = physical_device;
}

VulkanMaterial::~VulkanMaterial() {}

void VulkanMaterial::set_data(void* data, int size) {}

// void VulkanMaterial::use() {}

void VulkanMaterial::destroy_descriptor_set_layout(VkDevice device)
{
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
}

void VulkanMaterial::create_descriptor_pool(VkDevice device)
{
    std::vector<VkDescriptorPoolSize> poolSizes {ubos.size() + samplers.size()};

    int j = 0;

    for(auto ubo : ubos)
    {
        poolSizes[j].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[j].descriptorCount =
            static_cast<uint32_t>(VulkanConstants::max_in_flight);
        j++;
    }

    for(auto sampler : samplers)
    {
        poolSizes[j].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[j].descriptorCount =
            static_cast<uint32_t>(VulkanConstants::max_in_flight);
        j++;
    }

    VkDescriptorPoolCreateInfo poolInfo {};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes    = poolSizes.data();
    poolInfo.maxSets       = static_cast<uint32_t>(VulkanConstants::max_in_flight);

    if(vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
        throw std::runtime_error("failed to create descriptor pool!");
}

void VulkanMaterial::create_descriptor_sets(VkDevice device)
{
    std::vector<VkDescriptorSetLayout> layouts(VulkanConstants::max_in_flight,
                                               descriptorSetLayout);
    VkDescriptorSetAllocateInfo        allocInfo {};
    allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool     = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(VulkanConstants::max_in_flight);
    allocInfo.pSetLayouts        = layouts.data();

    descriptorSets.resize(VulkanConstants::max_in_flight);

    if(vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate descriptor sets!");

    for(size_t i = 0; i < VulkanConstants::max_in_flight; i++)
    {
        int                               j = 0;
        std::vector<VkWriteDescriptorSet> descriptorWrites {ubos.size() +
                                                            samplers.size()};

        std::vector<VkDescriptorBufferInfo> bufferInfo(ubos.size());

        int loc = 0;
        for(const auto& ubo : ubos)
        {
            // Ok so this assume that I only have 1 uniform block
            bufferInfo[loc].buffer =
                dynamic_cast<corgi::VulkanUniformBufferObject*>(ubo)->uniformBuffers[i];
            bufferInfo[loc].offset = 0;
            bufferInfo[loc].range  = sizeof(UniformBufferObject);

            descriptorWrites[j].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[j].dstSet          = descriptorSets[i];
            descriptorWrites[j].dstBinding      = j;
            descriptorWrites[j].dstArrayElement = 0;
            descriptorWrites[j].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[j].descriptorCount = 1;
            descriptorWrites[j].pBufferInfo     = &bufferInfo[loc];
            j++;
            loc++;
        }

        std::vector<VkDescriptorImageInfo> imageInfo(samplers.size());

        loc = 0;

        for(const auto& sampler : samplers)
        {
            imageInfo[loc].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            auto vs = static_cast<VulkanTexture*>(sampler->texture);

            imageInfo[loc].imageView    = vs->image_view;
            imageInfo[loc].sampler      = sampler->sampler;

            descriptorWrites[j].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[j].dstSet          = descriptorSets[i];
            descriptorWrites[j].dstBinding      = j;
            descriptorWrites[j].dstArrayElement = 0;
            descriptorWrites[j].descriptorType =
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[j].descriptorCount = 1;
            descriptorWrites[j].pImageInfo      = &imageInfo.at(loc);
            loc++;
            j++;
        }
        vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()),
                               descriptorWrites.data(), 0, nullptr);
    }
}

void VulkanMaterial::createDescriptorSetLayout(VkDevice device)
{
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    for(auto ubo : ubos)
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding {};
        uboLayoutBinding.binding = ubo->binding_;
        switch(ubo->shader_stage)
        {
            // Means it's the UBO for the Vertex Shader
            case UniformBufferObject::ShaderStage::Fragment:
                uboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                break;

            // Means it's the UBO for the Vertex Shader
            case UniformBufferObject::ShaderStage::Vertex:
                uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                break;
        }

        uboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount    = 1;
        uboLayoutBinding.pImmutableSamplers = nullptr;    // Optional

        bindings.push_back(uboLayoutBinding);
    }

    for(auto sampler : samplers)
    {
        // Temporary for now
        // TODO CHANGE THAT TOO
        const VkDescriptorSetLayoutBinding samplerLayoutBinding {
            .binding            = static_cast<uint32_t>(sampler->binding),
            .descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount    = 1,
            .stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr};

        bindings.push_back(samplerLayoutBinding);
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo {};
    layoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings    = bindings.data();

    if(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) !=
       VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    init_pipeline();
}

static std::vector<char> read_file(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if(!file.is_open())
    {
        throw std::runtime_error("failed to open file " + filename);
    }

    size_t            fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

static VkShaderModule create_shader_module(VkDevice device, const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo {};
    createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode    = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module!");
    }
    return shaderModule;
}

VulkanMaterial::VulkanMaterial(VkDevice                     device,
                               VkPhysicalDevice             physical_device,
                               AbstractMaterial::Descriptor descriptor,
                               RenderPass                   render_pass,
                               Swapchain                    swapchain)
    : AbstractMaterial()
{
    device_          = device;
    physical_device_ = physical_device;
    this->swapchain  = swapchain;
    this->render_pass = render_pass;

    vertex_shader_   = descriptor.vertex_shader;
    fragment_shader_ = descriptor.fragment_shader;
    for(auto sampler : descriptor.samplers)
    {
        samplers.push_back(static_cast<VulkanSampler*>(sampler));
    }
}

void VulkanMaterial::init_pipeline()
{
    auto* vs = static_cast<VulkanShader*>(vertex_shader_);
    auto* fs = static_cast<VulkanShader*>(fragment_shader_);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo {};
    vertShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vs->shader_module_;
    vertShaderStageInfo.pName  = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo {};
    fragShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fs->shader_module_;
    fragShaderStageInfo.pName  = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                      fragShaderStageInfo};

    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                                 VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicState {};
    dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates    = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
    pipelineLayoutInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts    = &descriptorSetLayout;

    if(vkCreatePipelineLayout(device_, &pipelineLayoutInfo, nullptr, &pipeline_layout) !=
       VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    auto bindingDescription    = VertexBuffer::getBindingDescription();
    auto attributeDescriptions = VertexBuffer::getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount   = 1;
    vertexInputInfo.pVertexBindingDescriptions      = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = 3;
    vertexInputInfo.pVertexAttributeDescriptions    = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
    inputAssembly.sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport {};
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = (float)swapchain.create_info.imageExtent.width;
    viewport.height   = (float)swapchain.create_info.imageExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor {};
    scissor.offset = {0, 0};
    scissor.extent = swapchain.create_info.imageExtent;

    VkPipelineViewportStateCreateInfo viewportState {};
    viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports    = &viewport;
    viewportState.scissorCount  = 1;
    viewportState.pScissors     = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable        = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth               = 1.0f;
    rasterizer.cullMode                = VK_CULL_MODE_NONE;
    rasterizer.frontFace               = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable         = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;    // Optional
    rasterizer.depthBiasClamp          = 0.0f;    // Optional
    rasterizer.depthBiasSlopeFactor    = 0.0f;    // Optional

    VkPipelineMultisampleStateCreateInfo multisampling {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable   = VK_FALSE;
    multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading      = 1.0f;        // Optional
    multisampling.pSampleMask           = nullptr;     // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE;    // Optional
    multisampling.alphaToOneEnable      = VK_FALSE;    // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment {};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable         = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;     // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;    // Optional
    colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;         // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;     // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;    // Optional
    colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;         // Optional

    VkPipelineColorBlendStateCreateInfo colorBlending {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable     = VK_FALSE;
    colorBlending.logicOp           = VK_LOGIC_OP_COPY;    // Optional
    colorBlending.attachmentCount   = 1;
    colorBlending.pAttachments      = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;    // Optional
    colorBlending.blendConstants[1] = 0.0f;    // Optional
    colorBlending.blendConstants[2] = 0.0f;    // Optional
    colorBlending.blendConstants[3] = 0.0f;    // Optional

    VkPipelineDepthStencilStateCreateInfo depthStencil {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable       = VK_TRUE;
    depthStencil.depthWriteEnable      = VK_TRUE;
    depthStencil.depthCompareOp        = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds        = 0.0f;    // Optional
    depthStencil.maxDepthBounds        = 1.0f;    // Optional
    depthStencil.stencilTestEnable     = VK_FALSE;
    depthStencil.front                 = {};    // Optional
    depthStencil.back                  = {};    // Optional

    VkGraphicsPipelineCreateInfo pipelineInfo {};
    pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount          = 2;
    pipelineInfo.pStages             = shaderStages;
    pipelineInfo.pVertexInputState   = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState      = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState   = &multisampling;
    pipelineInfo.pDepthStencilState  = &depthStencil;    // Optional
    pipelineInfo.pColorBlendState    = &colorBlending;
    pipelineInfo.pDynamicState       = &dynamicState;

    pipelineInfo.layout             = pipeline_layout;
    pipelineInfo.renderPass         = render_pass.render_pass;
    pipelineInfo.subpass            = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;    // Optional
    pipelineInfo.basePipelineIndex  = -1;                // Optional

    if(vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr,
                                 &pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }
}

void VulkanMaterial::init()
{
    createDescriptorSetLayout(device_);
    create_descriptor_pool(device_);
    create_descriptor_sets(device_);
    init_pipeline();
}

void VulkanMaterial::update()
{
    for(auto ubo : ubos)
    {
        // I need to update the material for every buffer copy for each
        // image
        for(int i = 0; i < VulkanConstants::max_in_flight; i++)
            ubo->update(i);
    }
}