#include <corgi/rendering/PostProcessing.h>
#include <corgi/rendering/renderer.h>

#include <corgi/resources/Mesh.h>

#include <corgi/components/Camera.h>

#include <corgi/utils/ResourcesCache.h>

#include "corgi/main/Game.h"

namespace corgi
{
//	void PostProcessing::Step::update(Renderer& renderer)
//	{
//		/*Mesh mesh(
//			{
//					{0, 0, 3},
//					{1,3, 2}
//			});
//
//		mesh.add_vertex()
//			.position(-1.0f, -1.0f, 0.0f)
//			.uv(0.0f, 0.0f);
//
//		mesh.add_vertex()
//			.position(1.0f, -1.0f, 0.0f)
//			.uv(1.0f, 0.0f);
//
//		mesh.add_vertex()
//			.position(1.0f, 1.0f, 0.0f)
//			.uv(1.0f, 1.0f);
//
//		mesh.add_vertex()
//			.position(-1.0f, 1.0f, 0.0f)
//			.uv(0.0f, 1.0f);
//
//		mesh.add_triangle(0, 1, 2);
//		mesh.add_triangle(0, 2, 3);
//
//		mesh.update_vertices();*/
//
//		/*renderer.begin_frame_buffer(&framebuffer);
//		renderer.clear();
//		renderer.begin_material(material);
//		renderer.draw(mesh, Matrix());
//		renderer.end_material(material);
//		renderer.end_frame_buffer();*/
//	}
//
//    void PostProcessing::update(const Camera& camera, Renderer& renderer)
//    {
//        for(auto& step : steps_)
//            step.update(renderer);
//
//        render_on_screen(camera, renderer);
//    }
//
//    void PostProcessing::render_on_screen(const Camera& main_camera, Renderer& renderer)
//    {
////		static Material material = *Game::instance().resources_cache().get<Material>("corgi/materials/unlit/unlit_texture.mat");
////
////		// If no step has been defined, we simply used the main camera framebuffer
////		if(steps_.empty())
////			material.set_texture(0, *main_camera.framebuffer()->color_attachment());
////		else
////			material.set_texture(0, *steps_.back().framebuffer.color_attachment());
////
////		Mesh mesh
////		(
////			{
////				{0, 0, 3},
////				{1, 3, 2}
////			}
////		);
////
////		mesh.add_vertex()
////			.position(-1.0f, -1.0f, 0.0f)
////			.uv(0.0f, 0.0f);
////
////		mesh.add_vertex()
////			.position(1.0f, -1.0f, 0.0f)
////			.uv(1.0f, 0.0f);
////
////		mesh.add_vertex()
////			.position(1.0f, 1.0f, 0.0f)
////			.uv(1.0f, 1.0f);
////
////		mesh.add_vertex()
////			.position(-1.0f, 1.0f, 0.0f)
////			.uv(0.0f, 1.0f);
////
////		mesh.add_triangle(0, 1, 2);
////		mesh.add_triangle(0, 2, 3);
////
////		mesh.update_vertices();
////
////		renderer.begin_default_frame_buffer();
////		renderer.clear();
////		renderer.begin_material(material);
//////		renderer.draw(mesh, Matrix());
////
////		renderer.end_frame_buffer();
//    }
}
