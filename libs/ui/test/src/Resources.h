
#pragma once

#include <string>
#include <vector>

#include<corgi/utils/ResourcesCache.h>

namespace frog
{
	namespace Resources
	{
		namespace Textures
{
			static corgi::Texture* grey(){return corgi::ResourcesCache::get<Texture>("grey.png");}
			static corgi::Texture* grey_pressed(){return corgi::ResourcesCache::get<Texture>("grey_pressed.png");}
			static corgi::Texture* image(){return corgi::ResourcesCache::get<Texture>("image.png");}
		} 
		namespace Animations
{
}}}