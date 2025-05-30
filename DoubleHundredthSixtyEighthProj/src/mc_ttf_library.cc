#include "mc_ttf_library.h"
#include "mc_SDL_ttf.h"
#include "mc_ttf_exception.h"
namespace
{
int reference_count = 0;
}

namespace ttf
{
Library::Library()
{
  if(!TTF_WasInit() && TTF_Init()==-1) 
  {
    BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error()));
  }
  ++reference_count;
}

Library::~Library()
{
  if(reference_count > 0)
  {
    --reference_count;
    if(reference_count == 0)
    {
      TTF_Quit();
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