#pragma once

#include <corgi/resources/Resource.h>

#include <string>
#include <vector>
namespace corgi
{
class Tilemap : public Resource
{
public:
    struct Property
    {
        enum class Type
        {
            Bool,
            Float,
            Int,
            String
        };

        union
        {
            bool        bool_value;
            float       float_value;
            int         int_value;
            std::string string_value;
        };

        std::string name_;
        Type        type;

        Property(const Property& prop)
        {
            switch(prop.type)
            {
                case Type::Bool:
                    bool_value = prop.bool_value;
                    break;

                case Type::Int:
                    int_value = prop.int_value;
                    break;

                case Type::Float:
                    float_value = prop.float_value;
                    break;

                case Type::String:
                    new(&string_value) std::string(prop.string_value);
                    break;
                default:
                    break;
            }

            name_ = prop.name_;
            type  = prop.type;
        }

        Property& operator=(const Property& prop)
        {
            switch(prop.type)
            {
                case Type::Bool:
                    bool_value = prop.bool_value;
                    break;

                case Type::Int:
                    int_value = prop.int_value;
                    break;

                case Type::Float:
                    float_value = prop.float_value;
                    break;

                case Type::String:
                    new(&string_value) std::string(prop.string_value);
                    break;
                default:
                    break;
            }
            name_ = prop.name_;
            type  = prop.type;
            return *this;
        }

        explicit Property(const std::string& name, bool value)
            : bool_value(value)
            , name_(name)
            , type(Type::Bool)
        {
        }

        explicit Property(const std::string& name, float value)
            : float_value(value)
            , name_(name)
            , type(Type::Float)
        {
        }

        explicit Property(const std::string& name, int value)
            : int_value(value)
            , name_(name)
            , type(Type::Int)
        {
        }

        Property(const std::string& name, const std::string& value)
            : string_value(value)
            , name_(name)
            , type(Type::String)
        {
        }

        ~Property()
        {
            switch(type)
            {
                case Type::String:
                    (&string_value)->~basic_string();
                    break;
                default:
                    break;
            }
        }
    };

    Tilemap() = default;

    struct Object
    {
        Object() = default;
        Object(std::fstream& file);

        Property property(const std::string& key) const;

        float       height {0.0f};
        int         id {0};
        int         gid {-1};
        float       rotation {0.0f};
        std::string type;
        std::string name;
        bool        visible {false};
        float       width {0.0f};
        float       x {0.0f};

        struct Point
        {
            float x {0};
            float y {0};
        };

        std::vector<Point> polyline;
        std::vector<Point> polygon;

        float y {0};

        std::vector<Property> properties;
    };

    struct Layer
    {
        std::string name;
        int         id {0};
        float       x {0.0f};
        float       y {0.0f};
        float       opacity {0.0f};
        bool        visible {false};

        std::vector<Property> properties;
    };

    // Could use inheritance here
    struct ObjectGroup : public Layer
    {
        ObjectGroup() = default;
        ObjectGroup(std::fstream& file);
        std::string                  draw_order;
        std::vector<Tilemap::Object> objects;
    };

    struct TileLayer : public Layer
    {
        TileLayer() = default;
        TileLayer(std::fstream& file);
        float height {0.0f};
        float width {0.0f};
        // Tiles are apparently stored as long long
        // though tiles can encode it differently or even
        // compress it but that'll do for now
        std::vector<long long> data;
    };

    struct TerrainInfo
    {
        TerrainInfo() = default;
        TerrainInfo(std::fstream& file);

        TerrainInfo(std::string n, int t)
            : name(n)
            , tile(t)
        {
        }

        std::string name;
        int         tile {0};
    };

    struct TileAnimation
    {
        unsigned tile_id;
        int      duration;
    };

    // Only special tiles have a TileInfo object
    // Tiles that have an animation, a probability or
    // part of a terrain
    struct TileInfo
    {
        int   id {0};
        float probablity {0.0f};
        int   terrain[4];

        std::vector<TileAnimation> animations;
    };

    struct TilesetInfo
    {
        int      firstgid {0};
        int      columns {0};
        int      image_height {0};
        int      image_width {0};
        int      margin {0};
        int      tile_count {0};
        unsigned tile_height {0u};
        unsigned tile_width {0u};
        int      spacing {0};

        std::string image;
        std::string name;
        std::string transparent_color;

        std::vector<TerrainInfo> terrains;
        std::vector<TileInfo>    tiles;
    };

    // Lifecycle

    Tilemap(const std::string& path, const std::string& identifier);

    std::string identifier;

    int   height {0};
    int   width {0};
    int   nextlayerid {0};
    int   next_object_id {0};
    int   tile_height {0};
    int   tile_width {0};
    float version {0.0f};
    bool  infinite {false};

    std::string orientation;
    std::string render_order;
    std::string tiled_version;
    std::string type;

    std::vector<TilesetInfo> tileset_infos;
    std::vector<TileLayer>   tile_layers;
    std::vector<ObjectGroup> object_groups;
};
}    // namespace corgi
