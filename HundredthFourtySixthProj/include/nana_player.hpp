#ifndef NANA_AUDIO_PLAYER_HPP
#define NANA_AUDIO_PLAYER_HPP
#include <nana_push_ignore_diagnostic>
#include <nana_deploy.hpp>

#ifdef NANA_ENABLE_AUDIO

#include <nana_traits.hpp>

namespace nana{	namespace audio
{       /// class player
        /// \brief play an audio file in PCM Windows WAV format 
        ///
        /// \include  audio_player.cpp
	class player
		: private nana::noncopyable
	{
		struct implementation;
	public:
		player();
		player(const std::string& file);
		~player();

		bool open(const std::string& file);
		void play();
		void close();
	private:
		implementation* impl_;
	};
}//end namespace audio
}//end namespace nana

#endif	//NANA_ENABLE_AUDIO

#include <nana_pop_ignore_diagnostic>

#endif
