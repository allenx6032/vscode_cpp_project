#ifndef NANA_AUDIO_PLAYER_HPP
#define NANA_AUDIO_PLAYER_HPP
#include <nana_traits.hpp>
#include <nana_deploy.hpp>

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
		player(const nana::string& file);
		~player();

		bool open(const nana::string& file);
		void play();
		void close();
	private:
		implementation* impl_;
	};
}//end namespace audio
}//end namespace nana
#endif