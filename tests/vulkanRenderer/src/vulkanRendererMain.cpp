#define VK_USE_PLATFORM_WIN32_KHR
#include <corgi/rendering/vulkan/DepthBuffer.h>
#include <corgi/rendering/vulkan/VulkanFramebuffer.h>
#include "IndexBuffer.h"
#include "PhysicalDevice.h"
#include <corgi/rendering/vulkan/VulkanPipeline.h>
#include <corgi/rendering/vulkan/VulkanVertexBuffer.h>
#include "VulkanRenderer.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>
#include <corgi/rendering/vulkan/VulkanSwapchain.h>
#include <corgi/rendering/VulkanRenderer.h>
#include <corgi/rendering/vulkan/VulkanConstants.h>
#include <corgi/rendering/vulkan/VulkanMaterial.h>
#include <corgi/rendering/vulkan/VulkanUniformBufferObject.h>
#include <corgi/resources/image.h>
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

corgi::VulkanMaterial* material1;
corgi::VulkanMaterial* material2;

Image image1;
Image image2;

ImageView image_view_1;
ImageView image_view_2;

VkSampler sampler_1;
VkSampler sampler_2;

corgi::VulkanRenderer vr;

void create_vulkan_instance()
{
    vulkan_renderer = new VulkanRenderer(window);

    vr.physical_device_ = vulkan_renderer->physical_device_.vulkan_device();
    vr.device_          = vulkan_renderer->device_;

    image1 = vulkan_renderer->create_image("corgi.img");
    image2 = vulkan_renderer->create_image("enemy.img");

    auto ubo_test1 = vr.create_ubo(&uniform, sizeof(uniform),
                                   corgi::UniformBufferObject::ShaderStage::Vertex);

    auto ubo_test2 = vr.create_ubo(&uniform2, sizeof(uniform),
                                   corgi::UniformBufferObject::ShaderStage::Vertex);

    image_view_1 = ImageView::create_texture_image_view(image1, vulkan_renderer->device_);
    image_view_2 = ImageView::create_texture_image_view(image2, vulkan_renderer->device_);

    corgi::VulkanSampler sampler1;
    sampler1.binding = 1;
    sampler1.init(vulkan_renderer->device_,
                  vulkan_renderer->physical_device_.vulkan_device(), image_view_1);

    corgi::VulkanSampler sampler2;
    sampler2.binding = 1;
    sampler2.init(vulkan_renderer->device_,
                  vulkan_renderer->physical_device_.vulkan_device(), image_view_2);

    corgi::VulkanSampler sampler3;
    sampler3.binding = 2;
    sampler3.init(vulkan_renderer->device_,
                  vulkan_renderer->physical_device_.vulkan_device(), image_view_2);

    corgi::VulkanSampler sampler4;
    sampler4.binding = 2;
    sampler4.init(vulkan_renderer->device_,
                  vulkan_renderer->physical_device_.vulkan_device(), image_view_1);

    sampler_1 = ImageView::createTextureSampler(
        vulkan_renderer->device_, vulkan_renderer->physical_device_.vulkan_device());

    sampler_2 = ImageView::createTextureSampler(
        vulkan_renderer->device_, vulkan_renderer->physical_device_.vulkan_device());

    material1 = vr.create_material(corgi::UniformBufferObject::ShaderStage::Vertex);
    material2 = vr.create_material(corgi::UniformBufferObject::ShaderStage::Vertex);

    material1->samplers.push_back(sampler1);
    material1->samplers.push_back(sampler3);

    material2->samplers.push_back(sampler2);
    material2->samplers.push_back(sampler4);

    material1->ubos.push_back(ubo_test1);
    material2->ubos.push_back(ubo_test2);

    material1->render_pass =vulkan_renderer->render_pass_;
    material1->swapchain = vulkan_renderer->swapchain_;

    material2->render_pass = vulkan_renderer->render_pass_;
    material2->swapchain   = vulkan_renderer->swapchain_;

    material1->init();
    material2->init();
}

Mesh mesh;
Mesh mesh2;


std::vector<std::pair<Mesh, corgi::VulkanMaterial*>> meshes;

void init_vulkan()
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("test", 100, 100, 1600, 900,
                              SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);

    create_vulkan_instance();

    // Probably the part handling the mesh creation. I should probably move it elsewhere
    mesh.vb       = vulkan_renderer->create_vertex_buffer(vertices);
    mesh.ib       = vulkan_renderer->create_index_buffer(indexes);
    

    mesh2.vb       = vulkan_renderer->create_vertex_buffer(vertices);
    mesh2.ib       = vulkan_renderer->create_index_buffer(indexes);
   

    meshes.push_back({mesh, material1});
    meshes.push_back({mesh2, material2});
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

        uniform.model = corgi::Matrix::translation(-0.30f, 0.0f, -0.10f) *
                        corgi::Matrix::euler_angles(0.0f, 0.0f, time);

        uniform.view.identity();
        uniform.proj = corgi::Matrix::ortho(-2, 2, -2, 2, -100, 100);

        uniform2.model = corgi::Matrix::translation(0.30f, 0.0f, -0.20f) *
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