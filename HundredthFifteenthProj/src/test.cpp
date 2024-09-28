
#include <vector>
#include <iostream>
#include <sstream>
#include <functional>

#include <AL/al.h>
#include <AL/alext.h>

typedef std::stringstream strs;
typedef std::function<std::string (strs &, strs &)> vfunc;

std::string pad(std::string s, size_t size = 16)
{
	while (s.length() < size)
		s.append(" ");
	return s;
}

#undef main
int main(int argc, char **argv)
{
	vfunc OpenAL = [](strs &c, strs &l)
	{
		alIsEnabled(AL_SOURCE_DISTANCE_MODEL);
		c << "N/A";
		l << "N/A";
		return "OpenAL";
	};
	std::vector<vfunc> funcs;
	funcs.push_back(OpenAL);
	for (size_t i = 0; i < funcs.size(); ++i)
	{
		vfunc f = funcs[i];
		strs c, l;
		std::string name = f(c, l);
		std::cout << "-- " << pad(name) << "   compiled: " << pad(c.str(), 7) << "   linked: " << pad(l.str(), 7) << std::endl;
	}
	return getchar();
}
