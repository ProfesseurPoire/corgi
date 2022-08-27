#define VK_USE_PLATFORM_WIN32_KHR
#include "Buffer.h"
#include "DepthBuffer.h"
#include "Framebuffer.h"
#include "Image.h"
#include "IndexBuffer.h"
#include "PhysicalDevice.h"
#include "Pipeline.h"
#include "ImageView.h"
#include "UniformBufferObject.h"
#include "VertexBuffer.h"
#include "VulkanConstants.h"
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

SDL_Window* window {nullptr};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR        capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>   presentModes;
};

const std::vector<uint16_t> indexes = {0, 1, 2, 2, 3, 0};

std::vector<Vertex> vertices = {{-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f},
                                {0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f},
                                {0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
                                {-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}};

// bool checkDeviceExtensionSupport(VkPhysicalDevice device)
// {
//     uint32_t extensionCount;
//     vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

//     std::vector<VkExtensionProperties> availableExtensions(extensionCount);
//     vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
//                                          availableExtensions.data());

//     std::set<std::string> requiredExtensions(deviceExtensions.begin(),
//                                              deviceExtensions.end());

//     for(const auto& extension : availableExtensions)
//     {
//         requiredExtensions.erase(extension.extensionName);
//     }

//     return requiredExtensions.empty();
// }

uint32_t currentFrame = 0;

std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

// The uniforms
struct TestUniform
{
    corgi::Matrix model;
    corgi::Matrix view;
    corgi::Matrix proj;
};

TestUniform uniform;
TestUniform uniform2;

UniformBufferObject ubo1;
UniformBufferObject ubo2;

Image image1;
Image image2;

ImageView image_view_1;
ImageView image_view_2;

VkSampler sampler_1;
VkSampler sampler_2;

void create_vulkan_instance()
{
    vulkan_renderer = new VulkanRenderer(window);

    image1 = vulkan_renderer->create_image("corgi.png");
    image2 = vulkan_renderer->create_image("goose.png");

    image_view_1 = ImageView::create_texture_image_view(image1, vulkan_renderer->device_);
    image_view_2 = ImageView::create_texture_image_view(image2, vulkan_renderer->device_);

    sampler_1 = ImageView::createTextureSampler(vulkan_renderer->device_, vulkan_renderer->physical_device_.vulkan_device());
    sampler_2 = ImageView::createTextureSampler(
        vulkan_renderer->device_, vulkan_renderer->physical_device_.vulkan_device());

    ubo1 = vulkan_renderer->add_uniform_buffer_object(
        &uniform, sizeof(uniform), UniformBufferObject::ShaderStage::Vertex, 0, image_view_1, sampler_1);

    ubo2 = vulkan_renderer->add_uniform_buffer_object(
        &uniform2, sizeof(uniform), UniformBufferObject::ShaderStage::Vertex, 0, image_view_2, sampler_2);
}

Mesh                                   mesh;
Mesh                                   mesh2;
Pipeline                               pipeline;
Pipeline                               pipeline2;
std::vector<std::pair<Mesh, Pipeline>> meshes;

void init_vulkan()
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("test", 100, 100, 1600, 900,
                              SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);

    create_vulkan_instance();

    // Probably the part handling the mesh creation. I should probably move it elsewhere
    mesh.vb  = vulkan_renderer->create_vertex_buffer(vertices);
    mesh.ib  = vulkan_renderer->create_index_buffer(indexes);
    mesh.ubo = ubo1;
    pipeline = vulkan_renderer->create_pipeline(mesh.ubo);

    mesh2.vb  = vulkan_renderer->create_vertex_buffer(vertices);
    mesh2.ib  = vulkan_renderer->create_index_buffer(indexes);
    mesh2.ubo = ubo2;
    pipeline2  = vulkan_renderer->create_pipeline(mesh2.ubo);


    meshes.push_back({mesh, pipeline});
    meshes.push_back({mesh2, pipeline2});
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

        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();

        float time = std::chrono::duration<float, std::chrono::seconds::period>(
                         currentTime - startTime)
                         .count();

        uniform.model = corgi::Matrix::translation(-0.50f, 0.0f, 0.0f) *
                        corgi::Matrix::euler_angles(0.0f, 0.0f, time);

        uniform.view.identity();
        uniform.proj = corgi::Matrix::ortho(-2, 2, -2, 2, -100, 100);

        uniform2.model = corgi::Matrix::translation(0.50f, 0.0f, 0.0f)*
            corgi::Matrix::euler_angles(0.0f, 0.0f, -time);
        uniform2.view.identity();
        uniform2.proj = corgi::Matrix::ortho(-2, 2, -2, 2, -100, 100);

        vulkan_renderer->draw(meshes);
        SDL_GL_SwapWindow(window);
    }
    vkDeviceWaitIdle(vulkan_renderer->device_);
}

void cleanup()
{
    //for(size_t i = 0; i < VulkanConstants::max_in_flight; i++)
    //{
    //    vkDestroySemaphore(vulkan_renderer->device_, render_finished_semaphores_[i],
    //                       nullptr);
    //    vkDestroySemaphore(vulkan_renderer->device_, image_available_semaphores_[i],
    //                       nullptr);
    //    vkDestroyFence(vulkan_renderer->device_, in_flight_fences_[i], nullptr);
    //}

    ////if(enableValidationLayers)
    ////    DestroyDebugUtilsMessengerEXT(vulkan_renderer->instance_, debugMessenger, nullptr);

    //vkDestroyCommandPool(vulkan_renderer->device_, commandPool, nullptr);

    //for(auto& framebuffer : vulkan_renderer->swapchain_.framebuffers)
    //    vkDestroyFramebuffer(vulkan_renderer->device_, framebuffer.framebuffer(),
    //                         nullptr);

    //vkDestroyPipeline(vulkan_renderer->device_, pipeline.pipeline, nullptr);
    //vkDestroyPipelineLayout(vulkan_renderer->device_, pipeline.pipeline_layout, nullptr);
    //vkDestroyRenderPass(vulkan_renderer->device_, vulkan_renderer->render_pass_.render_pass, nullptr);

    //for(auto imageView : vulkan_renderer->swapchain_.images_view)
    //    vkDestroyImageView(vulkan_renderer->device_, imageView, nullptr);

    //vulkan_renderer->swapchain_.finalize(vulkan_renderer->device_);
    //vkDestroySurfaceKHR(vulkan_renderer->instance_, vulkan_renderer->surface_, nullptr);
    //vkDestroyDevice(vulkan_renderer->device_, nullptr);
    //vkDestroyInstance(vulkan_renderer->instance_, nullptr);
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