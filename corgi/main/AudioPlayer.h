#pragma once

namespace corgi
{
	class Sound;
	
	class AudioPlayer
	{
	public:

	// Lifecycle

		AudioPlayer();
		~AudioPlayer();

		AudioPlayer(const AudioPlayer& other)=delete;
		AudioPlayer(AudioPlayer&& other)=delete;

		AudioPlayer& operator=(const AudioPlayer& other)=delete;
		AudioPlayer& operator=(AudioPlayer&& other)=delete;
		
	// Functions

		
		void initialize();
		void finalize();

		bool is_playing(int channel)const;

		void stop_playing(int channel)const;

		/*
		 * @brief Play a wav file
		 *
		 *@param channel	If channel equals to -1, the audio library will chose
		 *					the first channel available to play the sound
		 */
		void play(const Sound& sound, int channel = -1);
	};
}
