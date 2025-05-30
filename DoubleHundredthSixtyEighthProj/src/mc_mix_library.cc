#include <map>
#include <string>
#include "SDL.h"
#include "SDL_mixer.h"
#include "mc_mix_library.h"
#include "mc_sdl_library.h"
#include "mc_mix_exception.h"
namespace
{
int reference_count;

auto MixQuit() -> void
{
  while(Mix_Init(0))
  {
    Mix_Quit();
  }
}
}

namespace mix
{
Library::Library() : sdl_(SDL_INIT_AUDIO)
{
  if(reference_count == 0)
  {
    int flags = MIX_INIT_OGG;
    if((Mix_Init(flags) & flags) != flags) 
    {
      BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error()));
    }

    if(Mix_OpenAudio(44100, AUDIO_S16, 2, 1024) == -1)
    {
      MixQuit();
      BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error()));
    }

    if(Mix_AllocateChannels(1024) != 1024)
    {
      Mix_CloseAudio();
      MixQuit();
      BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error()));
    }
  }
  reference_count++;
}

Library::~Library()
{
  if(reference_count > 0)
  {
    --reference_count;  
    if(reference_count == 0)
    {
      Mix_CloseAudio();
      MixQuit();
    }
  }
}

Library::Library(Library const&) : Library()
{
}

Library::Library(Library&&) : Library()
{
}

Library& Library::operator=(Library)
{
  return *this;
}
}