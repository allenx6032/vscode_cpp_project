// Note: No need to increase compile time by including TestUtilities/System.hpp
#include <SFML_String.hpp>
#include <SFML_Time.hpp>
#include <sstream>

// String conversions for Catch framework
namespace Catch
{
    std::string toString(const sf::String& string)
    {
        return string.toAnsiString();
    }

    std::string toString(sf::Time time)
    {
        std::ostringstream stream;
        stream << time.asMicroseconds() << "us";
        return stream.str();
    }
}
