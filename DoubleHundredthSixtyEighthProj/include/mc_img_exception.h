#ifndef IMG_EXCEPTION_H_
#define IMG_EXCEPTION_H_
#include "mc_exception.h"
#include <string>
namespace img
{
auto Error() -> std::string;

class Exception : virtual public exception::Exception
{
public:
  typedef boost::error_info<class What, std::string> What;
  char const* what() const noexcept {return "SDL IMG Exception";}
};
}
#endif