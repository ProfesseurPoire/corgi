#include "corgi/components/SpriteRenderer.h"
#include "corgi/systems/SpriteRendererSystem.h"

#include <config.h>
#include <corgi/components/Camera.h>
#include <corgi/components/Transform.h>
#include <corgi/ecs/Entity.h>
#include <corgi/main/Game.h>
#include <corgi/resources/Font.h>
#include <corgi/systems/RenderingSystem.h>
#include <corgi/systems/UISystem.h>
#include <corgi/ui/Button.h>
#include <corgi/ui/Panel.h>
#include <corgi/ui/Rectangle.h>
#include <corgi/ui/Text.h>
#include <corgi/ui/Window.h>

using namespace corgi;

int main()
{
    Game game(PROJECT_RESOURCE_DIRECTORY, LIBRARY_RESOURCE_DIRECTORY);

    game.initialize_window("TestUI", 200, 300, 1600, 900, -1, false);

    // I feel like this should be optional
    // Component maps should be built regardless of if
    // something as been called, even though it's probably
    // actually better to initialize them
    game.scene().component_maps().add<Camera>();
    game.scene().component_maps().add<Transform>();
    game.scene().component_maps().add<MeshRenderer>();

    game.scene().emplace_system<RenderingSystem>(game.scene(), *game.scene().component_maps().get<MeshRenderer>());

    // Maybe I could actually do that in the constructor of
    // SpriteRendererSystem no?
    SpriteRendererSystem::initializeSpriteMesh();

    game.update_ = [&]()
    {
        /*DrawList::NineSlice ns;
	    ns.width = 300;
	    ns.height = 300;

	    ns.material             = *game.resources_cache().get<Material>("corgi/materials/unlit/unlit_texture.mat");
	    auto t          = game.resources_cache().get<Texture>("corgi/textures/mpb.tex");
	    ns.material.add_texture(game.resources_cache().get<Texture>("corgi/textures/mpb.tex"));

	    ns.top_border_      = 30.0f;
	    ns.bottom_border_   = 30;
	    ns.left_border_     = 30.0f;
	    ns.right_border_    = 30;

	    ns.left_border_uv_   = static_cast<float>(ns.left_border_) / t->width();
	    ns.right_border_uv_  = static_cast<float>(ns.right_border_) / t->width();
	    ns.top_border_uv_    = static_cast<float>(ns.top_border_)/t->height();
	    ns.bottom_border_uv_ = static_cast<float>(ns.bottom_border_)/t->height();
          
	    const float scale=1.0f;

	    ns.top_border_      *= scale;
	    ns.bottom_border_   *= scale;
	    ns.left_border_     *= scale;
	    ns.right_border_    *= scale;

	    ns.x = 700;
	    ns.y = 200;
	    
	    game.renderer().window_draw_list().add_rectangle(400,0.0f,100.0f,100.0f,0.0f,1.0f,1.0f,1.0f);
	    game.renderer().window_draw_list().add_rectangle(500,0.0f,50.0f,100.0f,0.0f,1.0f,0.0f,1.0f);
	    game.renderer().window_draw_list().add_rectangle(550,0.0f,50.0f,100.0f,0.70f,1.0f,0.20f,1.0f);*/
    };

    game.scene().emplace_system<UISystem>(game.window(), game.inputs());

    auto& ui_system = game.scene().get_system<UISystem>();
    auto& interface = ui_system.emplace_back();

    auto& window = interface.emplace_back<ui::Window>(Game::instance().cursor());

    window.title("TestWindow");
    window.position(100, 100);
    window.dimension(550, 500);

    // It'd probably better not to use emplace_back
    // I'd prefer to construct the thing on the side then
    // add this
    // auto& rectangle = window.emplace_back<ui::Rectangle>();
    // rectangle.position(100,200);
    // rectangle.dimension(100, 100);
    // rectangle.setRadius(5);
    // // auto& r = interface.emplace_back<ui::Rectangle>();
    // // r.position(405, 0);
    // // r.dimension(100, 100);

    // auto& rectangle2 = window.emplace_back<ui::Rectangle>();
    // rectangle2.setColor(1.f, 0.f, 0.f);
    // rectangle2.position(100, 200);
    // rectangle2.dimension(100, 100);
    // rectangle2.setMarginLeft(50);
    // rectangle2.setMarginRight(40);
    // rectangle2.setMarginTop(25);
    // rectangle2.setMarginBottom(25);
    // rectangle2.setRadius(5);

    auto& rectangle3 = window.emplace_back<ui::Rectangle>();
    rectangle3.setColor(0.6f, 0.6f, 0.6f);
    rectangle3.setRadius(10);
    rectangle3.dimension(100, 100);
    rectangle3.position(100, 100);
    rectangle3.setRight(100);

    auto& rectangle4 = window.emplace_back<ui::Rectangle>();
    rectangle4.setColor(0.0f, 0.6f, 0.6f);
    rectangle4.setRadius(10);
    rectangle4.dimension(100, 100);
    rectangle4.position(300, 300);
    rectangle4.setRight(2, true);
    rectangle4.setBottom(10);

    auto& panel = interface.emplace_back<ui::Panel>();
    panel.position(100, 100);
    panel.dimension(200, 200);

    auto& button = window.emplace_back<ui::Button>();
    button.position(300, 300);
    button.text().vertical_alignment(ui::Text::VerticalAlignment::Top);
    button.text().horizontal_alignment(ui::Text::HorizontalAlignmnent::Left);
    button.dimension(200, 100);

    //     auto& button2 = window.emplace_back<ui::Button>();
    //     button2.text().text("I hate sand\nIt's coarse and rough");
    //     button2.text().vertical_alignment(ui::Text::VerticalAlignment::Top);
    //     button2.text().horizontal_alignment(ui::Text::HorizontalAlignmnent::Center);
    //     button2.position(600, 300);
    //     button2.dimension(200, 100);

    //     auto& button3 = window.emplace_back<ui::Button>();
    //     button3.text().text("aaa  aaa");
    //     button3.text().vertical_alignment(ui::Text::VerticalAlignment::Top);
    //     button3.text().horizontal_alignment(ui::Text::HorizontalAlignmnent::Right);
    //     button3.position(900, 300);
    //     button3.dimension(200, 100);

    //     auto& button4 = window.emplace_back<ui::Button>();
    //     button4.text().text("Unlimited\npowa\naa");
    //     button4.text().vertical_alignment(ui::Text::VerticalAlignment::Center);
    //     button4.text().horizontal_alignment(ui::Text::HorizontalAlignmnent::Left);
    //     button4.position(300, 450);
    //     button4.dimension(200, 100);

    // 	//button4.on_update()+=[&]()
    // 	//{
    // 	//	button4.text().text(std::to_string(game.renderer().profiler().triangle_count));
    // 	//};

    //     auto& button5 = window.emplace_back<ui::Button>();
    //     button5.text().text("Set Font Size to 40");
    //     button5.text().vertical_alignment(ui::Text::VerticalAlignment::Center);
    //     button5.text().horizontal_alignment(ui::Text::HorizontalAlignmnent::Center);
    //     button5.position(600, 450);
    //     button5.dimension(200, 100);
    //    // button5.text().font_view.font_ = ResourcesCache::font("coders_crux.fnt");

    //     button5.on_click()+=[&]()
    //     {
    //         if(button5.text().font_view.current_configuration_==0)
    //         {
    //             button5.text().text("Set Font Size to 24");
    //             button5.text().font_view.set(38);
    //         }
    //         else
    //         {
    //             button5.text().text("Set Font Size to 38");
    //             button5.text().font_view.set(24);
    //         }
    //     };

    //     auto& button6 = window.emplace_back<ui::Button>();
    //     button6.text().text("Bonjour tout le monde");
    //     button6.text().vertical_alignment(ui::Text::VerticalAlignment::Center);
    //     button6.text().horizontal_alignment(ui::Text::HorizontalAlignmnent::Right);
    //     button6.position(900, 450);
    //     button6.dimension(200, 100);

    //     button6.on_click() += [&]()
    //     {
    //         auto rendering_mode = static_cast<int>(button5.text().font_view.rendering_mode());

    //         rendering_mode = (rendering_mode + 1) % 5;

    //         button5.text().font_view.set(button5.text().font_view.font_size(), static_cast<Font::RenderingMode>(rendering_mode));

    // 		switch (button5.text().font_view.rendering_mode())
    // 		{
    //         case Font::RenderingMode::Normal:
    //             button6.text().text("Normal");
    //             break;

    //         case Font::RenderingMode::Light:
    //             button6.text().text("Light");
    //             break;

    //         case Font::RenderingMode::Mono:
    //             button6.text().text("Mono");
    //             break;

    //         case Font::RenderingMode::LCD:
    //             button6.text().text("LCD");
    //             break;

    //         case Font::RenderingMode::LCD_V:
    //             button6.text().text("LCD_V");
    //             break;
    // 		}
    //     };

    //     auto& button7 = window.emplace_back<ui::Button>();
    //     button7.text().text("aaaaa");
    //     button7.text().vertical_alignment(ui::Text::VerticalAlignment::Down);
    //     button7.text().horizontal_alignment(ui::Text::HorizontalAlignmnent::Left);
    //     button7.position(300, 600);
    //     button7.dimension(200, 100);

    //     auto& button8 = window.emplace_back<ui::Button>();
    //     button8.text().text("This is how\nliberty dies");
    //     button8.text().vertical_alignment(ui::Text::VerticalAlignment::Down);
    //     button8.text().horizontal_alignment(ui::Text::HorizontalAlignmnent::Center);
    //     button8.position(600, 600);
    //     button8.dimension(200, 100);

    //     auto& button9 = window.emplace_back<ui::Button>();
    //     button9.text().text("aaaaa\nbip boop bip");
    //     button9.text().vertical_alignment(ui::Text::VerticalAlignment::Down);
    //     button9.text().horizontal_alignment(ui::Text::HorizontalAlignmnent::Right);
    //     button9.position(900, 600);
    //     button9.dimension(200, 100);

    auto camera_entity = game.scene().new_entity("Camera");
    camera_entity->add_component<Transform>();

    auto camera_component = camera_entity->add_component<Camera>();

    camera_component->clear_color(0.7f, 0.6f, 0.5f, 1.0f);
    camera_component->ortho(1, game.window().aspect_ratio(), 0, 100);
    camera_component->viewport(0, 0, game.window().width(), game.window().height());
    game.run();
}