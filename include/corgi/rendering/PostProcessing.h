#pragma once

#include <corgi/rendering/Material.h>
#include <corgi/rendering/FrameBuffer.h>

#include <string>
#include <vector>

namespace corgi
{
	class Camera;

	class Renderer;

	// The goal here is to make post processing easier. So, what's post processing?
	// Basically it's about taking textures as input, create a quad, use a shader and draw the texture
	// into the quad. And then, either we directly draw on the default framebuffer, or we write the output
	// as a texture and reuse that later on
	// By default, if not Step have been specified, it'll look for a camera called "main_camera" in the scene, 
	// get back the color attachment of its framebuffer and display it as is
	//class PostProcessing
	//{
	//	public:

	//		class Step
	//		{
	//			public:

	//			Step(int w, int h)
	//				: material("name"),framebuffer(w,h)
	//			{}

	//			std::string		name ="";
	//			Material		material;
	//			FrameBuffer		framebuffer;

	//			void update(Renderer& renderer);
	//		};

	//		void update(const Camera& camera, Renderer& renderer);

	//		std::vector<Step> steps_;

	//	private:

	//		// Actually draw on the window/final framebuffer
	//		// Not sure how to actually call that
	//		void render_on_screen(const Camera& main_camera, Renderer& renderer);

	//};
}