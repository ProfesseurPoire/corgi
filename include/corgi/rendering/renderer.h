#pragma once


#include <corgi/ecs/ComponentPool.h>

#include <corgi/math/Matrix.h>
#include <corgi/math/Vec3.h>
#include <corgi/math/Vec4.h>

#include <corgi/rendering/DrawList.h>
#include <corgi/rendering/Profiler.h>
#include <corgi/rendering/WindowDrawList.h>

#include <corgi/utils/Color.h>
#include <corgi/utils/TimeHelper.h>

#include <corgi/components/Transform.h>

#include <memory>
#include <vector>

namespace corgi
{
    class Transform;
    class RendererComponent;
    class SpriteRenderer;
    class Camera;
    class Text;
    struct Panel;
    class Window;
    class Image;
    class FrameBuffer;
    class Texture;
    struct Viewport;
    class Tilemap;
    class ParticleEmitter;
    class BoxCollider;
    class ColliderComponent;
    class MeshRenderer;
    class Mesh;
    class ResourcesCache;
    struct Canvas;

    class Scene;

    // So the Renderer is basically the OpenGLContext here. It can creates Buffer objects and stuff,
    // but theses buffer objects only makes sense for the current OpenGL Context.
    // So the architecture will stay the way it is now. Window owns a "Renderer" Object
    // which owns the state of every OpenGL thing.
    class Renderer
    {
    public:
        // Trying to use the default values for most things
        // Also this could literally be a Material
        struct OpenGLState
        {
            bool write_color {true};    // 0 = false 1 = true -1 = undefined
            bool enable_depth_test {true};
            bool depth_mask {true};
            bool enable_blend {true};

            bool        stencil_mask = false;    // 1 byte
            bool        enable_stencil_test {false};
            StencilOp   stencil_fail                  = StencilOp::Keep;
            StencilOp   stencil_success_depth_fail    = StencilOp::Replace;
            StencilOp   stencil_success_depth_success = StencilOp::Replace;
            StencilOp   stencil_op   = StencilOp::Replace;     // 1 byte (can make it 1)
            StencilTest stencil_test = StencilTest::Always;    // 1 bytes
            FaceMode    face {FaceMode::FrontAndBack};

            int stencil_value {1};

            std::vector<unsigned> binded_textures;

            DepthTest             depth_test {DepthTest::Always};
            Material::PolygonMode polygon_mode {Material::PolygonMode::Fill};
            unsigned              program_id = 0;
        };

        OpenGLState _opengl_state;
        Counter     counter_;

        // Constructors

        // The renderer is tightly related to the window anyways since
        // the window actually creates and keep the opengl context
        // Anything
        Renderer();

        // Functions

        void initialize();
        void initialize_opengl_state();
        void clear();
        void draw_scene(Window& window);
        void draw_colliders(Scene& scene);

        [[deprecated("Use windowDrawList() instead")]] WindowDrawList& window_draw_list();

        [[nodiscard]] WindowDrawList& windowDrawList();

        DrawList& world_draw_list();

        //[[nodiscard]] PostProcessing* post_processing();

        // FrameBuffer

        //void resize_frame_buffer(FrameBuffer* framebuffer, int width, int height);

        static void begin_default_frame_buffer();
        static void begin_frame_buffer(const FrameBuffer* frameBuffer);
        static void end_frame_buffer();

        void set_uniform(unsigned id, const std::string& name, float v);
        void set_uniform(unsigned id, const std::string& name, int v);
        void set_uniform(unsigned id, const std::string& name, const Vec3& v);
        void set_uniform(unsigned id, const std::string& name, const Vec4& v);
        void set_uniform(unsigned id, const std::string& name, const Matrix& m);

        /*!
         * @brief Sets the window the renderer is drawing into
         */
        void set_current_window(Window* window);

        /*!
		 * @brief	Actually send the model matrix with the mesh's vertex to the GPU
		 */
        void draw_mesh(const Mesh* mesh, const Matrix& matrix);

        // Materials

        /*!
		 * @brief	Defines a lot of opengl stuff tied to the current material
		 *			Must be called before a a draw function
		 */
        void begin_material(const Material& material);

        // Variables

        [[nodiscard]] ProfilerInfo profiler() const noexcept;

        Window* window = nullptr;

        // TODO : probably implement that on the game side?
        bool show_colliders_ = true;

    private:
        // Functions

        void setClearColor(Color c);
        void set_viewport(const Viewport& v);
        void set_viewport(int x, int y, int width, int height);

        /*
		 * @brief	Draw a bunch of RendererComponent objects that share the same material
		 */
        void draw(const std::vector<const RendererComponent*>& renderers,
                  const Material&                              material,
                  Ref<Transform>                               transform);
        void draw(const RendererComponent& component,
                  const Material&          material,
                  Transform&               transform);

        // TODO :	I'm not sure I should keep that like that
        void draw_collider(ColliderComponent* collider, const Matrix& world_matrix);
        void draw_dl(const DrawList& drawlist);
        void draw_rect(const DrawList::Rectangle& rectangle);
        void draw_ns(const DrawList::NineSlice& nine_slice);
        void draw_txt(const DrawList::Text& text);
        void draw_line(const DrawList::Line& line);
        void draw_dlsprite(const DrawList::DrawListSprite& sprite);

        void initialize_camera(const Matrix& inverse, const Camera& camera);

        /*!
		 * @brief	Handle the drawing of everything on the screenSpace
		 *			usually that means everything UI related, or calls to 
		 *			the direct rendering mode to screen stuff
		 */
        void drawScreenSpace(Window& window);

        // Member Variables

        Matrix _view_matrix;
        Matrix _projection_matrix;
        Matrix _view_projection_matrix;

        // Use this when you need to draw temporary primitives.
        // Objects will be drawn relative to the screen coordinates/size
        WindowDrawList window_draw_list_;

        // Use this when you need to draw temporary stuff in the world
        DrawList _world_draw_list;

        int model_matrix_id = -1;

        //std::unique_ptr<PostProcessing> post_processing_;
        ProfilerInfo profiler_ = ProfilerInfo();

        ComponentPool<Transform>* transform_map_ = nullptr;

        Scene* _current_scene {nullptr};
        Window* current_window_ {nullptr};
    };
}    // namespace corgi
