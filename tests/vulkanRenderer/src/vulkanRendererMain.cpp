#define VK_USE_PLATFORM_WIN32_KHR
#include "Buffer.h"
#include "DepthBuffer.h"
#include "Framebuffer.h"
#include "Image.h"
#include "IndexBuffer.h"
#include "PhysicalDevice.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "Texture.h"
#include "UniformBufferObject.h"
#include "Vertex.h"
#include "VulkanRenderer.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>
#include <Swapchain.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <span>
#include <vector>

// TODO : Inspect this :
// I'm disabling the main SDL macro because i'm stumbling upon a undefined reference
// thing and I don't really understand why since the rest of the SDL seems to be
// working nicely
#ifdef main
#    undef main
#endif /* main */

VulkanRenderer* vulkan_renderer = nullptr;

SDL_Window*     window {nullptr};
VkDevice        device;
VkQueue         graphicsQueue;
VkQueue         presentQueue;
Swapchain       swapchain;
RenderPass      renderPass;
VkCommandPool   commandPool;
VkCommandBuffer commandBuffer;
VkSemaphore     imageAvailableSemaphore;
VkSemaphore     renderFinishedSemaphore;
VkFence         inFlightFence;
Pipeline        pipeline;
VkBuffer        vertexBuffer;

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR        capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>   presentModes;
};

std::vector<Vertex> vertices = {{-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f},
                                {0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f},
                                {0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
                                {-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}};

const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

bool checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                         availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(),
                                             deviceExtensions.end());

    for(const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

void create_command_pool()
{
    VkCommandPoolCreateInfo poolInfo {};
    poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = vulkan_renderer->graphic_family_index_.value();

    if(vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }
}

void create_vulkan_device(VkPhysicalDevice physical_device, unsigned graphic_queue_index)
{
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    std::set<unsigned> unique_family_index = {
        vulkan_renderer->graphic_family_index_.value(),
        vulkan_renderer->present_family_index_.value()};

    for(auto family_index : unique_family_index)
    {
        VkDeviceQueueCreateInfo queueCreateInfo {};
        queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = family_index;
        queueCreateInfo.queueCount       = 1;
        float queuePriority              = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures {};
    // TODO : Check if the device is suitable
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos    = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = queueCreateInfos.size();

    // Adding extensions to the Device
    // We need VK_KHR_SWAPCHAIN_EXTENSION_NAME to draw things
    createInfo.enabledExtensionCount   = deviceExtensions.size();
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    if(vkCreateDevice(physical_device, &createInfo, nullptr, &device) != VK_SUCCESS)
        throw std::runtime_error("failed to create logical device!");

    vkGetDeviceQueue(device, graphic_queue_index, 0, &graphicsQueue);
    vkGetDeviceQueue(device, vulkan_renderer->present_family_index_.value(), 0,
                     &presentQueue);
}

void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags            = 0;          // Optional
    beginInfo.pInheritanceInfo = nullptr;    // Optional

    if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo {};
    renderPassInfo.sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass  = renderPass.render_pass;
    renderPassInfo.framebuffer = swapchain.framebuffers[imageIndex].framebuffer();

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapchain.create_info.imageExtent;

    VkClearValue clearColor        = {{{0.0f, 1.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues    = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);

    VkViewport viewport {};
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = static_cast<float>(swapchain.create_info.imageExtent.width);
    viewport.height   = static_cast<float>(swapchain.create_info.imageExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor {};
    scissor.offset = {0, 0};
    scissor.extent = swapchain.create_info.imageExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkBuffer     vertexBuffers[] = {vertexBuffer};
    VkDeviceSize offsets[]       = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipeline.pipeline_layout, 0, 1,
                            &UniformBufferObject::descriptorSets[0], 0, nullptr);

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void createCommandBuffer()
{
    VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool        = commandPool;
    allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if(vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void createSyncObjects()
{
    VkSemaphoreCreateInfo semaphoreInfo {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fenceInfo {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    if(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) !=
           VK_SUCCESS ||
       vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) !=
           VK_SUCCESS ||
       vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create semaphores!");
    }
}

void create_vulkan_instance()
{
    vulkan_renderer = new VulkanRenderer(window);

    create_vulkan_device(vulkan_renderer->physical_device_.vulkan_device(),
                         vulkan_renderer->graphic_queue_index_);

    swapchain.initialize(vulkan_renderer->physical_device_.vulkan_device(), device,
                         vulkan_renderer->surface_, window,
                         vulkan_renderer->graphic_family_index_,
                         vulkan_renderer->present_family_index_);

    renderPass.initialize(device, vulkan_renderer->physical_device_.vulkan_device(),
                          swapchain.create_info.imageFormat);

    DepthBuffer::createDepthResources(swapchain, device,
                                      vulkan_renderer->physical_device_.vulkan_device());

    swapchain.initialize_framebuffers(DepthBuffer::depthImageView, device,
                                      renderPass.render_pass);

    UniformBufferObject::createDescriptorSetLayout(device);

    pipeline.initialize(device, renderPass.render_pass, swapchain);

    create_command_pool();

    Image::create_texture_image(device, vulkan_renderer->physical_device_.vulkan_device(),
                                commandPool, graphicsQueue);

    Texture::create_texture_image_view(device);
    Texture::createTextureSampler(device,
                                  vulkan_renderer->physical_device_.vulkan_device());

    vertexBuffer = Vertex::create_vertex_buffer(
        vulkan_renderer->physical_device_.vulkan_device(), device, vertices);

    IndexBuffer::create(graphicsQueue, commandPool, device,
                        vulkan_renderer->physical_device_.vulkan_device());

    UniformBufferObject::create_uniform_buffers(
        device, vulkan_renderer->physical_device_.vulkan_device());
    UniformBufferObject::create_descriptor_pool(device);
    UniformBufferObject::create_descriptor_sets(device);

    createCommandBuffer();
    createSyncObjects();
}

void init_vulkan()
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("test", 100, 100, 1600, 900,
                              SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);

    create_vulkan_instance();
}

void draw_frame()
{
    // Before drawing, we need to wait for the fence to be available
    // Vulkan is designed with multithreading in mind, fences are like mutex
    // Fences are used for CPU operations, whereas Semaphore are used for GPU operations
    vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &inFlightFence);

    // We ask the swapchain the image we're going to use for our drawing operations
    // The semaphore will get signaled when we actually acquire the image
    // Though apparently the function will wait until it at least knows the index of
    // the image we're going to draw on
    uint32_t imageIndex;
    vkAcquireNextImageKHR(device, swapchain.swapchain, UINT64_MAX,
                          imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    UniformBufferObject::updateUniformBuffer(device, 0);

    // We clear the command buffer
    vkResetCommandBuffer(commandBuffer, 0);

    // We run our commands
    recordCommandBuffer(commandBuffer, imageIndex);

    // We submit our commands
    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // Here we specify the semaphore that needs to be signaled for the queue operation
    // to be launched
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};

    // Here we define the semaphore we need to wait for
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount     = 1;
    submitInfo.pWaitSemaphores        = waitSemaphores;
    submitInfo.pWaitDstStageMask      = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer;

    // And here we define the semaphore we're going to signal when the operation succeed
    VkSemaphore signalSemaphores[]  = {renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = signalSemaphores;

    // VkQueueSubmit being asynchronous, we need to use semaphores.
    // The function is asynchronous, but on the GPU side, it will wait for the
    // imageAvailableSemaphore to be signaled by the vkAcquireNextImageKHR function
    // Fence will also be signaled. I guess it's fine since we'll only
    if(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS)
        throw std::runtime_error("failed to submit draw command buffer!");

    // We tell the queuePresent operation to wait for the signalSemaphore
    VkPresentInfoKHR presentInfo {};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapchain.swapchain};
    presentInfo.swapchainCount  = 1;
    presentInfo.pSwapchains     = swapChains;
    presentInfo.pImageIndices   = &imageIndex;
    presentInfo.pResults        = nullptr;    // Optional

    // We give our image to the presentation queue
    vkQueuePresentKHR(presentQueue, &presentInfo);
}

void main_loop()
{
    bool exit = false;

    while(!exit)
    {
        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            switch(e.type)
            {
                case SDL_QUIT:
                    exit = true;
                    break;
            }
        }
        draw_frame();
        SDL_GL_SwapWindow(window);
    }
    vkDeviceWaitIdle(device);
}

void cleanup()
{
    //if(enableValidationLayers)
    //    DestroyDebugUtilsMessengerEXT(vulkan_renderer->instance_, debugMessenger, nullptr);

    vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
    vkDestroyFence(device, inFlightFence, nullptr);
    vkDestroyCommandPool(device, commandPool, nullptr);

    for(auto& framebuffer : swapchain.framebuffers)
        vkDestroyFramebuffer(device, framebuffer.framebuffer(), nullptr);

    vkDestroyPipeline(device, pipeline.pipeline, nullptr);
    vkDestroyPipelineLayout(device, pipeline.pipeline_layout, nullptr);
    vkDestroyRenderPass(device, renderPass.render_pass, nullptr);

    for(auto imageView : swapchain.images_view)
        vkDestroyImageView(device, imageView, nullptr);

    swapchain.finalize(device);
    vkDestroySurfaceKHR(vulkan_renderer->instance_, vulkan_renderer->surface_, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(vulkan_renderer->instance_, nullptr);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char** argv)
{

    init_vulkan();
    main_loop();
    cleanup();
    return 0;
}