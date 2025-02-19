#pragma once

#include <string>
#include <vector>

namespace SDL_GUI {

std::vector<std::string> split_string(std::string s, std::string delimiter, unsigned max = -1);

}
