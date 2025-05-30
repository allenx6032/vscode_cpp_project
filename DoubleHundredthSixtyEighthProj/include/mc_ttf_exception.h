#ifndef TTF_EXCEPTION_H_
#define TTF_EXCEPTION_H_
#include "mc_exception.h"
#include <string>
namespace ttf
{
auto Error() -> std::string;

class Exception : virtual public exception::Exception
{
public:
  typedef boost::error_info<class What, std::string> What;
  char const* what() const noexcept {return "SDL TTF Exception";}
};
}
#endif