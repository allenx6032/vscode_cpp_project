#include "mc_sdl_library.h"
#include <climits>
#include "mc_sdl_exception.h"
#include "SDL.h"
namespace
{
bool initialised = false;
int reference_count[CHAR_BIT * sizeof(Uint32)] = {0};
}

namespace sdl
{
Library::Library(Uint32 flags) : flags_(flags)
{
  if(!initialised)
  {
    SDL_SetMainReady();
    if(SDL_Init(flags) < 0)
    {
      BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error()));
    }
    initialised = true;
  }
  else
  {
    int init_flags = flags & (SDL_WasInit(0u) ^ flags);
    if(SDL_InitSubSystem(init_flags) < 0)
    {
      BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error()));
    }
  }

  int index = 0;
  while(flags)
  {
    if(flags & 0x01u)
    {
      reference_count[index]++;
    }
    flags >>= 1u;
    index++;
  }
}

Library::~Library()
{
  int index = 0;
  Uint32 quit_flags = 0x00u;
  Uint32 flags = flags_;

  while(flags)
  {
    if(flags & 0x01u)
    {
      if(reference_count[index] > 0)
      {
        reference_count[index]--;

        if(reference_count[index] == 0)
        {
          quit_flags |= static_cast<Uint32>(0x01u) << index;
        }
      }
    }
    flags >>= 1u;
    index++;
  }

  SDL_QuitSubSystem(quit_flags);

  if(SDL_WasInit(0u) == 0x00u)
  {
    SDL_Quit();
    initialised = false;
  }
}

Library::Library(Library const& other) : Library(other.flags_)
{
}

Library::Library(Library&& other) : Library(0)
{
  std::swap(flags_, other.flags_);
}

Library& Library::operator=(Library other)
{
  std::swap(flags_, other.flags_);
  return *this;
}
}