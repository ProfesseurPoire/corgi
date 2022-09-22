#define VK_USE_PLATFORM_WIN32_KHR
#include "IndexBuffer.h"
#include "PhysicalDevice.h"
#include "VulkanRenderer.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>
#include <corgi/main/Window.h>
#include <corgi/rendering/Sampler.h>
#include <corgi/rendering/VulkanRenderer.h>
#include <corgi/rendering/vulkan/DepthBuffer.h>
#include <corgi/rendering/vulkan/VulkanConstants.h>
#include <corgi/rendering/vulkan/VulkanFramebuffer.h>
#include <corgi/rendering/vulkan/VulkanMaterial.h>
#include <corgi/rendering/vulkan/VulkanSwapchain.h>
#include <corgi/rendering/vulkan/VulkanUniformBufferObject.h>
#include <corgi/rendering/vulkan/VulkanVertexBuffer.h>
#include <corgi/resources/image.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <corgi/rendering/AbstractTexture.h>

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

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR        capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>   presentModes;
};

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

void cleanup()
{
    // //for(size_t i = 0; i < VulkanConstants::max_in_flight; i++)
    // //{
    // //    vkDestroySemaphore(vulkan_renderer->device_, render_finished_semaphores_[i],
    // //                       nullptr);
    // //    vkDestroySemaphore(vulkan_renderer->device_, image_available_semaphores_[i],
    // //                       nullptr);
    // //    vkDestroyFence(vulkan_renderer->device_, in_flight_fences_[i], nullptr);
    // //}

    // ////if(enableValidationLayers)
    // ////    DestroyDebugUtilsMessengerEXT(vulkan_renderer->instance_, debugMessenger, nullptr);

    // //vkDestroyCommandPool(vulkan_renderer->device_, commandPool, nullptr);

    // //for(auto& framebuffer : vulkan_renderer->swapchain_.framebuffers)
    // //    vkDestroyFramebuffer(vulkan_renderer->device_, framebuffer.framebuffer(),
    // //                         nullptr);

    // //vkDestroyPipeline(vulkan_renderer->device_, pipeline.pipeline, nullptr);
    // //vkDestroyPipelineLayout(vulkan_renderer->device_, pipeline.pipeline_layout, nullptr);
    // //vkDestroyRenderPass(vulkan_renderer->device_, vulkan_renderer->render_pass_.render_pass, nullptr);

    // //for(auto imageView : vulkan_renderer->swapchain_.images_view)
    // //    vkDestroyImageView(vulkan_renderer->device_, imageView, nullptr);

    // //vulkan_renderer->swapchain_.finalize(vulkan_renderer->device_);
    // //vkDestroySurfaceKHR(vulkan_renderer->instance_, vulkan_renderer->surface_, nullptr);
    // //vkDestroyDevice(vulkan_renderer->device_, nullptr);
    // //vkDestroyInstance(vulkan_renderer->instance_, nullptr);
    // SDL_Quit();
}

int main(int argc, char** argv)
{
    corgi::Window::CreateInfo window_info;
    window_info.title       = "Vulkan";
    window_info.x           = 100;
    window_info.y           = 100;
    window_info.width       = 1600;
    window_info.height      = 900;
    window_info.monitor     = 0;
    window_info.graphic_api = corgi::Window::GraphicAPI::Vulkan;
    window_info.fullscreen  = false;
    window_info.borderless  = false;
    window_info.vsync       = true;

    corgi::Window window(window_info);

    vulkan_renderer = new VulkanRenderer(static_cast<SDL_Window*>(window.sdl_window()));

    corgi::VulkanRenderer vr;
    vr.set_current_window(&window);
    vr.physical_device_ = vulkan_renderer->physical_device_.vulkan_device();
    vr.device_          = vulkan_renderer->device_;
    vr.command_pool     = vulkan_renderer->commandPool;
    vr.graphics_queue   = vulkan_renderer->graphicsQueue;

    auto vertex_shader =
        vr.create_shader(corgi::Shader::Stage::Vertex, "shaders/vert.spv");

    auto fragment_shader =
        vr.create_shader(corgi::Shader::Stage::Fragment, "shaders/frag.spv");
    
    corgi::Image img("resources/corgi.img");

    corgi::AbstractTexture::CreateInfo texture_info;
    texture_info.format          = corgi::AbstractTexture::Format::RGBA;
    texture_info.internal_format = corgi::AbstractTexture::InternalFormat::RGBA;
    texture_info.data_type       = corgi::AbstractTexture::DataType::UnsignedByte;
    texture_info.width           = img.width();
    texture_info.height          = img.height();
    texture_info.data            = img.pixels();

    auto texture = vr.create_texture(texture_info);

    //auto image1 = vulkan_renderer->create_image("corgi.img");

   // ImageView image_view_1;

    VkSampler sampler_1;

    std::vector<Vertex> vertices = {{-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f},
                                    {0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f},
                                    {0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
                                    {-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}};

    const std::vector<uint16_t> indexes = {0, 1, 2, 0, 2, 3};

    Mesh mesh;

    std::vector<std::pair<Mesh, corgi::VulkanMaterial*>> meshes;

    TestUniform uniform;
    uniform.proj = corgi::Matrix::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -100.0f, 100.0f);

    // This should be removed later on since it's the corgi::VulkanRenderer that will init
    // everything but for now this stays because there's more important cleaning to do

    auto ubo_test1 = vr.create_ubo(&uniform, sizeof(uniform),
                                   corgi::UniformBufferObject::ShaderStage::Vertex);


    corgi::Sampler::CreateInfo sampler_info;
    sampler_info.binding = 2;
    sampler_info.texture = texture;

    auto sampler1 = vr.create_sampler(sampler_info);

    corgi::AbstractMaterial::Descriptor descriptor;
    descriptor.fragment_shader = fragment_shader;
    descriptor.vertex_shader   = vertex_shader;
    descriptor.samplers.push_back(sampler1);

    auto material              = vr.create_material(descriptor);
    material->ubos.push_back(ubo_test1);

    material->render_pass = vulkan_renderer->render_pass_;
    material->swapchain   = vulkan_renderer->swapchain_;

    material->init();

    // Probably the part handling the mesh creation. I should probably move it elsewhere
    mesh.vb = vulkan_renderer->create_vertex_buffer(vertices);
    mesh.ib = vulkan_renderer->create_index_buffer(indexes);

    meshes.push_back({mesh, material});

    static auto startTime = std::chrono::high_resolution_clock::now();

    bool quit = false;

    while(!quit)
    {
        SDL_Event sdl_event;
        while(SDL_PollEvent(&sdl_event))
        {
            switch(sdl_event.type)
            {
                case SDL_QUIT:
                    quit = true;
                    break;
            }
        }

        auto currentTime = std::chrono::high_resolution_clock::now();

        float time = std::chrono::duration<float, std::chrono::seconds::period>(
                         currentTime - startTime)
                         .count();

        uniform.model = corgi::Matrix::translation(0.0f, 0.0f, 0.0f) *
                        corgi::Matrix::euler_angles(0.0f, 0.0f, time);

        material->update();

        vulkan_renderer->draw(meshes);

        window.swap_buffers();
    }
    vkDeviceWaitIdle(vulkan_renderer->device_);

    return 0;
}