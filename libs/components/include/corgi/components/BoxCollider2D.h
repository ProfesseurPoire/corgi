#pragma once

#include <corgi/components/ColliderComponent.h>

namespace corgi
{
    class BoxCollider2D : public ColliderComponent
    {
    public:

    // Lifecycle
    	
        BoxCollider2D();
        BoxCollider2D(int layer, float width, float height);
        BoxCollider2D(int layer, int width, int height);

    // Functions

        void dimensions(float width, float height);
    	
        [[nodiscard]] float width()const noexcept;
        [[nodiscard]] float height()const noexcept;

        void width(float value);
        void height(float value);

        std::shared_ptr<Mesh> _edges_mesh;	// 8 

        [[nodiscard]] const std::vector<Vec2>& edges()    const noexcept;
        [[nodiscard]] const std::vector<Vec2>& positions()const noexcept;
        [[nodiscard]] const std::vector<Vec2>& axes() const noexcept;
    
    private:

        void build_box();
    	
        float _width    { 1.0f};
        float _height   { 1.0f };

        std::vector<Vec2> _edges;
        std::vector<Vec2> _positions;
        std::vector<Vec2> _axes;
    };
}
