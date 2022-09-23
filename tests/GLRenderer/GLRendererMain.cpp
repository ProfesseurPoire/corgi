#include <SDL2/SDL_events.h>
#include <corgi/components/SpriteRenderer.h>
#include <corgi/components/Transform.h>
#include <corgi/main/Window.h>
#include <corgi/rendering/renderer.h>
#include <corgi/rendering/texture.h>
#include <corgi/systems/RenderingSystem.h>
#include <corgi/utils/ResourcesCache.h>
#include <corgi/resources/image.h>

using namespace corgi;

struct Uniform
{
    Matrix model;
    Matrix view;
    Matrix proj;
};

int main()
{
    corgi::Window::CreateInfo window_info;
    window_info.title       = "OpenGL";
    window_info.x           = 100;
    window_info.y           = 100;
    window_info.width       = 1600;
    window_info.height      = 900;
    window_info.monitor     = 0;
    window_info.graphic_api = corgi::Window::GraphicAPI::OpenGL;
    window_info.fullscreen  = false;
    window_info.borderless  = false;
    window_info.vsync       = true;

    corgi::Window window(window_info);

    corgi::Renderer renderer;
    renderer.set_current_window(&window);

    auto vertex_shader = renderer.create_shader(
        Shader::Stage::Vertex, "resources/corgi/materials/unlit/vert.spv");

    auto fragment_shader = renderer.create_shader(
        Shader::Stage::Fragment, "resources/corgi/materials/unlit/frag.spv");

    corgi::Image img("resources/corgi.img");

    AbstractTexture::CreateInfo texture_info;
    texture_info.format             = AbstractTexture::Format::RGBA;
    texture_info.internal_format    = AbstractTexture::InternalFormat::RGBA;
    texture_info.data_type          = AbstractTexture::DataType::UnsignedByte;
    texture_info.width              = img.width();
    texture_info.height             = img.height();
    texture_info.data               = img.pixels();

    auto corgi_texture = renderer.create_texture(texture_info);

    corgi::Sampler::CreateInfo sampler_info;
    sampler_info.binding = 2;
    sampler_info.texture = corgi_texture;

    auto sampler = renderer.create_sampler(sampler_info);

    AbstractMaterial::Descriptor descriptor;
    descriptor.fragment_shader = fragment_shader;
    descriptor.vertex_shader   = vertex_shader;
    descriptor.samplers.push_back(sampler);

    auto material = renderer.create_material(descriptor);

    float width  = 1.0f;
    float height = 1.0f;

    std::vector<float> vertices({-width, -height, 0.0f,   0.0f,  0.0f,   
                                width, -height,   0.0f,   1.0f,    0.0f,   
                                width, height, 0.0f,  1.0f,
                                 1.0f,   
                                -width,  height, 0.0f,  0.0f,   1.0f});

    std::vector<unsigned> indexes({0, 1, 2, 0, 2, 3});

    auto mesh =
        corgi::Mesh::new_standard_2D_mesh(std::move(vertices), std::move(indexes));
    mesh->build_bounding_volumes();

    Uniform uniform;
    uniform.proj = corgi::Matrix::ortho(-2, 2, -2, 2, -100, 100);

    // Creating the UBO that communicates with the shaders
    UniformBufferObject::CreateInfo uniform_info;
    uniform_info.binding      = 0;
    uniform_info.data         = &uniform;
    uniform_info.size         = sizeof(Uniform);
    uniform_info.shader_stage = UniformBufferObject::ShaderStage::Vertex;

    auto ubo = renderer.create_ubo(uniform_info);

    material->ubo = ubo;

    corgi::Renderer::begin_default_frame_buffer();
    renderer.setClearColor(corgi::Color {0, 0, 0, 255});
    renderer.set_viewport(0, 0, 500, 500);

    material->enable_depth_test(true);
    material->is_transparent(true);

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

        // Probably need to clear too
        renderer.clear();

        uniform.model = corgi::Matrix::translation(0.0f, 0.0f, 0.0f) *
                        corgi::Matrix::euler_angles(0.0f, 0.0f, time);

        material->update();

        renderer.draw(*mesh.get(), *material);

        window.swap_buffers();
    }
}