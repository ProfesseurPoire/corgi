
#include <corgi/rendering/renderer.h>

using namespace corgi;

int main()
{
	/*Window window("window",100,100, 100,100,-1,true);

	window.vsync(false);

	window.renderer().ClearColor(1.0f, 0.0f, 1.0f, 1.0f);

	float	refreshRate = 1.0f;

	Counter counter;
	Counter creationCounter;
	Counter renderCounter;

	int rectangleCount = 100000;
	window.renderer().windowDrawList().reserve(rectangleCount);

	while (!window.is_closing())
	{
		counter.start();

		if (counter.elapsedTime() > refreshRate)
		{
			std::cout <<"FPS : "    <<  counter.tickPerSecond() << std::endl;
			std::cout << "Creation " << creationCounter.elapsedTime()/ creationCounter.ticks() *1000.0f <<" ms"<< std::endl;
			std::cout << "Render "   << renderCounter.elapsedTime()/ renderCounter.ticks() *1000.0f <<" ms"<< std::endl;
			counter.reset();
			creationCounter.reset();
			renderCounter.reset();
		}
		
		window.update_events();
		
		creationCounter.start();
		for (int i = 0; i < rectangleCount; ++i)
		{
			window.renderer().windowDrawList().AddRectangle(10, 10, 10, 10, Color::red);
		}
		creationCounter.tick();


		renderCounter.start();
		window.swap_buffers();
		renderCounter.tick();

		counter.tick();
	}*/
}