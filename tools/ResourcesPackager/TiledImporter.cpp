#include "TiledImporter.h"

#include <corgi/resources/Tilemap.h>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/istreamwrapper.h>

#include <algorithm>
#include <fstream>
#include <ranges>

namespace corgi
{
static void convert_string(std::ofstream& file, const std::string& str)
{
    const auto size = str.size() + 1;
    file.write(reinterpret_cast<const char*>(&size), sizeof size);
    file.write(str.c_str(), size);
}

std::vector<Tilemap::Property> load_properties(rapidjson::Value& value)
{
    std::vector<Tilemap::Property> properties;

    if(!value.HasMember("properties"))
        return properties;

    for(auto& prop : value["properties"].GetArray())
    {
        std::string property_type = prop["type"].GetString();

        if(property_type == "float")
            properties.emplace_back(prop["name"].GetString(), prop["value"].GetFloat());

        if(property_type == "bool")
            properties.emplace_back(prop["name"].GetString(), prop["value"].GetBool());

        if(property_type == "string")
            properties.emplace_back(prop["name"].GetString(),
                                    std::string(prop["value"].GetString()));

        if(property_type == "int")
            properties.emplace_back(prop["name"].GetString(), prop["value"].GetInt());
    }
    return properties;
}

Tilemap::TilesetInfo load_tileset_info(rapidjson::Value& value)
{
    Tilemap::TilesetInfo tsi;

    tsi.firstgid     = value["firstgid"].GetInt();
    tsi.columns      = value["columns"].GetInt();
    tsi.image        = value["image"].GetString();
    tsi.image        = tsi.image.substr(0, tsi.image.size() - 3) + "tex";
    tsi.image_height = value["imageheight"].GetInt();
    tsi.image_width  = value["imagewidth"].GetInt();
    tsi.margin       = value["margin"].GetInt();
    tsi.name         = value["name"].GetString();
    tsi.spacing      = value["spacing"].GetInt();
    tsi.tile_count   = value["tilecount"].GetInt();
    tsi.tile_height  = value["tileheight"].GetInt();
    tsi.tile_width   = value["tilewidth"].GetInt();
    //tsi.transparent_color	= value["transparentcolor"].GetString();

    if(value.HasMember("terrains"))
    {
        for(auto& v : value["terrains"].GetArray())
        {
            tsi.terrains.push_back({v["name"].GetString(), v["tile"].GetInt()});
        }
    }

    if(value.HasMember("tiles"))
    {
        for(auto& v : value["tiles"].GetArray())
        {
            Tilemap::TileInfo i;

            if(v.HasMember("probability"))
                i.probablity = v["probability"].GetFloat();

            i.id = v["id"].GetInt();

            if(v.HasMember("terrains"))
            {
                int iter = 0;
                for(auto& tv : v["terrain"].GetArray())
                {
                    i.terrain[iter++] = tv.GetInt();
                }
            }

            if(v.HasMember("animation"))
            {
                for(auto& animation : v["animation"].GetArray())
                {
                    Tilemap::TileAnimation anim;
                    anim.tile_id  = animation["tileid"].GetInt();
                    anim.duration = animation["duration"].GetInt();
                    i.animations.push_back(anim);
                }
            }

            tsi.tiles.push_back(i);
        }
    }
    return tsi;
}

Tilemap::Object load_object(rapidjson::Value& value, float height)
{
    Tilemap::Object object;

    object.id       = value["id"].GetInt();
    object.name     = value["name"].GetString();
    object.rotation = value["rotation"].GetFloat();
    object.type     = value["type"].GetString();
    object.visible  = value["visible"].GetBool();
    object.width    = value["width"].GetFloat();
    object.height   = value["height"].GetFloat();
    object.x        = value["x"].GetFloat();
    object.y        = value["y"].GetFloat();

    if(value.HasMember("gid"))
        object.gid = value["gid"].GetInt();

    if(value.HasMember("polyline"))
    {
        for(auto& v : value["polyline"].GetArray())
        {
            object.polyline.push_back({v["x"].GetFloat(), height - v["y"].GetFloat()});
        }
    }

    if(value.HasMember("polygon"))
    {
        for(auto& v : value["polygon"].GetArray())
        {
            object.polygon.push_back({v["x"].GetFloat(), height - v["y"].GetFloat()});
        }
    }
    object.properties = load_properties(value);
    return object;
}

Tilemap::ObjectGroup load_object_group(rapidjson::Value& value, float height)
{
    Tilemap::ObjectGroup object_group;

    object_group.opacity    = value["opacity"].GetFloat();
    object_group.visible    = value["visible"].GetBool();
    object_group.name       = value["name"].GetString();
    object_group.id         = value["id"].GetInt();
    object_group.x          = value["x"].GetFloat();
    object_group.y          = value["y"].GetFloat();
    object_group.draw_order = value["draworder"].GetString();

    for(auto& obj : value["objects"].GetArray())
        object_group.objects.push_back(load_object(obj, height));

    object_group.properties = load_properties(value);

    return object_group;
}

Tilemap::TileLayer load_tile_layer(rapidjson::Value& value)
{
    Tilemap::TileLayer layer;

    layer.width   = value["width"].GetFloat();
    layer.height  = value["height"].GetFloat();
    layer.opacity = value["opacity"].GetFloat();
    layer.visible = value["visible"].GetBool();
    layer.name    = value["name"].GetString();
    layer.id      = value["id"].GetInt();
    layer.x       = value["x"].GetFloat();
    layer.y       = value["y"].GetFloat();

    for(auto& data : value["data"].GetArray())
        layer.data.emplace_back(data.GetInt64());

    layer.properties = load_properties(value);

    return layer;
}

static void convert_properties(std::ofstream&                        output,
                               const std::vector<Tilemap::Property>& properties)
{
    const auto property_size = properties.size();
    output.write(reinterpret_cast<const char*>(&property_size), sizeof property_size);

    for(const auto& prop : properties)
    {
        convert_string(output, prop.name_);

        const auto index = static_cast<int>(prop.type);
        output.write(reinterpret_cast<const char*>(&index), sizeof index);

        bool        bval {false};
        float       fval {0.0f};
        int         ival {0};
        std::string sval;

        switch(prop.type)
        {
            case Tilemap::Property::Type::Bool:
                bval = prop.bool_value;
                output.write(reinterpret_cast<const char*>(&bval), sizeof bval);
                break;
            case Tilemap::Property::Type::Float:
                fval = prop.float_value;
                output.write(reinterpret_cast<const char*>(&fval), sizeof fval);
                break;
            case Tilemap::Property::Type::Int:
                ival = prop.int_value;
                output.write(reinterpret_cast<const char*>(&ival), sizeof ival);
                break;
            case Tilemap::Property::Type::String:
                const auto sval = prop.string_value;
                convert_string(output, sval);
                break;
        }
    }
}

static void convert_object(std::ofstream& output, const Tilemap::Object& object)
{
    output.write(reinterpret_cast<const char*>(&object.id), sizeof object.id);
    output.write(reinterpret_cast<const char*>(&object.gid), sizeof object.gid);
    output.write(reinterpret_cast<const char*>(&object.visible), sizeof object.visible);
    output.write(reinterpret_cast<const char*>(&object.rotation), sizeof object.rotation);
    output.write(reinterpret_cast<const char*>(&object.width), sizeof object.width);
    output.write(reinterpret_cast<const char*>(&object.height), sizeof object.height);
    output.write(reinterpret_cast<const char*>(&object.x), sizeof object.x);
    output.write(reinterpret_cast<const char*>(&object.y), sizeof object.y);

    convert_string(output, object.type);
    convert_string(output, object.name);

    int polyline_size = object.polyline.size() * sizeof(Tilemap::Object::Point);
    output.write(reinterpret_cast<const char*>(&polyline_size), sizeof polyline_size);
    if(polyline_size != 0)
        output.write(reinterpret_cast<const char*>(&object.polyline[0]), polyline_size);

    int polygon_size = object.polygon.size() * sizeof(Tilemap::Object::Point);
    output.write(reinterpret_cast<const char*>(&polygon_size), sizeof polygon_size);
    if(polygon_size != 0)
        output.write(reinterpret_cast<const char*>(&object.polygon[0]), polygon_size);

    convert_properties(output, object.properties);
}

static void convert_object_group(std::ofstream&              output,
                                 const Tilemap::ObjectGroup& object_group)
{
    convert_string(output, object_group.name);

    output.write(reinterpret_cast<const char*>(&object_group.id), sizeof object_group.id);
    output.write(reinterpret_cast<const char*>(&object_group.x), sizeof object_group.x);
    output.write(reinterpret_cast<const char*>(&object_group.y), sizeof object_group.y);
    output.write(reinterpret_cast<const char*>(&object_group.opacity),
                 sizeof object_group.opacity);
    output.write(reinterpret_cast<const char*>(&object_group.visible),
                 sizeof object_group.visible);

    convert_properties(output, object_group.properties);
    convert_string(output, object_group.draw_order);

    const auto objects_size = object_group.objects.size();
    output.write(reinterpret_cast<const char*>(&objects_size), sizeof objects_size);

    for(const auto& object : object_group.objects)
        convert_object(output, object);
}

static void convert_tile_layer(std::ofstream&            output,
                               const Tilemap::TileLayer& tile_layer)
{
    convert_string(output, tile_layer.name);

    output.write(reinterpret_cast<const char*>(&tile_layer.id), sizeof tile_layer.id);
    output.write(reinterpret_cast<const char*>(&tile_layer.x), sizeof tile_layer.x);
    output.write(reinterpret_cast<const char*>(&tile_layer.y), sizeof tile_layer.y);
    output.write(reinterpret_cast<const char*>(&tile_layer.opacity),
                 sizeof tile_layer.opacity);
    output.write(reinterpret_cast<const char*>(&tile_layer.visible),
                 sizeof tile_layer.visible);
    output.write(reinterpret_cast<const char*>(&tile_layer.height),
                 sizeof tile_layer.height);
    output.write(reinterpret_cast<const char*>(&tile_layer.width),
                 sizeof tile_layer.width);

    const int data_size = tile_layer.data.size() * sizeof(long long);
    output.write(reinterpret_cast<const char*>(&data_size), sizeof data_size);
    output.write(reinterpret_cast<const char*>(&tile_layer.data[0]), data_size);

    convert_properties(output, tile_layer.properties);
}

static void convert_terrain_info(std::ofstream&              output,
                                 const Tilemap::TerrainInfo& terrain_info)
{
    std::cout << "Terrain info " << terrain_info.tile << std::endl;
    std::cout << "Terrain name" << terrain_info.name.c_str() << std::endl;

    output.write(reinterpret_cast<const char*>(&terrain_info.tile),
                 sizeof terrain_info.tile);
    convert_string(output, terrain_info.name.c_str());
}

static void convert_tile_info(std::ofstream& output, const Tilemap::TileInfo& tile_info)
{
    output.write(reinterpret_cast<const char*>(&tile_info.id), sizeof tile_info.id);
    output.write(reinterpret_cast<const char*>(&tile_info.probablity),
                 sizeof tile_info.probablity);
    output.write(reinterpret_cast<const char*>(&tile_info.terrain),
                 sizeof tile_info.terrain);

    const auto animation_size = tile_info.animations.size();
    output.write(reinterpret_cast<const char*>(&animation_size), sizeof animation_size);

    if(animation_size != 0)
        output.write(reinterpret_cast<const char*>(tile_info.animations.data()),
                     animation_size * sizeof(Tilemap::TileAnimation));
}

static void convert_tileset_info(std::ofstream&              output,
                                 const Tilemap::TilesetInfo& tileset_info)
{
    output.write(reinterpret_cast<const char*>(&tileset_info.firstgid),
                 sizeof tileset_info.firstgid);
    output.write(reinterpret_cast<const char*>(&tileset_info.columns),
                 sizeof tileset_info.columns);
    output.write(reinterpret_cast<const char*>(&tileset_info.image_height),
                 sizeof tileset_info.image_height);
    output.write(reinterpret_cast<const char*>(&tileset_info.image_width),
                 sizeof tileset_info.image_width);
    output.write(reinterpret_cast<const char*>(&tileset_info.margin),
                 sizeof tileset_info.margin);
    output.write(reinterpret_cast<const char*>(&tileset_info.tile_count),
                 sizeof tileset_info.tile_count);
    output.write(reinterpret_cast<const char*>(&tileset_info.tile_height),
                 sizeof tileset_info.tile_height);
    output.write(reinterpret_cast<const char*>(&tileset_info.tile_width),
                 sizeof tileset_info.tile_width);
    output.write(reinterpret_cast<const char*>(&tileset_info.spacing),
                 sizeof tileset_info.spacing);

    convert_string(output, tileset_info.image);
    convert_string(output, tileset_info.name);
    convert_string(output, tileset_info.transparent_color);

    std::cout << "spacing " << tileset_info.spacing << std::endl;

    std::cout << "image " << tileset_info.image.c_str() << std::endl;
    std::cout << "name " << tileset_info.name.c_str() << std::endl;
    std::cout << "transparentColor " << tileset_info.transparent_color.c_str()
              << std::endl;

    auto terrains_size = tileset_info.terrains.size();
    output.write(reinterpret_cast<const char*>(&terrains_size), sizeof terrains_size);
    std::cout << "Terrain Size " << terrains_size << std::endl;
    for(const auto& terrain_info : tileset_info.terrains)
        convert_terrain_info(output, terrain_info);

    auto tiles_size = tileset_info.tiles.size();
    std::cout << "Tiles " << tileset_info.tiles.size() << std::endl;

    output.write(reinterpret_cast<const char*>(&tiles_size), sizeof tiles_size);

    for(const auto& tile_info : tileset_info.tiles)
        convert_tile_info(output, tile_info);
}

void TiledImporter::import(const char* input, const char* output)
{
    // Variables

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

    std::vector<Tilemap::TileLayer>   tile_layers;
    std::vector<Tilemap::ObjectGroup> object_groups;
    std::vector<Tilemap::TilesetInfo> tileset_infos;

    // Loading

    std::ifstream       file(input);
    rapidjson::Document d;
    auto                f = rapidjson::IStreamWrapper(file);
    d.ParseStream(f);

    std::cout << "Height" << d["height"].GetInt() << std::endl;
    std::cout << "Width" << d["width"].GetInt() << std::endl;

    height = d["height"].GetInt();
    width  = d["width"].GetInt();

    tile_height = d["tileheight"].GetInt();
    tile_width  = d["tilewidth"].GetInt();

    nextlayerid    = d["nextlayerid"].GetInt();
    next_object_id = d["nextobjectid"].GetInt();

    version  = d["version"].GetFloat();
    infinite = d["infinite"].GetBool();

    orientation   = d["orientation"].GetString();
    render_order  = d["renderorder"].GetString();
    tiled_version = d["tiledversion"].GetString();
    type          = d["type"].GetString();

    std::ranges::transform(d["tilesets"].GetArray(), std::back_inserter(tileset_infos),
                           load_tileset_info);

    for(auto& layer : d["layers"].GetArray())
    {
        std::string layer_type = layer["type"].GetString();

        if(layer_type == "tilelayer")
            tile_layers.push_back(load_tile_layer(layer));

        if(layer_type == "objectgroup")
            object_groups.push_back(load_object_group(layer, height));
    }

    // Output

    std::ofstream output_file(output, std::ofstream::binary | std::ofstream::trunc);

    output_file.write(reinterpret_cast<const char*>(&width), sizeof width);
    output_file.write(reinterpret_cast<const char*>(&height), sizeof height);

    output_file.write(reinterpret_cast<const char*>(&tile_width), sizeof tile_width);
    output_file.write(reinterpret_cast<const char*>(&tile_height), sizeof tile_height);

    output_file.write(reinterpret_cast<const char*>(&nextlayerid), sizeof nextlayerid);
    output_file.write(reinterpret_cast<const char*>(&next_object_id),
                      sizeof next_object_id);

    output_file.write(reinterpret_cast<const char*>(&version), sizeof version);

    output_file.write(reinterpret_cast<const char*>(&infinite), sizeof infinite);

    std::cout << "Orientation " << orientation.c_str() << std::endl;

    convert_string(output_file, orientation);
    convert_string(output_file, render_order);
    convert_string(output_file, tiled_version);
    convert_string(output_file, type);

    const int tileset_infos_size = tileset_infos.size();
    output_file.write(reinterpret_cast<const char*>(&tileset_infos_size),
                      sizeof tileset_infos_size);

    for(auto tileset : tileset_infos)
        convert_tileset_info(output_file, tileset);

    const int tile_layers_size = tile_layers.size();
    output_file.write(reinterpret_cast<const char*>(&tile_layers_size),
                      sizeof tile_layers_size);

    std::cout << "tile_layers" << tile_layers_size << std::endl;

    for(const auto& layer : tile_layers)
        convert_tile_layer(output_file, layer);

    const int object_groups_size = object_groups.size();
    output_file.write(reinterpret_cast<const char*>(&object_groups_size),
                      sizeof object_groups_size);

    std::cout << "Object Group " << object_groups_size << std::endl;

    for(const auto& object_group : object_groups)
        convert_object_group(output_file, object_group);
}

//void TiledImporter::generate(corgi::Scene& scene, corgi::Texture* tileset)
//{
//	int it = 0;
//	// Generate a tilemap for every layer
//
//	for (auto& tile_layer : _tile_layers)
//	{
//		corgi::Entity& tm = scene.root()->push_back("tilemap");
//
//		tm.add_component<corgi::Transform>();
//		tm.add_component<corgi::Tilemap>();
//
//		tm.get_component<corgi::Transform>()->position(0.0f, 0.0f, -2.0f);
//		//auto tilemap = tm.get_component<corgi::Tilemap>();
//
//		tilemap->depth			= (it++) * 0.21f;
//		tilemap->_height		= tile_layer.height;
//		tilemap->_width			= tile_layer.width;
//
//		tilemap->set_tileset(tileset,
//			_tileset_infos[0].tilewidth,
//			_tileset_infos[0].tile_height);
//
//		tilemap->_x				= 0;
//		tilemap->_y				= 0;
//		tilemap->tiles			= tile_layer.data;
//	}
//}
}    // namespace corgi