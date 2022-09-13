#include <SDL2/SDL_events.h>
#include <corgi/components/Camera.h>
#include <corgi/components/MeshRenderer.h>
#include <corgi/components/SpriteRenderer.h>
#include <corgi/ecs/Entity.h>
#include <corgi/ecs/Scene.h>
#include <corgi/main/Window.h>
#include <corgi/rendering/renderer.h>
#include <corgi/rendering/texture.h>
#include <corgi/systems/RenderingSystem.h>
#include <corgi/systems/SpriteRendererSystem.h>
#include <corgi/systems/TransformSystem.h>
#include <corgi/utils/ResourcesCache.h>
#include <glad/glad.h>

#include <iostream>

int main()
{
    corgi::Window window;
    window.initialize("OpenGL", 100, 100, 500, 500, 0, true);
    window.setBorderless(false);
    window.show();

    ResourcesCache::directories().push_back("resources");
    corgi::Renderer renderer;
    renderer.set_current_window(&window);

    corgi::Scene scene;

    scene.component_maps().add<corgi::Transform>();
    scene.component_maps().add<corgi::Camera>();
    scene.component_maps().add<corgi::MeshRenderer>();
    scene.component_maps().add<corgi::SpriteRenderer>();

    scene.emplace_system<corgi::TransformSystem>(
        scene, *scene.component_maps().get<corgi::Transform>());
    scene.emplace_system<corgi::SpriteRendererSystem>(scene);

    corgi::SpriteRendererSystem::initializeSpriteMesh();

    scene.emplace_system<corgi::RenderingSystem>(
        scene, *scene.component_maps().get<corgi::MeshRenderer>());

    scene.root()->add_component<Transform>();

    auto entity = scene.new_entity("camera");
    auto camera = entity->add_component<corgi::Camera>();
    camera->set_window(window);
    camera->clearColor(corgi::Color(255, 0, 0));
    camera->ortho(-2000, 2000, -2000, 2000, -1000, 1000);
    camera->viewport(0, 0, 500, 500);

    entity->add_component<corgi::Transform>();

    auto t = corgi::ResourcesCache::get<Texture>("goose.tex");

    int quit = false;

    Material* material = corgi::ResourcesCache::get<Material>(
        "corgi/materials/unlit/unlit_simple_texture.mat");

    material->set_uniform("mvp_matrix", Matrix::translation(0.0f, 0.0f, -10.0f));

    Vec2 pivot_value {0.5f, 0.5f};

    SDL_Event e;

    float width  = t->width();
    float height = t->height();

    std::vector<float> vertices({-width, -height, 0.0f,   0.0f,  0.0f,   width, -height,
                                 0.0f,   1.0f,    0.0f,   width, height, 0.0f,  1.0f,
                                 1.0f,   -width,  height, 0.0f,  0.0f,   1.0f});

    std::vector<unsigned> indexes({0, 1, 2, 0, 2, 3});

    auto mesh = Mesh::new_standard_2D_mesh(std::move(vertices), std::move(indexes));
    mesh->build_bounding_volumes();

    auto matrix = camera->projection_matrix() * Matrix::translation(0.0f, 0.0f, -10.0f) *
                  corgi::Matrix::euler_angles(0.0f, 0.0f, 0.0f);

    auto ubo = renderer.create_ubo(&matrix, sizeof(corgi::Matrix),
                                   UniformBufferObject::ShaderStage::Vertex);

    material->ubo = ubo;
    Sampler sampler;
    sampler.binding      = 1;
    sampler.texture_name = t->id();
    material->samplers.push_back(sampler);

    // TODO : This kinda looks like the sampler lmao

    static auto startTime = std::chrono::high_resolution_clock::now();
    while(!quit)
    {
        while(SDL_PollEvent(&e))
        {
            switch(e.type)
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

        renderer.initialize_camera(Matrix(), camera.get());

        glClearStencil(0);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glStencilMask(0xFF);

        renderer.clear();

        material->set_texture(0, *t);

        ubo->update();

        matrix = camera->projection_matrix() * Matrix::translation(0.0f, 0.0f, -10.0f) *
                 corgi::Matrix::euler_angles(0.0f, 0.0f, time);

        material->enable_depth_test(true);
        material->is_transparent(true);

        renderer.draw(*mesh.get(), *material);

        window.swap_buffers();
    }
}