#include <corgi/main/Game.h>
#include <corgi/utils/ResourcesCache.h>

#include <corgi/components/SpriteRenderer.h>
#include <corgi/rendering/texture.h>

#include <corgi/ecs/Entity.h>

#include "VectorBenchmark.h"

using namespace corgi;

int main()
{
	//Game game;
	//
	//game.initialize_window("Benchmarks", 100, 100, 100, 100, 1, 1);

	//auto entity = game.scene().new_entity("sprite_renderer");

	//auto sprite_renderer	= entity->add_component<SpriteRenderer>();
	//auto texture			= game.resources_cache().get<Texture>("majestic.tex");
	// 
	//Sprite sprite(texture);
	//
	//game.time().start();

	//std::cout << "Starting Benchmark " << std::endl;

	//// TODO :	Except switching to opengl 4, and using
	////			integer to identify uniforms, there's not much more
	////			I can do here to make this faster
	//
	//for (int i = 0; i < 100000; ++i)
	//{
	//	//sprite_renderer->sprite(sprite);
	//}

	//game.time().pause();
	//const auto time = game.time().elapsed_time();

	//std::cout << "It took " << std::to_string(time) << " s to complete the operation" << std::endl;


	test_vector_comparison();
	
}