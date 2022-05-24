#include "Tilemap.h"
#include <fstream>

namespace corgi
{
    static void read_string(std::fstream& file, SimpleString& in)
    {
        int size;
        file.read(reinterpret_cast<char*>(&size), sizeof size);
        char* str = new char[size];
        file.read(str, size);
        in = str;
        delete[] str;
    }

    static void read_properties(std::fstream& file, Vector<Tilemap::Property>& properties)
    {
        int property_size{0u};
        file.read(reinterpret_cast<char*>(&property_size), sizeof property_size);

        for(size_t i = 0; i < property_size; ++i)
        {
            SimpleString key;
            read_string(file, key);

            int index;
            file.read(reinterpret_cast<char*>(&index), sizeof index);

            bool         bval{false};
            float        fval{0.0f};
            int          ival{0};
            SimpleString sval;

            switch(index)
            {
                case 0u:
                    file.read(reinterpret_cast<char*>(&bval), sizeof bval);
                    properties.emplace_back(key, bval);
                    break;
                case 1u:
                    file.read(reinterpret_cast<char*>(&fval), sizeof fval);
                    properties.emplace_back(key, fval);
                    break;
                case 2u:
                    file.read(reinterpret_cast<char*>(&ival), sizeof ival);
                    properties.emplace_back(key, ival);
                    break;
                case 3u:
                    read_string(file, sval);
                    properties.emplace_back(key, sval);
                    break;
            }
        }
    }

    Tilemap::Object::Object(std::fstream& file)
    {
        file.read(reinterpret_cast<char*>(&id), sizeof id);
        file.read(reinterpret_cast<char*>(&gid), sizeof gid);
        file.read(reinterpret_cast<char*>(&visible), sizeof visible);
        file.read(reinterpret_cast<char*>(&rotation), sizeof rotation);
        file.read(reinterpret_cast<char*>(&width), sizeof width);
        file.read(reinterpret_cast<char*>(&height), sizeof height);
        file.read(reinterpret_cast<char*>(&x), sizeof x);
        file.read(reinterpret_cast<char*>(&y), sizeof y);

        read_string(file, type);
        read_string(file, name);

        int polyline_size;
        file.read(reinterpret_cast<char*>(&polyline_size), sizeof polyline_size);

        polyline.resize(polyline_size / sizeof(Tilemap::Object::Point));
        if(polyline_size != 0)
            file.read(reinterpret_cast<char*>(&polyline[0]), polyline_size);

        int polygon_size;
        file.read(reinterpret_cast<char*>(&polygon_size), sizeof polygon_size);

        polyline.resize(polygon_size / sizeof(Tilemap::Object::Point));
        if(polygon_size != 0)
            file.read(reinterpret_cast<char*>(&polygon[0]), polygon_size);

        read_properties(file, properties);
    }

    Tilemap::ObjectGroup::ObjectGroup(std::fstream& file)
    {
        read_string(file, name);

        file.read(reinterpret_cast<char*>(&id), sizeof id);
        file.read(reinterpret_cast<char*>(&x), sizeof x);
        file.read(reinterpret_cast<char*>(&y), sizeof y);
        file.read(reinterpret_cast<char*>(&opacity), sizeof opacity);
        file.read(reinterpret_cast<char*>(&visible), sizeof visible);

        read_properties(file, properties);

        read_string(file, draw_order);

        int objects_size{0u};
        file.read(reinterpret_cast<char*>(&objects_size), sizeof objects_size);

        objects.reserve(objects_size);

        for(int i{0}; i < objects_size; ++i)
            objects.emplace_back(file);
    }

    Tilemap::TileLayer::TileLayer(std::fstream& file)
    {
        read_string(file, name);

        file.read(reinterpret_cast<char*>(&id), sizeof id);
        file.read(reinterpret_cast<char*>(&x), sizeof x);
        file.read(reinterpret_cast<char*>(&y), sizeof y);
        file.read(reinterpret_cast<char*>(&opacity), sizeof opacity);
        file.read(reinterpret_cast<char*>(&visible), sizeof visible);
        file.read(reinterpret_cast<char*>(&height), sizeof height);
        file.read(reinterpret_cast<char*>(&width), sizeof width);

        int data_size{0u};
        file.read(reinterpret_cast<char*>(&data_size), sizeof data_size);
        data.resize(data_size / sizeof(long long));

        if(data_size != 0)
            file.read(reinterpret_cast<char*>(&data[0]), data_size);

        read_properties(file, properties);
    }

    Tilemap::TerrainInfo::TerrainInfo(std::fstream& file)
    {
        file.read(reinterpret_cast<char*>(&tile), sizeof tile);
        read_string(file, name);
    }


    Tilemap::Property Tilemap::Object::property(const SimpleString& key) const
    {
        for(int i = 0; i < properties.size(); i++)
        {
            if(properties[i].name_ == key)
            {
                return properties[i];
            }
        }

        return Property("", "");
    }

    static void read_tileset_info(std::fstream& file, Tilemap::TilesetInfo& tileset_info)
    {
        file.read(reinterpret_cast<char*>(&tileset_info.firstgid), sizeof tileset_info.firstgid);
        file.read(reinterpret_cast<char*>(&tileset_info.columns), sizeof tileset_info.columns);
        file.read(reinterpret_cast<char*>(&tileset_info.image_height), sizeof tileset_info.image_height);
        file.read(reinterpret_cast<char*>(&tileset_info.image_width), sizeof tileset_info.image_width);
        file.read(reinterpret_cast<char*>(&tileset_info.margin), sizeof tileset_info.margin);
        file.read(reinterpret_cast<char*>(&tileset_info.tile_count), sizeof tileset_info.tile_count);
        file.read(reinterpret_cast<char*>(&tileset_info.tile_height), sizeof tileset_info.tile_height);
        file.read(reinterpret_cast<char*>(&tileset_info.tile_width), sizeof tileset_info.tile_width);
        file.read(reinterpret_cast<char*>(&tileset_info.spacing), sizeof tileset_info.spacing);

        read_string(file, tileset_info.image);
        read_string(file, tileset_info.name);
        read_string(file, tileset_info.transparent_color);

        int terrains_size{0u};
        file.read(reinterpret_cast<char*>(&terrains_size), sizeof terrains_size);
        tileset_info.terrains.reserve(terrains_size);

        for(size_t i = 0u; i < terrains_size; ++i)
            tileset_info.terrains.emplace_back(file);

        int tiles_size{0};
        file.read(reinterpret_cast<char*>(&tiles_size), sizeof tiles_size);
        tileset_info.tiles.reserve(tiles_size);

        for(size_t i = 0u; i < tiles_size; ++i)
        {
            Tilemap::TileInfo tile_info;

            file.read(reinterpret_cast<char*>(&tile_info.id), sizeof tile_info.id);
            file.read(reinterpret_cast<char*>(&tile_info.probablity), sizeof tile_info.probablity);
            file.read(reinterpret_cast<char*>(&tile_info.terrain), sizeof tile_info.terrain);

            int animation_size;
            file.read(reinterpret_cast<char*>(&animation_size), sizeof animation_size);

            tile_info.animations.resize(animation_size);

            if(animation_size != 0)
                file.read(reinterpret_cast<char*>(&tile_info.animations[0]),
                          animation_size * sizeof(Tilemap::TileAnimation));

            tileset_info.tiles.emplace_back(tile_info);
        }
    }

    Tilemap::Tilemap(const SimpleString& path, const SimpleString& identifier)
        : identifier(identifier)
    {
        std::fstream file(path.c_str(), std::fstream::binary | std::fstream::in);

        if(!file.is_open())
        {
            throw std::invalid_argument("Path is not valid");
        }

        file.read(reinterpret_cast<char*>(&width), sizeof width);
        file.read(reinterpret_cast<char*>(&height), sizeof height);

        file.read(reinterpret_cast<char*>(&tile_width), sizeof tile_width);
        file.read(reinterpret_cast<char*>(&tile_height), sizeof tile_height);

        file.read(reinterpret_cast<char*>(&nextlayerid), sizeof nextlayerid);
        file.read(reinterpret_cast<char*>(&next_object_id), sizeof next_object_id);
        file.read(reinterpret_cast<char*>(&version), sizeof version);

        file.read(reinterpret_cast<char*>(&infinite), sizeof infinite);

        read_string(file, orientation);
        read_string(file, render_order);
        read_string(file, tiled_version);
        read_string(file, type);

        

        int tileset_info_size;
        file.read(reinterpret_cast<char*>(&tileset_info_size), sizeof tileset_info_size);
        tileset_infos.reserve(tileset_info_size);

        for(size_t i = 0u; i < tileset_info_size; ++i)
        {
            tileset_infos.emplace_back();
            read_tileset_info(file, tileset_infos[i]);
        }

        int tile_layers_size{0u};
        file.read(reinterpret_cast<char*>(&tile_layers_size), sizeof tile_layers_size);

        tile_layers.reserve(tile_layers_size);

        for(size_t i = 0u; i < tile_layers_size; ++i)
            tile_layers.emplace_back(file);

        int object_groups_size{0u};
        file.read(reinterpret_cast<char*>(&object_groups_size), sizeof object_groups_size);

        object_groups.reserve(object_groups_size);

        for(size_t i{0u}; i < object_groups_size; ++i)
            object_groups.emplace_back(file);
    }
}
