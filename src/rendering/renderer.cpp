#include <assert.h>
#include <corgi/components/BoxCollider.h>
#include <corgi/components/BoxCollider2D.h>
#include <corgi/components/Camera.h>
#include <corgi/components/Transform.h>
#include <corgi/ecs/Component.h>
#include <corgi/ecs/Entity.h>
#include <corgi/ecs/Scene.h>
#include <corgi/logger/log.h>
#include <corgi/main/Window.h>
#include <corgi/rendering/FrameBuffer.h>
#include <corgi/rendering/Material.h>
#include <corgi/rendering/ShaderProgram.h>
#include <corgi/rendering/gl/GLUniformBufferObject.h>
#include <corgi/rendering/renderer.h>
#include <corgi/rendering/texture.h>
#include <corgi/resources/Mesh.h>
#include <corgi/resources/image.h>
#include <corgi/systems/RenderingSystem.h>
#include <corgi/utils/Color.h>
#include <corgi/utils/ResourcesCache.h>
#include <glad/glad.h>

#include <fstream>
#include <iostream>

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX   0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

namespace corgi
{
std::string read_file_to_string(const std::string& path)
{
    std::ifstream t(path);
    std::string   str;

    t.seekg(0, std::ios::end);
    str.reserve(std::string::size_type(t.tellg()));
    t.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    return str;
}

/*!
* @brief	Returns the size of a file
*/
[[nodiscard]] static int get_file_size(const std::string& path)
{
    std::ifstream file(path.c_str(), std::ifstream::in | std::ifstream::binary);

    if(file.is_open())
    {
        file.seekg(0, std::ios::end);
        return static_cast<int>(file.tellg());
    }
    return -1;
}

void Renderer::begin_frame_buffer(const FrameBuffer* frameBuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->id_);
}

// Actually sets the default framebuffer
void Renderer::end_frame_buffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

WindowDrawList& Renderer::window_draw_list()
{
    return window_draw_list_;
}

WindowDrawList& Renderer::windowDrawList()
{
    return window_draw_list_;
}

// void Renderer::resize_frame_buffer(FrameBuffer* frameBuffer, int width, int height)
// {
// 	// TODO : Remake this
// 	//frameBuffer->_width = width;
// 	//frameBuffer->_height = height;

// 	//glDeleteFramebuffers(1, &frameBuffer->_frameBufferObjectId);
// 	//glDeleteTextures(1, &frameBuffer->_color_attachment.lock().get()->id());

// 	//glGenFramebuffers(1, &frameBuffer->_frameBufferObjectId);
// 	//glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->_frameBufferObjectId);

// 	//glGenTextures(1, &frameBuffer->_colorAttachmentId);
// 	//glBindTexture(GL_TEXTURE_2D, frameBuffer->_colorAttachmentId);

// 	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frameBuffer->_width, frameBuffer->_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

// 	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
// 	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

// 	////unsigned int rbo;
// 	////glGenRenderbuffers(1, &rbo);
// 	////glBindRenderbuffer(GL_RENDERBUFFER, rbo);

// 	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, frameBuffer->_width, frameBuffer->_height);

// 	////glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
// 	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBuffer->_colorAttachmentId, NULL);

// 	//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
// 	//{
// 	//	//std::cout << "Error lol" << std::endl;
// 	//}
// 	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
// }

static void GLAPIENTRY debug_function(GLenum        source,
                                      GLenum        type,
                                      GLuint        id,
                                      GLenum        severity,
                                      GLsizei       length,
                                      const GLchar* message,
                                      const void*   userParam)
{
    if(id == 131185)    //we just ignore this info (about memory video and VBO)
        return;
    std::map<GLenum, std::string> sources {
        {GL_DEBUG_SOURCE_API, "API"},
        {GL_DEBUG_SOURCE_WINDOW_SYSTEM, "Window System"},
        {GL_DEBUG_SOURCE_SHADER_COMPILER, "Shader compiler"},
        {GL_DEBUG_SOURCE_THIRD_PARTY, "Third party"},
        {GL_DEBUG_SOURCE_APPLICATION, "Application"},
        {GL_DEBUG_SOURCE_OTHER, "Other"}};

    std::map<GLenum, std::string> types {
        {GL_DEBUG_TYPE_ERROR, "Error"},
        {GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, "Deprecated Behavior"},
        {GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, "Undefined Behavior"},
        {GL_DEBUG_TYPE_PORTABILITY, "Portability"},
        {GL_DEBUG_TYPE_PERFORMANCE, "Performance"},
        {GL_DEBUG_TYPE_MARKER, "Marker"},
        {GL_DEBUG_TYPE_PUSH_GROUP, "Push Group"},
        {GL_DEBUG_TYPE_POP_GROUP, "Pop Group"},
        {GL_DEBUG_TYPE_OTHER, "Other"}};

    std::string str;
    str += "From : \"" + sources[source] + "\" ";
    str += "Type : \"" + types[type] + "\" Message : \"";
    str += "ID : \"" + std::to_string(id) + "\" ";
    str += message;
    str += +"\"";

    switch(severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            log_fatal_error(str);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            log_error(str);
            break;
        case GL_DEBUG_SEVERITY_LOW:
            log_warning(str);
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            log_info(str);
            break;
    }
}

Renderer::Renderer()
{
    //post_processing_ = std::make_unique<PostProcessing>();
}

void Renderer::initialize()
{
    glDebugMessageCallback(debug_function, nullptr);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    initialize_opengl_state();
}

void setRectangle(float startX,
                  float startY,
                  float width,
                  float height,
                  float startTextureX,
                  float startTextureY,
                  float textureWidth,
                  float textureHeight)
{
    glTexCoord2f(startTextureX, startTextureY);
    glVertex2f(startX, startY);

    glTexCoord2f(startTextureX + textureWidth, startTextureY);
    glVertex2f(startX + width, startY);

    glTexCoord2f(startTextureX + textureWidth, startTextureY + textureHeight);
    glVertex2f(startX + width, startY + height);

    glTexCoord2f(startTextureX, startTextureY + textureHeight);
    glVertex2f(startX, startY + height);
}

static bool
depth_sorting(Transform& ta, Transform& tb, const Component* a, const Component* b)
{
    /*	Vec3 va = ta.world_position();
            Vec3 vb = tb.world_position();

            if (va.z == vb.z)
                    return va.y > va.y;

            return	va.z < vb.z;*/

    return false;
}

void Renderer::set_current_window(Window* window)
{
    current_window_ = window;
    window_draw_list_.set_current_window(window);
}

void Renderer::draw_collider(ColliderComponent* collider, const Matrix& world_matrix)
{
    static Material material =
        *ResourcesCache::get<Material>("corgi/materials/unlit/unlit_color.mat");

    material.enable_depth_test(false);
    material.enable_blend(false);
    material.polygon_mode(Material::PolygonMode::Line);

    if(collider->colliding)
        material.set_uniform("main_color", Vec4(1.0f, 0.0f, 0.0f, 1.0f));
    else
        material.set_uniform("main_color", Vec4(0.0f, 1.0f, 0.0f, 1.0f));

    begin_material(material);
    draw_mesh(collider->_mesh.get(), _view_projection_matrix * world_matrix);
}

/*PostProcessing* Renderer::post_processing()
    {
            return post_processing_.get();
    }*/

void Renderer::begin_default_frame_buffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Renderer::draw_colliders(Scene& scene)
{
    if(!scene.component_maps().contains<BoxCollider>() || !show_colliders_)
        return;

    int i = 0;

    if(scene.component_maps().contains<BoxCollider2D>())
    {
        for(auto& collider : *scene.component_maps().get<BoxCollider2D>())
        {
            auto  entity_id = EntityId(scene.component_maps()
                                           .get<BoxCollider2D>()
                                           ->component_index_to_entity_id()
                                           .at(i));
            auto& entity    = _current_scene->entity_contiguous().at(entity_id.id_);

            if(collider.is_enabled() && entity.is_enabled())
            {
                // Drawing the edges solid

                Material mat = *ResourcesCache::get<Material>(
                    "corgi/materials/unlit/unlit_color.mat");

                mat.enable_depth_test(false);
                mat.enable_blend(false);
                mat.polygon_mode(Material::PolygonMode::Line);

                if(collider.colliding)
                    mat.set_uniform("main_color", Vec4(1.0f, 0.0f, 0.0f, 1.0f));
                else
                    mat.set_uniform("main_color", Vec4(0.0f, 1.0f, 0.0f, 1.0f));

                begin_material(mat);
                draw_mesh(collider._edges_mesh.get(),
                          _view_projection_matrix *
                              transform_map_->get(entity_id).world_matrix());

                // Drawing the inside transparent
                mat.enable_depth_test(false);
                mat.enable_blend(true);
                mat.polygon_mode(Material::PolygonMode::Fill);

                if(collider.colliding)
                    mat.set_uniform("main_color", Vec4(1.0f, 0.0f, 0.0f, 0.20f));
                else
                    mat.set_uniform("main_color", Vec4(0.0f, 1.0f, 0.0f, 0.20f));
                begin_material(mat);
                draw_mesh(collider._mesh.get(),
                          _view_projection_matrix *
                              transform_map_->get(entity_id).world_matrix());
            }
            i++;
        }
    }

    i = 0;

    for(auto& collider : *scene.component_maps().get<BoxCollider>())
    {
        auto entity_id = EntityId(
            scene.component_maps().get<BoxCollider>()->component_index_to_entity_id().at(
                i));
        auto& entity = _current_scene->entity_contiguous().at(entity_id.id_);

        if(collider.is_enabled() && entity.is_enabled())
        {
            draw_collider(&collider, transform_map_->get(entity_id).world_matrix());
        }
        i++;
    }
}

/*!
     * @brief	Cameras won't draw components linked to a layer that has been referenced in their culling list
     *
     * Also, components are sorted by their material renderqueue integer. Meaning material with a lower value
     * will actually be drawn before ones with a higher value. It's a way to manually control the draw order
     * of certain components
     */
static std::vector<const MeshRenderer*> sort_by_camera_layer(
    Scene& scene, const Camera& camera, const std::vector<MeshRenderer>& meshes)
{
    const auto layers    = camera.culling_layers().layers();
    const auto mesh_size = meshes.size();
    // Warning 6386 might pop out if we don't use exactly mesh_size variable here (like using meshes.size() would proc the warning)
    const MeshRenderer** filtered_meshes = new const MeshRenderer*[mesh_size];

    int i = 0;

    for(int j = 0; j < mesh_size; ++j)
    {
        // This is supposed to sort meshes by their layer
        if(meshes[j].layer & layers)
        {
            // We only display a mesh if both the MeshRenderer Component and the Entity it's attached to is enabled
            if(meshes[j].is_enabled() && meshes[j]._entity_id->is_enabled())
            {
                filtered_meshes[i++] = &meshes[j];
            }
        }
    }

    std::vector<const MeshRenderer*> components(filtered_meshes, filtered_meshes + i);
    delete[] filtered_meshes;
    //components.insert(components.begin(), arrr, i);
    return components;
}

struct MaterialIndex
{
    const Material* material;
    int             index;
};

static int cmpfunc(const void* a, const void* b)
{
    const auto* aa = (MaterialIndex*)(a);
    const auto* bb = (MaterialIndex*)(b);

    return *aa->material < *bb->material;
}

// TODO : This probably needs more granularity, make some stuff into functions
// because it's hard to understand what's actually going on here
void Renderer::draw_scene(Window& window, Scene& scene)
{
    // We simply don't draw anything when the window is minimized
    // because opengl functions don't work anymore

    if(window.isMinimized())
        return;

    // Lots of things aren't really right here, like getting the scene from the
    // game etc

    _current_scene = &scene;

    transform_map_ = scene.component_maps().get<Transform>();

    if(!scene.component_maps().contains<Camera>())
    {
        log_warning("No Camera Component Map found in current scene");
        return;
    }

    if(scene.component_maps().get<Camera>()->empty())
    {
        log_warning("No Camera Component found in the Camera Component Pool");
        return;
    }

    if(!scene.has_system<RenderingSystem>())
    {
        log_warning("No Rendering System found");
        return;
    }

    profiler_.vertices_size  = 0;
    profiler_.vertices_count = 0;
    profiler_.triangle_count = 0;
    profiler_.draw_calls     = 0;

#ifdef WIN32
    GLint current_memory_available = 0;
    glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &current_memory_available);

    GLint total_memory_available_kb = 0;
    glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &total_memory_available_kb);
    profiler_.gpu_memory_used      = total_memory_available_kb - current_memory_available;
    profiler_.gpu_memory_available = total_memory_available_kb;
#endif

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Sort the camera by their order value
    //std::sort(scene.cameras.begin(), scene.cameras.end());

    int i = 0;
    for(auto& camera : *scene.component_maps().get<Camera>())
    {
        // We only draw cameras that shared the current window
        if(camera.get_window()->id() != window.id())
            continue;

        auto entity_id        = scene.component_maps().get<Camera>()->entity_id(i++);
        auto camera_transform = transform_map_->get(entity_id);

        initialize_camera(camera_transform.world_matrix().inverse(), camera);

        if(camera.on_start)
            camera.on_start();

        // Probably should add a flag to the camera too, to tell
        // if it's supposed to use a depth buffer or not

        // glClearStencil will tell opengl to fill the Stencil Buffer
        // with the 0 value when clearing

        glClearStencil(0);
        glStencilMask(0xFF);

        // In case the last drawn material put this to false, we need to put
        // it back to true here otherwise we won't clear the color buffer
        if(!_opengl_state.write_color)
        {
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            _opengl_state.write_color = true;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        //glDisable(GL_SCISSOR_TEST);

        auto& rendering_system = *scene.get_system<RenderingSystem>();

        std::map<Material, std::vector<const MeshRenderer*>> components_by_material;

        auto components =
            sort_by_camera_layer(scene, camera, rendering_system._meshes.components());

        std::vector<const MeshRenderer*> culled_components;
        culled_components.reserve(components.size());

        // Now we remove anything that's not in the viewport?

        float ortho_height = camera.orthographic_height_;

        float max_distance =
            Vec2(ortho_height * camera.ratio(), ortho_height).length() * 1.5f;

        Vec2 camera_position(camera_transform.position().x,
                             camera_transform.position().y);

        // This is the culling part, probably should put it in its own function
        for(auto& cc : components)
        {
            auto  mesh      = cc->_mesh;
            auto& transform = transform_map_->get(EntityId(cc->_entity_id->id()));
            //bool culled = true;

            auto wm = transform.world_matrix();

            auto wmdata = wm.data();

            Vec2 position;

            position.x = mesh->bounding_circle_offset_x + wmdata[12];
            position.y = mesh->bounding_circle_offset_y + wmdata[13];

            // So now I have the actual center of the bounding circle;

            const auto distance = (position - camera_position).length();

            if(distance < (max_distance + mesh->bounding_circle_radius))
            {
                culled_components.push_back(cc);
            }
        }

        components = culled_components;

        //std::cout << "culled component " << culled_components.size()<<std::endl;

        // Sorting components through their material. See Material::operator<()

        //	for(auto* component : components)
        //		components_by_material[component->material].emplace_back(component);

        std::vector<std::vector<const MeshRenderer*>> test_sort;
        std::vector<MaterialIndex>                    test_materials;
        test_materials.reserve(2000);

        for(auto* component : components)
        {
            /*auto r = std::find_if(test_materials.begin(), test_materials.end(), [&](const std::pair<Material,int>&p)
                            {
                                            return p.first == component->material;
                            });*/

            int  min   = 0;
            int  max   = static_cast<int>(test_materials.size()) - 1;
            bool found = false;

            int index;
            while(!found)
            {
                if(max == -1)
                    break;

                index = (min + max) / 2;

                if(*test_materials[index].material == component->material)
                {
                    found = true;
                    break;
                }

                if(min >= max)
                    break;

                if(*test_materials[index].material < component->material)
                {
                    max = index - 1;
                }
                else
                {
                    min = index + 1;
                }
            }

            if(found)
            {
                test_sort[test_materials[index].index].emplace_back(component);
            }
            else
            {
                test_materials.push_back(
                    {&component->material, (int)test_materials.size()});

                qsort(test_materials.data(), test_materials.size(), sizeof(MaterialIndex),
                      cmpfunc);

                /*[]( const std::pair<Material, int>& p1, const std::pair<Material, int>& p2)->bool
                                    {
                                                    return p1.first < p2.first;
                                    }
                                    );*/

                test_sort.emplace_back();
                test_sort.back().reserve(20);
                test_sort.back().push_back(component);
            }
        }

        //// TODO : This is temporary, just because the material sorting has been
        //// desactivated
        //std::sort(components.begin(),components.end(), [](const RendererComponent* a, const RendererComponent* b)->bool
        //{
        //	return a->material.render_queue < b->material.render_queue;
        //});

        for(auto& components : test_sort)    // Drawing components by material
        {
            begin_material(components[0]->material);

            for(auto& mesh_renderer : components)
            {
                auto& transform =
                    transform_map_->get(EntityId(mesh_renderer->_entity_id->id()));
                draw_mesh(mesh_renderer->_mesh.get(),
                          _view_projection_matrix * transform.world_matrix());
            }
        }

        draw_colliders(scene);    // Only drawn if show_collider_ = true
        draw_dl(_world_draw_list);

        if(camera.on_end)
            camera.on_end();
    }
    // Once we draw everything in the world, we draw things on the screen space
    drawScreenSpace(window);
}

void Renderer::drawScreenSpace(Window& window)
{
    // First we return back to the default frame buffer
    begin_default_frame_buffer();

    // We set the viewport to be equal to the window's dimensions
    set_viewport(0, 0, window.width(), window.height());

    // We reset the view matrix
    _view_matrix.identity();

    // Orthographic projection is relative to the window's dimensions
    _projection_matrix =
        Matrix::ortho(0.0f, static_cast<float>(window.width()),
                      static_cast<float>(window.height()), 0.0f, -10.0f, 100.0f);
    // Not sure what this is lol
    _view_projection_matrix = _projection_matrix;

    // And finally we draw everything stored in our window_draw_list
    // I probably need to rename that to "ScreenDrawList" because it's really
    // more related to the screen rather than the "window" but well, semantics
    // the argument is that screen space is an actual term used by opengl so it
    // make it easier to understand what's what
    draw_dl(windowDrawList().draw_list());
}

void Renderer::initialize_camera(const Matrix& inverse, const Camera& camera)
{
    // Set the framebuffer attached to the camera. If the camera
    // has no framebuffer attached, we use the default one
    if(camera.framebuffer_)
        begin_frame_buffer(camera.framebuffer_.get());
    else
        begin_default_frame_buffer();

    setClearColor(camera.clearColor());

    set_viewport(camera.viewport());

    _projection_matrix = camera.projection_matrix();
    _view_matrix       = inverse;

    _view_projection_matrix = _projection_matrix * _view_matrix;
}

static void set_stencil_enum(GLenum& e, StencilOp v)
{
    switch(v)
    {
        case StencilOp::Keep:
            e = GL_KEEP;
            break;
        case StencilOp::Replace:
            e = GL_REPLACE;
            break;
    }
}

UniformBufferObject* Renderer::create_ubo(UniformBufferObject::ShaderStage shader_stage)
{
    auto ubo = new GLUniformBufferObject(shader_stage);
    ubos_.emplace_back(ubo);

    //glUniformBlockBinding(shader_program_id, 0, 0);

    // GLint blockSize;
    // glGetActiveUniformBl =
    //     ockiv(shader_program_id, 0, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

    // GLubyte* blockBuffer;
    // blockBuffer = (GLubyte*)malloc(blockSize);

    // // We must make a buffer and put the data inside it

    // glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
    // glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);

    // // After that I need to use this
    // glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboHandle);

    return ubos_.back().get();
}

void Renderer::initialize_opengl_state()
{
    if(_opengl_state.write_color)
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    else
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    if(_opengl_state.enable_depth_test)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    if(_opengl_state.depth_mask)
        glDepthMask(GL_TRUE);
    else
        glDepthMask(GL_FALSE);

    if(_opengl_state.enable_blend)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
        glDisable(GL_BLEND);

    if(_opengl_state.enable_stencil_test)
    {
        glEnable(GL_STENCIL_TEST);
        glStencilMask(0xFF);
        GLenum sfail;
        GLenum ssuccess_dfail;
        GLenum ssuccess_d_success;
        set_stencil_enum(sfail, _opengl_state.stencil_fail);
        set_stencil_enum(ssuccess_dfail, _opengl_state.stencil_success_depth_fail);
        set_stencil_enum(ssuccess_d_success, _opengl_state.stencil_success_depth_success);
        switch(_opengl_state.stencil_test)
        {
            case StencilTest::Always:
                glStencilFunc(GL_ALWAYS, _opengl_state.stencil_value,
                              0xFF);    // Means we always write in the stencil test
                break;

            case StencilTest::NotEqual:
                glStencilFunc(GL_NOTEQUAL, _opengl_state.stencil_value, 0xFF);
                break;

            case StencilTest::Equal:
                glStencilFunc(GL_EQUAL, _opengl_state.stencil_value, 0xFF);
                break;
        }

        glStencilOp(sfail, ssuccess_dfail, ssuccess_d_success);
        glStencilMask(0xFF);
    }
    else
        glDisable(GL_STENCIL_TEST);
}

void Renderer::begin_material(const Material& material)
{
    if(_opengl_state.write_color != material.write_color())
    {
        if(material.write_color())
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        else
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        _opengl_state.write_color = material.write_color();
    }

    if(_opengl_state.enable_depth_test != material.enable_depth_test())
    {
        if(material.enable_depth_test())
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);
        _opengl_state.enable_depth_test = material.enable_depth_test();
    }

    if(_opengl_state.depth_mask != material.depth_mask())
    {
        if(material.depth_mask())
            glDepthMask(GL_TRUE);
        else
            glDepthMask(GL_FALSE);

        _opengl_state.depth_mask = material.depth_mask();
    }
    glStencilMask(~0);

    if(material.enable_stencil_test())
    {
        glEnable(GL_STENCIL_TEST);
        glStencilMask(0xFF);

        GLenum sfail;
        GLenum ssuccess_dfail;
        GLenum ssuccess_d_success;

        switch(material.stencil_test)
        {
            case StencilTest::Always:
                glStencilFunc(GL_ALWAYS, material.stencil_value,
                              0xFF);    // Means we always write in the stencil test
                break;

            case StencilTest::NotEqual:
                glStencilFunc(GL_NOTEQUAL, material.stencil_value, 0xFF);
                break;

            case StencilTest::Equal:
                glStencilFunc(GL_EQUAL, material.stencil_value, 0xFF);
                break;
        }

        set_stencil_enum(ssuccess_dfail, material.stencil_success_depth_fail());
        set_stencil_enum(ssuccess_d_success, material.stencil_success_depth_success());
        set_stencil_enum(sfail, material.stencil_fail());

        glStencilOp(sfail, ssuccess_dfail, ssuccess_d_success);
        glStencilMask(0xFF);
    }
    else
        glDisable(GL_STENCIL_TEST);

    _opengl_state.enable_stencil_test = material.enable_stencil_test();

    /*if (!material.render_color)
                    glColorMask(0, 0, 0, 0);
            else
                    glColorMask(1, 1, 1, 1);*/

    switch(material.depth_test)
    {
        case DepthTest::Always:
            glDepthFunc(GL_ALWAYS);
            break;
        case DepthTest::Never:
            glDepthFunc(GL_NEVER);
            break;
        case DepthTest::Less:
            glDepthFunc(GL_LESS);
            break;
        case DepthTest::Equal:
            glDepthFunc(GL_EQUAL);
            break;
        case DepthTest::LEqual:
            glDepthFunc(GL_LEQUAL);
            break;
        case DepthTest::Greater:
            glDepthFunc(GL_GREATER);
            break;
        case DepthTest::NotEqual:
            glDepthFunc(GL_NOTEQUAL);
            break;
        case DepthTest::GEqual:
            glDepthFunc(GL_GEQUAL);
            break;
    }
    _opengl_state.depth_test = material.depth_test;

    if(_opengl_state.polygon_mode != material.polygon_mode() ||
       _opengl_state.face != material._face)
    {
        GLenum facemode = GL_FRONT_AND_BACK;

        switch(material.face())
        {
            case FaceMode::FrontAndBack:
                facemode = GL_FRONT_AND_BACK;
                break;
            case FaceMode::Back:
                facemode = GL_BACK;
                break;
            case FaceMode::Front:
                facemode = GL_FRONT;
                break;
        }

        switch(material.polygon_mode())
        {
            case Material::PolygonMode::Fill:
                glPolygonMode(facemode, GL_FILL);
                break;
            case Material::PolygonMode::Line:
                glPolygonMode(facemode, GL_LINE);
                break;
            case Material::PolygonMode::Point:
                glPolygonMode(facemode, GL_POINT);
                break;
        }

        _opengl_state.face         = material.face();
        _opengl_state.polygon_mode = material.polygon_mode();
    }

    if(_opengl_state.enable_blend != material.enable_blend())
    {
        if(material.enable_blend())
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
            glDisable(GL_BLEND);

        _opengl_state.enable_blend = material.enable_blend();
    }

    if(_opengl_state.program_id != material.shader_program->id())
    {
        glUseProgram(material.shader_program->id());
        _opengl_state.program_id = material.shader_program->id();
    }

    // Handle textures

    if(!material._texture_uniforms.empty())
        glEnable(GL_TEXTURE_2D);

    int texture_index = 0;

    for(const auto& sampler : material.samplers)
    {
        glActiveTexture(GL_TEXTURE0 + sampler.binding);    // Texture unit 0

        glBindTexture(GL_TEXTURE_2D, sampler.texture_name);

        switch(sampler.wrap_s)
        {
            case Sampler::Wrap::ClampToBorder:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                break;

            case Sampler::Wrap::ClampToEdge:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                break;

            case Sampler::Wrap::MirroredRepeat:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
                break;

            case Sampler::Wrap::MirrorClampToEdge:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                                GL_MIRROR_CLAMP_TO_EDGE);
                break;

            case Sampler::Wrap::Repeat:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                break;
        }

        switch(sampler.wrap_t)
        {
            case Sampler::Wrap::ClampToBorder:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                break;

            case Sampler::Wrap::ClampToEdge:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                break;

            case Sampler::Wrap::MirroredRepeat:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
                break;

            case Sampler::Wrap::MirrorClampToEdge:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                                GL_MIRROR_CLAMP_TO_EDGE);
                break;

            case Sampler::Wrap::Repeat:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                break;
        }

        switch(sampler.min_filter)
        {
            case Sampler::MinFilter::Nearest:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                break;
            case Sampler::MinFilter::Linear:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                break;
            case Sampler::MinFilter::NearestMipmapNearest:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                GL_NEAREST_MIPMAP_NEAREST);
                break;
            case Sampler::MinFilter::NearestMipmapLinear:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                GL_NEAREST_MIPMAP_LINEAR);
                break;
            case Sampler::MinFilter::LinearMipmapLinear:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                GL_LINEAR_MIPMAP_LINEAR);
                break;
            case Sampler::MinFilter::LinearMipmapNearest:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                                GL_LINEAR_MIPMAP_NEAREST);
                break;
        }

        switch(sampler.mag_filter)
        {
            case Sampler::MagFilter::Nearest:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                break;
            case Sampler::MagFilter::Linear:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                break;
        }
    }

    model_matrix_id = material.location_model_view_projection_matrix;

    // Handle lighting

    //if (material.is_lit)
    //{
    //	auto light = Game::scene()._point_lights.front();

    //	set_uniform("light.ambient",	light->ambient);
    //	set_uniform("light.diffuse",	light->diffuse);
    //	set_uniform("light.specular",	light->specular);
    //	set_uniform("light.constant",	light->constant);
    //	set_uniform("light.linear",		light->linear);
    //	set_uniform("light.quadratic",	light->quadratic);
    //	set_uniform("light.position",	light->entity().transform().world_position());

    //	// This is also kinda light related but I'm not really sure how to actually handle it
    //	// I probably need the ability to create material and manually add/remove uniforms stuff
    //	/*set_uniform("material.ambient_color",	material.u);
    //	set_uniform("material.diffuse_color",	material.diffuse_color);
    //	set_uniform("material.specular_color",	material.specular_color);
    //	set_uniform("material.shininess",		material.shininess);*/

    //	// The camera position is also needed as a part of the lighting shader thing
    //	if (_current_camera != nullptr)
    //		set_uniform("camera_position", _current_camera->entity().transform().world_position());
    //}

    // for(const auto& uniform : material._uniforms)
    // {
    //     switch(uniform.type)
    //     {
    //         case Material::UniformType::Int:
    //             glUniform1i(uniform.location, uniform.data.int_value);
    //             break;

    //         case Material::UniformType::Unsigned:
    //             glUniform1ui(uniform.location, uniform.data._unsigned_value);
    //             break;

    //         case Material::UniformType::Float:
    //             glUniform1f(uniform.location, uniform.data.value.x);
    //             break;

    //         case Material::UniformType::Vec2:
    //             glUniform2fv(uniform.location, 1, &uniform.data.value.x);
    //             break;

    //         case Material::UniformType::Vec3:
    //             glUniform3fv(uniform.location, 1, &uniform.data.value.x);
    //             break;

    //         case Material::UniformType::Vec4:
    //             glUniform4fv(uniform.location, 1, &uniform.data.value.x);
    //             break;
    //         case Material::UniformType::Matrix:
    //             glUniformMatrix4fv(uniform.location, 1, GL_FALSE,
    //                                uniform.data.matrix.data());
    //             break;
    //     }
    // }

    if(material.ubo)
        material.ubo->use();
}

void Renderer::set_uniform(unsigned id, const std::string& name, float v)
{
    glUniform1f(glGetUniformLocation(id, name.c_str()), v);
}

void Renderer::set_uniform(unsigned id, const std::string& name, int v)
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), v);
}

void Renderer::set_uniform(unsigned id, const std::string& name, const Vec3& v)
{
    glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &v.x);
}

void Renderer::set_uniform(unsigned id, const std::string& name, const Vec4& v)
{
    glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &v.x);
}

void Renderer::set_uniform(unsigned id, const std::string& name, const Matrix& m)
{
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, m.data());
}

ProfilerInfo Renderer::profiler() const noexcept
{
    return profiler_;
}

void Renderer::draw(const std::vector<const RendererComponent*>& renderers,
                    const Material&                              material,
                    Ref<Transform>                               transform)
{
    begin_material(material);

    for(const auto& renderer : renderers)
        draw_mesh(renderer->_mesh.get(),
                  _view_projection_matrix * transform->world_matrix());
}

void Renderer::draw(const RendererComponent& component,
                    const Material&          material,
                    Transform&               transform)
{
    begin_material(material);
    draw_mesh(component._mesh.get(), _view_projection_matrix * transform.world_matrix());
}

void Renderer::draw(const Mesh& mesh, const Material& material)
{
    begin_material(material);

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // This is done in the main loop since each model will have a different MVP matrix (At least for the M part)

    // So somehow I need to pass that here?
    //glUniformMatrix4fv(model_matrix_id, 1, GL_FALSE, matrix.data());

    glBindVertexArray(mesh.vao_id_);

    switch(mesh.primitive_type())
    {
        case PrimitiveType::Triangles:
            //profiler_.draw_calls++;
            //profiler_.triangle_count += mesh->indexes().size()/3;
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indexes_.size()),
                           GL_UNSIGNED_INT, (void*)0);
            break;

        case PrimitiveType::Quads:
            //profiler_.draw_calls++;
            //profiler_.triangle_count += mesh->indexes().size()/4 *2;
            glDrawElements(GL_QUADS, static_cast<GLsizei>(mesh.indexes_.size()),
                           GL_UNSIGNED_INT, (void*)0);
            break;

        case PrimitiveType::Lines:
            glDrawElements(GL_LINES, static_cast<GLsizei>(mesh.indexes().size()),
                           GL_UNSIGNED_INT, (void*)0);
            break;
    }
}

void Renderer::draw_mesh(const Mesh* mesh, const Matrix& matrix)
{
    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
    glUniformMatrix4fv(model_matrix_id, 1, GL_FALSE, matrix.data());
    glBindVertexArray(mesh->vao_id_);

    // We also add information to the "profiler"

    /*profiler_.vertices_size+= mesh->vertex_size()*mesh->vertex_count();
    profiler_.vertices_count+=mesh->vertex_count();*/

    switch(mesh->primitive_type())
    {
        case PrimitiveType::Triangles:
            //profiler_.draw_calls++;
            //profiler_.triangle_count += mesh->indexes().size()/3;
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->indexes_.size()),
                           GL_UNSIGNED_INT, (void*)0);
            break;

        case PrimitiveType::Quads:
            //profiler_.draw_calls++;
            //profiler_.triangle_count += mesh->indexes().size()/4 *2;
            glDrawElements(GL_QUADS, static_cast<GLsizei>(mesh->indexes_.size()),
                           GL_UNSIGNED_INT, (void*)0);
            break;

        case PrimitiveType::Lines:
            glDrawElements(GL_LINES, static_cast<GLsizei>(mesh->indexes().size()),
                           GL_UNSIGNED_INT, (void*)0);
            break;
    }
    //glBindVertexArray(0);
}

void Renderer::draw_line(const DrawList::Line& line)
{
    //for (auto& line : drawlist.lines_)
    //{
    //	auto mesh = Mesh::new_standard_mesh();

    //	// TODO : Build a Cylinder here instead of doing this crap that only
    //	// works on the x-y plan
    //	Vec3 a(line.x, line.y, line.z);
    //	Vec3 b(line.x2, line.y2, line.z2);

    //	Vec2 c(b.x - a.x, b.y - a.y);

    //	float line_width = 0.5f;
    //	c = c.orthogonal_vector();
    //	c.normalize();
    //	c *= line_width;

    //	mesh->vertices().resize(4 * 8, 0.0f);

    //	auto& vertices = mesh->vertices();

    //	vertices[0]		= line.x - c.x;
    //	vertices[1]		= line.y - c.y;
    //	vertices[2]		= line.z;

    //	vertices[8]		= line.x2 - c.x;
    //	vertices[9]		= line.y2 - c.y;
    //	vertices[10]	= line.z2;

    //	vertices[16]	= line.x2 + c.x;
    //	vertices[17]	= line.y2 + c.y;
    //	vertices[18]	= line.z2;

    //	vertices[24]	= line.x + c.x;
    //	vertices[25]	= line.y + c.y;
    //	vertices[26]	= line.z;

    //	mesh->indexes().insert(mesh->indexes().begin(), { 0,1,2,0,2,3 });
    //	mesh->update_vertices();

    //	static Material material = *ResourcesCache::material("unlit_color.mat");
    //	material.set_uniform("main_color", Vec4(line.r, line.g, line.b, line.a));
    //	material.enable_depth_test	= false;
    //	material.enable_blend		= false;

    //	Matrix identity;

    //	begin_material(material);
    //	draw(*mesh, identity);
    //	end_material(material);
    //}
}

void Renderer::draw_dlsprite(const DrawList::DrawListSprite& sprite)
{
    auto mesh = Mesh::new_standard_mesh();

    mesh->vertices().resize(8 * 4, 0.0f);

    mesh->addTriangle(0, 1, 2);
    mesh->addTriangle(0, 2, 3);

    mesh->update_vertices();

    auto offset_x = 0.0f;
    auto offset_y = 0.0f;

    // Also maybe this doesn't need to be computed all the time? Maybe it could
    // be cached on the Sprite class
    const auto offset_texture_x =
        sprite.sprite.offset_x / static_cast<float>(sprite.sprite.texture->width());
    const auto offset_texture_y =
        sprite.sprite.offset_y / static_cast<float>(sprite.sprite.texture->height());
    const auto texture_width_coef =
        sprite.sprite.width / static_cast<float>(sprite.sprite.texture->width());
    const auto texture_height_coef =
        sprite.sprite.height / static_cast<float>(sprite.sprite.texture->height());

    auto& vertices = mesh->vertices();

    vertices[0] = sprite.x + offset_x - (sprite.width * sprite.sprite.pivot_value.x);
    vertices[1] = sprite.y + offset_y - (sprite.height * sprite.sprite.pivot_value.y);
    vertices[2] = 0.0f;
    vertices[3] = offset_texture_x;
    vertices[4] = offset_texture_y;
    vertices[5] = 0.0f;
    vertices[6] = 0.0f;
    vertices[7] = 1.0f;

    vertices[8] =
        sprite.x + offset_x + (sprite.width * (1.0f - sprite.sprite.pivot_value.x));
    vertices[9]  = sprite.y + offset_y - (sprite.height * sprite.sprite.pivot_value.y);
    vertices[10] = 0.0f;
    vertices[11] = offset_texture_x + texture_width_coef;
    vertices[12] = offset_texture_y;
    vertices[13] = 0.0f;
    vertices[14] = 0.0f;
    vertices[15] = 1.0f;

    vertices[16] =
        sprite.x + offset_x + (sprite.width * (1.0f - sprite.sprite.pivot_value.x));
    vertices[17] =
        sprite.y + offset_y + (sprite.height * (1.0f - sprite.sprite.pivot_value.y));
    vertices[18] = 0.0f;
    vertices[19] = offset_texture_x + texture_width_coef;
    vertices[20] = offset_texture_y + texture_height_coef;
    vertices[21] = 0.0f;
    vertices[22] = 0.0f;
    vertices[23] = 1.0f;

    vertices[24] = sprite.x + offset_x - (sprite.width * sprite.sprite.pivot_value.x);
    vertices[25] =
        sprite.y + offset_y + (sprite.height * (1.0f - sprite.sprite.pivot_value.y));
    vertices[26] = 0.0f;
    vertices[27] = offset_texture_x;
    vertices[28] = offset_texture_y + texture_height_coef;
    vertices[29] = 0.0f;
    vertices[30] = 0.0f;
    vertices[31] = 1.0f;

    mesh->update_vertices();

    begin_material(sprite.material);
    draw_mesh(mesh.get(), _view_projection_matrix * Matrix());
}

static void create_quad(Mesh& mesh,
                        float x,
                        float y,
                        float width,
                        float height,
                        float uv_x,
                        float uv_y,
                        float uv_width,
                        float uv_height)
{
    auto offset = mesh.vertex_count();
    mesh.add_vertex().position(x, y, 0.0f).uv(uv_x, uv_y);
    mesh.add_vertex().position(x + width, y, 0.0f).uv(uv_x + uv_width, uv_y);
    mesh.add_vertex()
        .position(x + width, y + height, 0.0f)
        .uv(uv_x + uv_width, uv_y + uv_height);
    mesh.add_vertex().position(x, y + height, 0.0f).uv(uv_x, uv_y + uv_height);

    mesh.addTriangle(offset, offset + 1, offset + 2);
    mesh.addTriangle(offset, offset + 2, offset + 3);
}

static void create_quad_lol(std::vector<float>& vertices,
                            int                 offset,
                            float               offsetx,
                            float               offsety,
                            float               width,
                            float               height,
                            float               uvx,
                            float               uvy,
                            float               uvw,
                            float               uvh)
{
    vertices[offset * 20 + 0] = offsetx;
    vertices[offset * 20 + 1] = offsety;
    vertices[offset * 20 + 2] = 0.0f;
    vertices[offset * 20 + 3] = uvx;
    vertices[offset * 20 + 4] = uvy;

    vertices[offset * 20 + 5] = offsetx + width;
    vertices[offset * 20 + 6] = offsety;
    vertices[offset * 20 + 7] = 0.0f;
    vertices[offset * 20 + 8] = uvx + uvw;
    vertices[offset * 20 + 9] = uvy;

    vertices[offset * 20 + 10] = offsetx + width;
    vertices[offset * 20 + 11] = offsety + height;
    vertices[offset * 20 + 12] = 0.0f;
    vertices[offset * 20 + 13] = uvx + uvw;
    vertices[offset * 20 + 14] = uvy + uvh;

    vertices[offset * 20 + 15] = offsetx;
    vertices[offset * 20 + 16] = offsety + height;
    vertices[offset * 20 + 17] = 0.0f;
    vertices[offset * 20 + 18] = uvx;
    vertices[offset * 20 + 19] = uvy + uvh;
}

void Renderer::draw_rect(const DrawList::Rectangle& rectangle)
{
    std::vector<float>    vertices(4 * 4 * 5);
    std::vector<unsigned> indexes {0, 1, 2, 0, 2, 3};

    create_quad_lol(vertices, 0, rectangle.x, rectangle.y, rectangle.width,
                    rectangle.height, 0.0f, 0.0f, 1.0f, 1.0f);

    auto mesh = Mesh::new_standard_2D_mesh(std::move(vertices), std::move(indexes));

    begin_material(rectangle.material);
    draw_mesh(mesh.get(), _view_projection_matrix * Matrix());
}

void Renderer::draw_dl(const DrawList& drawlist)
{
    for(const auto& [class_type, index] : drawlist.order_)
    {
        switch(class_type)
        {
            case DrawList::DrawListType::Rectangle:
                draw_rect(drawlist.rectangles_[index]);
                break;
            case DrawList::DrawListType::NineSclice:
                draw_ns(drawlist.nine_slices_[index]);
                break;
            case DrawList::DrawListType::Text:
                draw_txt(drawlist.texts_[index]);
                break;
            case DrawList::DrawListType::Line:
                draw_line(drawlist.lines_[index]);
                break;
            case DrawList::DrawListType::Sprite:
                draw_dlsprite(drawlist.sprites_[index]);
                break;
            case DrawList::DrawListType::Mesh:

                if(drawlist._meshes[index].window->id() == current_window_->id())
                {
                    begin_material(drawlist._meshes[index].material);
                    draw_mesh(&drawlist._meshes[index].mesh,
                              _view_projection_matrix * drawlist._meshes[index].matrix);
                }

                break;
            case DrawList::DrawListType::ResetStencilBuffer:
                glClearStencil(0);
                glStencilMask(0xFF);
                glClear(GL_STENCIL_BUFFER_BIT);
                break;
        }
    }
}

void Renderer::draw_txt(const DrawList::Text& text)
{
    // if(text.font.font_ == nullptr)
    //     log_fatal_error("Font for DrawList::Text is null");

    // auto line_height =
    //     text.font.font_->configurations_[text.font.current_configuration_]->height;
    // auto ascent =
    //     text.font.font_->configurations_[text.font.current_configuration_]->ascent;

    // float off = text.position.x;

    // std::vector<float>    vertices(5 * 4 * (text.text.size()), 0.0f);
    // std::vector<unsigned> indexes(text.text.size() * 6);

    // for(int i = 0; i < text.text.size(); i++)
    // {
    //     indexes[i * 6 + 0] = (i * 4) + 0;
    //     indexes[i * 6 + 1] = (i * 4) + 1;
    //     indexes[i * 6 + 2] = (i * 4) + 2;

    //     indexes[i * 6 + 3] = (i * 4) + 0;
    //     indexes[i * 6 + 4] = (i * 4) + 2;
    //     indexes[i * 6 + 5] = (i * 4) + 3;
    // }

    // Vec3 offset(text.position.x, text.position.y + text.dimensions.y);

    // float scaling = text.scaling;

    // float text_max_height = 0.0f;
    // float text_min_height = 0.0f;

    // // Selected the closest configuration according to the ViewFont parameter

    // auto& conf = *text.font.font_->configurations_[text.font.current_configuration_];

    // int i          = 0;
    // int line_count = 1;

    // for(auto c : text.text)
    // {
    //     if(c == '\n')
    //     {
    //         line_count++;
    //         offset.y -= line_height;
    //         off = 0;
    //         continue;
    //     }

    //     const auto  index           = static_cast<unsigned int>(c);
    //     const auto& character_infos = conf.characters[index];

    //     const float texture_width  = static_cast<float>(conf.texture->width());
    //     const float texture_height = static_cast<float>(conf.texture->height());

    //     // TODO : Inspect that part later on
    //     if(text_max_height < character_infos.bt)
    //         text_max_height = character_infos.bt;

    //     float vvv = character_infos.bh - character_infos.bt;

    //     if(text_min_height < vvv)
    //         text_min_height = vvv;

    //     float       v  = (texture_height - character_infos.bh) / texture_height;
    //     float       aa = character_infos.bw / texture_width;
    //     const float glyph_x_offset = character_infos.bl * scaling;
    //     const float glpyh_y_offset =
    //         -(character_infos.bh * scaling - character_infos.bt * scaling);
    //     const float glyph_width = character_infos.bw * scaling;
    //     const float glyph_height =
    //         character_infos.bh * scaling;    // abusing a bit here

    //     int ii = i * 20;

    //     vertices[ii + 0] = offset.x + off + glyph_x_offset;
    //     vertices[ii + 1] = offset.y + glpyh_y_offset;
    //     vertices[ii + 2] = offset.z;
    //     vertices[ii + 3] = character_infos.tx;
    //     vertices[ii + 4] = v;

    //     vertices[ii + 5] = offset.x + off + glyph_x_offset + glyph_width;
    //     vertices[ii + 6] = offset.y + glpyh_y_offset;
    //     vertices[ii + 7] = offset.z;
    //     vertices[ii + 8] = character_infos.tx + aa;
    //     vertices[ii + 9] = v;

    //     vertices[ii + 10] = offset.x + off + glyph_x_offset + glyph_width;
    //     vertices[ii + 11] = offset.y + glpyh_y_offset + glyph_height;
    //     vertices[ii + 12] = offset.z;
    //     vertices[ii + 13] = character_infos.tx + aa;
    //     vertices[ii + 14] = 1.0f;

    //     vertices[ii + 15] = offset.x + off + glyph_x_offset;
    //     vertices[ii + 16] = offset.y + glpyh_y_offset + glyph_height;
    //     vertices[ii + 17] = offset.z;
    //     vertices[ii + 18] = character_infos.tx;
    //     vertices[ii + 19] = 1.0f;

    //     off += conf.characters[static_cast<int>(c)].ax * scaling;
    //     i++;
    // }

    // float total_height = text_max_height + text_min_height;

    // switch(text.horizontal_alignment)
    // {
    //     case DrawList::Text::HorizontalAlignment::Centered:
    //         for(size_t i = 0; i < vertices.size() / 5; ++i)
    //             vertices[i * 5] += std::round(text.dimensions.x / 2.0f - off / 2.0f);
    //         break;

    //     case DrawList::Text::HorizontalAlignment::Left:
    //         break;

    //     case DrawList::Text::HorizontalAlignment::Right:
    //         for(size_t i = 0; i < vertices.size() / 5; ++i)
    //             vertices[i * 5] += text.dimensions.x - off;
    //         break;
    // }

    // switch(text.vertical_alignment)
    // {
    //     case DrawList::Text::VerticalAlignment::Centered:
    //         for(size_t i = 0; i < vertices.size() / 5; ++i)
    //             vertices[(i * 5) + 1] -= std::round(
    //                 text_max_height * scaling +
    //                 (text.dimensions.y / 2.0f - (total_height / 2.0f * scaling)));
    //         break;

    //     case DrawList::Text::VerticalAlignment::Top:
    //         for(size_t i = 0; i < vertices.size() / 5; ++i)
    //             vertices[(i * 5) + 1] -= ascent * scaling;
    //         break;

    //     case DrawList::Text::VerticalAlignment::Down:
    //         for(size_t i = 0; i < vertices.size() / 5; ++i)
    //             vertices[(i * 5) + 1] -=
    //                 (text_max_height * scaling) +
    //                 ((text.dimensions.y) - (total_height * scaling));
    //         break;
    // }

    // auto mesh = Mesh::new_standard_2D_mesh(std::move(vertices), std::move(indexes));

    // begin_material(text.material);
    // draw_mesh(mesh.get(), _view_projection_matrix * Matrix());
}

void Renderer::draw_ns(const DrawList::NineSlice& n)
{
    std::vector<float>    vertices(9 * 4 * 5);
    std::vector<unsigned> indexes {
        0,  1,  2,  0,  2,  3,  4,  5,  6,  4,  6,  7,  8,  9,  10, 8,  10, 11,

        12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23,

        24, 25, 26, 24, 26, 27, 28, 29, 30, 28, 30, 31, 32, 33, 34, 32, 34, 35};

    auto a = n.top_border_uv_ * 1;
    auto b = n.bottom_border_uv_ * -1;

    // bottom

    create_quad_lol(vertices, 0, n.x, n.y, n.left_border_, n.bottom_border_, 0.0f, 0.0f,
                    n.left_border_uv_, a);
    create_quad_lol(vertices, 1, n.x + n.left_border_, n.y,
                    n.width - n.left_border_ - n.right_border_, n.bottom_border_,
                    n.left_border_uv_, 0.0f,
                    1.0f - n.left_border_uv_ - n.right_border_uv_, a);
    create_quad_lol(vertices, 2, n.x + n.width - n.right_border_, n.y, n.right_border_,
                    n.bottom_border_, 1.0f - n.right_border_uv_, 0.0f, n.right_border_uv_,
                    a);

    // center

    create_quad_lol(vertices, 3, n.x, n.y + n.bottom_border_, n.left_border_,
                    n.height - n.bottom_border_ - n.top_border_, 0.0f, 0.0f + a,
                    n.left_border_uv_, 1.0f - a - b);

    create_quad_lol(vertices, 4, n.x + n.left_border_, n.y + n.bottom_border_,
                    n.width - n.left_border_ - n.right_border_,
                    n.height - n.bottom_border_ - n.top_border_, n.left_border_uv_,
                    0.0f + a, 1.0f - n.left_border_uv_ - n.right_border_uv_,
                    1.0f - a - b);

    create_quad_lol(vertices, 5, n.x + n.width - n.right_border_, n.y + n.bottom_border_,
                    n.right_border_, n.height - n.bottom_border_ - n.top_border_,
                    1.0f - n.right_border_uv_, 0.0f + a, n.right_border_uv_,
                    1.0f - a - b);

    // top

    create_quad_lol(vertices, 6, n.x, n.y + n.height - n.top_border_, n.left_border_,
                    n.top_border_, 0.0f, 1.0f - b, n.left_border_uv_, b);

    create_quad_lol(vertices, 7, n.x + n.left_border_, n.y + n.height - n.top_border_,
                    n.width - n.left_border_ - n.right_border_, n.top_border_,
                    n.left_border_uv_, 1.0f - b,
                    1.0f - n.left_border_uv_ - n.right_border_uv_, b);

    create_quad_lol(vertices, 8, n.x + n.width - n.right_border_,
                    n.y + n.height - n.top_border_, n.right_border_, n.top_border_,
                    1.0f - n.right_border_uv_, 1.0f - b, n.right_border_uv_, b);

    auto mesh = Mesh::new_standard_2D_mesh(std::move(vertices), std::move(indexes));

    begin_material(n.material);
    draw_mesh(mesh.get(), _view_projection_matrix * Matrix());
}

DrawList& Renderer::world_draw_list()
{
    return _world_draw_list;
}

void Renderer::setClearColor(Color c)
{
    glClearColor(c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha());
}

void Renderer::set_viewport(const Viewport& v)
{
    set_viewport(static_cast<GLint>(v.x), static_cast<GLint>(v.y),
                 static_cast<GLint>(v.width), static_cast<GLint>(v.height));
}

void Renderer::set_viewport(int x, int y, int width, int height)
{
    glViewport(x, y, width, height);
}

/* void Renderer::WindowDrawList::reserve(int size)
    {
            rectangles.resize(size);
    }*/

/* void Renderer::WindowDrawList::AddRectangle(float x, float y, float width, float height, Color color)
    {
            MeshRenderer::Vertex * vs = new MeshRenderer::Vertex[4];

            vs[0] = { x,			y,			0.0f, color.r, color.g, color.b,color.a, 0.0f, 0.0f };
            vs[1] = { x + width,	y,			0.0f, color.r, color.g, color.b,color.a, 0.0f, 0.0f };
            vs[2] = { x + width,	y + height,	0.0f, color.r, color.g, color.b,color.a, 0.0f, 0.0f };
            vs[3] = { x,			y + height,	0.0f, color.r, color.g, color.b,color.a, 0.0f, 0.0f };
                    
            MeshRenderer m(vs, 4, MeshRenderer::PrimitiveType::Quads);
            rectangles.add(m);
    }*/

void tex_sub_image(int           mipmap_level,
                   int           x,
                   int           y,
                   int           width,
                   int           height,
                   Image::Format format,
                   void*         pixels)
{
    if(format == Image::Format::RGBA_8888)
        glTexSubImage2D(GL_TEXTURE_2D, mipmap_level, x, y, width, height, GL_RGBA,
                        GL_UNSIGNED_BYTE, pixels);
}
}    // namespace corgi

//void Renderer::drawWindowDrawList()
//{
//    glViewport(0, 0, window->width(), window->height());
//    glMatrixMode(GL_PROJECTION);
//
//    auto mat = math::Matrixf::ortho
//    (
//        0.0f, static_cast<float>( window->width()),
//        static_cast<float>(window->height()),0.0f ,
//        -0.10f, 10.0f
//    );
//
//    glLoadMatrixf((float*) &mat);

//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();

//   /* for (auto& m : _windowDrawList.rectangles)
//    {
//        draw(&m);
//    }*/
//    //_windowDrawList.rectangles.clear();
//}

/* void Renderer::draw(World & world, FrameBuffer & frameb)
	{
		beginFrameBuffer(&frameb);
		Clear();
		Draw(world);
		endFrameBuffer();
	}*/

//// TODO :	For now I create material and shaders like that, maybe later on I could
////			serialize/deserialize materials and shaders inside a repo maybe?
//void Renderer::initialize_default_lit_material()
//{
//	Material material;

//	// For now I kinda duplicate it, but I think I prefer to be able to see
//	// everything in the same function
//	char default_vertex_shader[] = R"(
//		#version 330 core
//
//		layout(location = 0) in vec3 position;
//		layout(location = 1) in vec2 texture_coordinates;
//		layout(location = 2) in vec3 normals;

//		out vec2 uv;
//		out vec3 normal;
//		out vec3 fragment_position;

//		uniform mat4 model_matrix;
//		uniform mat4 view_matrix;
//		uniform mat4 projection_matrix;

//		void main()
//		{
//			// We get the normal matrix here
//			normal = mat3(transpose(inverse(model_matrix))) * normals;

//			gl_Position = projection_matrix* view_matrix*model_matrix*  vec4(position,1.0);
//			fragment_position = (model_matrix * vec4(position, 1.0f)).xyz;
//			uv = texture_coordinates;
//		}
//	)";

//	char default_fragment_shader[] = R"(
//		#version 330 core

//		vec3 ambient_color;
//		vec3 diffuse_color;
//		vec3 specular_color;
//		float shininess;

//		struct Light
//		{
//			vec3 position;
//			vec3 ambient;
//			vec3 diffuse;
//			vec3 specular;

//			float constant;
//			float linear;
//			float quadratic;
//		};

//		in vec2 uv;
//		in vec3 normal;
//		in vec3 fragment_position;

//		uniform Light		light;

//		uniform vec3		camera_position;

//		uniform sampler2D	main_texture;
//		uniform int			use_texture;
//		uniform vec4		main_color;

//		out vec4 color;

//		void main()
//		{
//			vec3 l_normal			= normalize(normal);
//			vec3 camera_direction	= normalize(camera_position - fragment_position);
//			vec3 light_direction	= normalize(light.position - fragment_position);
//			vec3 reflect_direction	= reflect(-light_direction, l_normal);

//			float distance    = length(light.position - fragment_position);
//			float attenuation = 1.0 / (light.constant + light.linear * distance +
//   		    light.quadratic * (distance * distance));

//			// Ambient

//			vec3 ambient = (light.ambient*attenuation) * ambient_color;

//			// Diffuse
//
//			float diff		= max(dot(normal, light_direction), 0.0);
//			vec3 diffuse	=  (light.diffuse*attenuation) * ( diffuse_color * diff);
//			// Specular

//			// 32 here is the shiniess of the object
//			float spec = pow(max(dot(camera_direction, reflect_direction), 0.0), shininess);
//			vec3 specular = (light.specular*attenuation) * (spec * specular_color);
//			if(use_texture == 1)
//			{
//				// We discard fragments that are totally transparent
//				if(texture( main_texture, uv ).a==0.0)
//					discard;
//				color		= texture( main_texture, uv ).rgba;
//				color.rgb		+= (ambient + diffuse + specular);
//			}
//			else
//			{
//				color	= vec4((ambient + diffuse + specular),1.0f);
//			}
//		}
//	)";

//	material.is_lit = true;

//	material.set_uniform("ambient_color", Vec3());
//	material.set_uniform("diffuse_color", Vec3());
//	material.set_uniform("specular_color", Vec3());
//	material.set_uniform("shininess", 0.0f);

//	generate_shader("default_vertex_shader", Shader::Type::Vertex, default_vertex_shader);
//	generate_shader("default_fragment_shader", Shader::Type::Fragment, default_fragment_shader);
//	generate_shader_program("default_lit_shader_program", "default_vertex_shader", "default_fragment_shader");
//	material.shader_program = get_shader_program("default_lit_shader_program").lock().get();

//	register_material("default_lit_material", material);
//}
