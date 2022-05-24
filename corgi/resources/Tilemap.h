#pragma once

#include <corgi/SimpleString.h>
#include <corgi/containers/Vector.h>
#include <corgi/resources/Resource.h>

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
                bool         bool_value;
                float        float_value;
                int          int_value;
                SimpleString string_value;
            };

            SimpleString name_;
            Type         type;

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
                        new(&string_value) SimpleString(prop.string_value);
                        break;
                    default:
                        break;
                }

                name_ = prop.name_;
                type  = prop.type;
            }

            explicit Property(const SimpleString& name, bool value)
                : bool_value(value)
                  , name_(name)
                  , type(Type::Bool)
            {
            }

            explicit Property(const SimpleString& name, float value)
                : float_value(value)
                  , name_(name)
                  , type(Type::Float)
            {
            }

            explicit Property(const SimpleString& name, int value)
                : int_value(value)
                  , name_(name)
                  , type(Type::Int)
            {
            }

            Property(const SimpleString& name, const SimpleString& value)
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
                        (&string_value)->~SimpleString();
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

            Property property(const SimpleString& key) const;

            float        height{0.0f};
            int          id{0};
            int          gid{-1};
            float        rotation{0.0f};
            SimpleString type;
            SimpleString name;
            bool         visible{false};
            float        width{0.0f};
            float        x{0.0f};

            struct Point
            {
                float x{0};
                float y{0};
            };

            Vector<Point> polyline;
            Vector<Point> polygon;

            float y{0};

            Vector<Property> properties;
        };

        struct Layer
        {
            SimpleString name;
            int          id{0};
            float        x{0.0f};
            float        y{0.0f};
            float        opacity{0.0f};
            bool         visible{false};

            Vector<Property> properties;
        };

        // Could use inheritance here
        struct ObjectGroup : public Layer
        {
            ObjectGroup() = default;
            ObjectGroup(std::fstream& file);
            SimpleString            draw_order;
            Vector<Tilemap::Object> objects;
        };

        struct TileLayer : public Layer
        {
            TileLayer() = default;
            TileLayer(std::fstream& file);
            float height{0.0f};
            float width{0.0f};
            // Tiles are apparently stored as long long
            // though tiles can encode it differently or even
            // compress it but that'll do for now
            Vector<long long> data;
        };

        struct TerrainInfo
        {
            TerrainInfo() = default;
            TerrainInfo(std::fstream& file);

            TerrainInfo(SimpleString n, int t)
                : name(n)
                  , tile(t)
            {
            }

            SimpleString name;
            int          tile{0};
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
            int   id{0};
            float probablity{0.0f};
            int   terrain[4];

            Vector<TileAnimation> animations;
        };

        struct TilesetInfo
        {
            int      firstgid{0};
            int      columns{0};
            int      image_height{0};
            int      image_width{0};
            int      margin{0};
            int      tile_count{0};
            unsigned tile_height{0u};
            unsigned tile_width{0u};
            int      spacing{0};

            SimpleString image;
            SimpleString name;
            SimpleString transparent_color;

            Vector<TerrainInfo> terrains;
            Vector<TileInfo>    tiles;
        };

        // Lifecycle

        Tilemap(const SimpleString& path, const SimpleString& identifier);

        SimpleString identifier;

        int   height{0};
        int   width{0};
        int   nextlayerid{0};
        int   next_object_id{0};
        int   tile_height{0};
        int   tile_width{0};
        float version{0.0f};
        bool  infinite{false};

        SimpleString orientation;
        SimpleString render_order;
        SimpleString tiled_version;
        SimpleString type;

        Vector<TilesetInfo> tileset_infos;
        Vector<TileLayer>   tile_layers;
        Vector<ObjectGroup> object_groups;
    };
} // namespace corgi
