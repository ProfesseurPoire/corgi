#pragma once

#include <corgi/containers/UniquePtr.h>
#include <corgi/math/Matrix.h>
#include <corgi/math/Vec2.h>
#include <corgi/utils/Color.h>
#include <corgi/utils/types.h>

#include <functional>

using namespace corgi;

namespace corgi
{
    class Transform;
    class FrameBuffer;

    class Layers
    {
    public:
        void enable_all();
        void disable_all();
        void enable_one_layer(int layer_id);
        void enable_layer(int layer_id);
        void disable_layer(int layer_id);

        [[nodiscard]] int_64 layers() const;

    private:
        int_64 layers_ = 1;    // 1 layer = 1 byte
    };

    /*!
     *	Just following the glviewport function here
     *  https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glViewport.xhtml
     *	using int though because GlSizei is actually implemented as an int
     */

    // TODO :	It would be nice to really match the opengl implementation type
    //			of glsizei or of another graphic library
    struct Viewport
    {
        int x {0}, y {0}, width {0}, height {0};
    };

    /*
     *  @brief  The camera component represents a camera viewpoint with its projection and viewport
     *			settings. Anything in "front" of the camera and inside its
     *			projection transformation will be rendered to a surface, primarily inside the window,
     *			but it can also be rendered inside a texture. The viewport is a rectangle that
     *			defines where the camera actually renders inside the surface.
     *
     *          You probably will only use 1, but some effect, like shadow mapping,
     *          can be achieved by using multiple cameras, that then render into a texture that
     *          can later on be used by a shader to do stuff.
     *
     *          You could also have multiple camera drawing into different viewport. A viewport
     *          being a rectangle on the screen/window.
     *
     *          Or maybe camera on different windows?
     *
     *          In OpenGL, there is no notion of "camera". There's only "one", and it's always
     *          centered in 0.0.0, looking to the Z negative axis.
     *
     *          So to actually "move" the camera, you need to move the whole world. A solution
     *          that is often used to solve this problem is to apply the inverse of the camera's
     *          transformation.
     *
     *          So if your camera object is 6 units to the x axis, you actually
     *          move all your objects from -6 units
     *
     *          The projection Matrix is here to create a perspective effect
     *          (frustum and perspective matrix), or to expand the size
     *          of the NDC cube (ortho matrix) Though you could technically
     *          throw any kind of matrix as the projection matrix
     *
     *          When rendering, OpenGL will take every vertex, and multiply them
     *          with the MVP matrix, for ModelViewProjection matrix, which is the multiplication
     *          of the Model matrix, the View matrix and the projection matrix to transform
     *          the vertices to NDC space. Inside that space, any vertex that is outside
     *          the -1/+1 cube is clipped by OpenGL and won't be rendered.
     *
     *          Also the camera holds the color used to clear the screen, and the viewport
     *          By default, the viewport matches the window's dimensions. If you manually
     *          set the viewport, this behavior is deactivated, so, use with caution/
     *          know what you're doing
     */
    class Camera
    {
        friend class Renderer;

    public:
        // Lifecycle

        Camera();
        ~Camera();

        Camera(const Camera& c);
        Camera(Camera&& c) noexcept;

        Camera& operator=(const Camera& c);
        Camera& operator=(Camera&& c) noexcept;

        // Functions

        /*!
         * @brief Function that gets called when the camera starts rendering
         */
        std::function<void()> on_start;

        /*!
         * @brief Function that gets called when the camera ends rendering
         */
        std::function<void()> on_end;

        /*!
         *  @brief  Sets an orthographic projection matrix.
         *
         *  @param  height  height of the projection
         *  @param  ratio   ratio = width/height
         *  @param  znear   near clip plane
         *  @param  zfar    far clip plane
         */
        void ortho(float height, float ratio, float znear, float zfar);

        /**
         * @brief Sets an orthographic projection matrix
         * 
         * @param left 
         * @param right 
         * @param bottom 
         * @param top 
         * @param near 
         * @param far 
         */
        void
        ortho(float left, float right, float bottom, float top, float near, float far);

        /*!
         *  @brief  Set the projection matrix as perspective
         */
        void frustum(float l, float r, float b, float y, float n, float f);

        /*!
         *  @brief  Sets the znear clip plane of the projection. 
         *  
         *			Anything drawn behind this value will be clipped out. This
         *          will also raise the dirty flag so the projection matrix
         *          is updated by the renderer system
         */
        void znear(float value);

        /*!
         *  @brief  Sets the zfar clip plane of the projection. 
         *  
         *			Anything drawn after this value will be clipped out. This
         *          will also raise the dirty flag so the projection matrix
         *          is updated by the renderer system
         */
        void zfar(float value);

        /*!
         *  @brief  Sets the ratio value. The ratio value is used to find
         *          the "width" of the orthographic projection by multiplying
         *          it to the @ref orthographicSize
         */
        void ratio(float value);

        /*
         *  @brief  Sets the orthographicHeight value. The orthographicSize
         *          is the height of the orthographic projection.This will
         *          also raise the _dirty flag to true so the component's
         *          projection matrix is updated by the renderer system
         */
        void orthographic_height(float value);

        /*
         *  @brief  Returns the znear clip plane of the projection. Works
         *          for both the perspective and orthographic projection
         */
        [[nodiscard]] float znear() const;

        /*
         *  @brief  Returns the zfar clip plane of the projection. Works
         *          for both the perspective and orthographic projection
         */
        [[nodiscard]] float zfar() const;

        /*
         *  @brief  Returns the ratio between the height and the width
         *          of the projection
         */
        [[nodiscard]] float ratio() const;

        /*
         *  @brief  Returns the height of the orthographic projection
         */
        [[nodiscard]] float orthographic_height() const;

        /*
         *  @brief  Sets a custom projection matrix
         *
         *  If you set the projection using this method, the component won't
         *  use _orthographicSize and ratio anymore
         */
        void projection_matrix(const Matrix& m);

        /*
         *  @brief Returns true if the camera is in orthographic mode
         */
        [[nodiscard]] bool is_orthographic() const;

        /*
         * @brief Returns true if the camera is in perspective mode
         */
        [[nodiscard]] bool is_perspective() const;

        /*
         *  @brief  Returns true if a custom projection matrix was fed
         *          to the camera
         */
        [[nodiscard]] bool is_custom() const;

        /*
         *  @brief  Sets the color used to clear the color attachment 
         *          of the framebuffer
         */
        void clearColor(Color color);

        /*
         *  @brief  Gets the color used to clear the framebuffer's color attachment
         *  @return Returns a Color reference to the mClearColor member variable 
         */
        [[nodiscard]] Color& clearColor() noexcept;

        /*
         *  @brief  Gets the color used to clear the framebuffer's color attachment
         *  @return Returns a const Color reference to the mClearColor member variable 
         */
        [[nodiscard]] const Color& clearColor() const noexcept;

        /*!
         *  @brief  Returns the viewport from the camera
         *          The viewport is the rectangle where the camera renders
         *          on the screen/framebuffer
         */
        [[nodiscard]] Viewport& viewport() noexcept;

        [[nodiscard]] const Viewport& viewport() const noexcept;

        /*
         *  @brief  Sets the camera's viewport. 
         *          The viewport is the rectangle where the camera renders on
         *          the screen/framebuffer
         */
        void viewport(int x, int y, int width, int height);

        /*
         *  @brief  Returns a reference to the projection matrix used by the
         *          the camera
         */
        [[nodiscard]] Matrix&       projection_matrix();
        [[nodiscard]] const Matrix& projection_matrix() const;

        void use_default_viewport();

        [[nodiscard]] Vec2 screen_to_world(const Transform& transform,
                                           int              screenWidth,
                                           int              screenHeight,
                                           int              x,
                                           int              y) const;

        [[nodiscard]] Vec2 screen_to_world_point(Transform& transform, float x, float y);
        // [[nodiscard]] Vec2 screen_to_world_point(int x, int y);

        [[nodiscard]] bool operator<(const Camera& camera) const;

        [[nodiscard]] int order() const;
        void              order(short v);

        [[nodiscard]] FrameBuffer*       framebuffer();
        [[nodiscard]] const FrameBuffer* framebuffer() const;
        void                             framebuffer(int width, int height);

        [[nodiscard]] Layers&       culling_layers();
        [[nodiscard]] const Layers& culling_layers() const;

        float l_;
        float r_;
        float t_;
        float b_;

    protected:
        enum class Type : char
        {
            Ortho,
            Perspective,
            Custom
        };

        // Lifecycle

        void move(Camera&& c) noexcept;
        void copy(const Camera& c);

        // Variables

        // When true the projection matrix need to be computed
        bool is_dirty_ = false;           // 1
        Type type_     = Type::Custom;    // 1

        // Every entity is attached to a layer
        // This will tell the camera to ignore entities based
        // on their layer
        Layers culling_layers_;    // on 8 bytes

        Matrix projection_matrix_;    // 64

        // Might be a bit big and often useless but it's also
        // not like I'm going to have thousands of camera
        float znear_               = 0.0f;    // 4
        float zfar_                = 0.0f;    // 4
        float ratio_               = 0.0f;    // 4
        float orthographic_height_ = 0.0f;    // 4

        // if null, the camera renders on the default framebuffer
        UniquePtr<FrameBuffer> framebuffer_;    // 8 bytes

        Color    mClearColor = Color {1.0f, 1.0f, 1.0f, 1.0f};    // 16
        Viewport viewport_   = {0, 0, 0, 0};                      // 16
        // Order in which the camera are sorted for rendering
        short order_ = 0;    // 2
    };
}    // namespace corgi
