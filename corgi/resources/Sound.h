#pragma once


#include <corgi/resources/Resource.h>
#include <corgi/SimpleString.h>

namespace corgi
{
	class Sound : public Resource
	{
		
	public:

		friend class AudioPlayer;

	// Constructors
		
		Sound(const SimpleString& file, const SimpleString& identifier);
		
		~Sound() override;

	// Functions

		int memory_usage() override;
		bool is_wav()const;

	private:

		bool is_wav_ = false;
		void* sound_file_ = nullptr;
		SimpleString name_;
	};
}
