#ifndef MIX_EXCEPTION_H_
#define MIX_EXCEPTION_H_
#include <string>
#include "mc_exception.h"
namespace mix
{
auto Error() -> std::string;

class Exception : virtual public exception::Exception
{
public:
  typedef boost::error_info<class What, std::string> What;
  char const* what() const noexcept {return "SDL Mixer Exception";}
};
}
#endif