#include <SDL_GUI_string.h>

using namespace SDL_GUI;


std::vector<std::string> SDL_GUI::split_string(std::string s, std::string delimiter, unsigned max) {
    std::vector<std::string> ret;
    size_t pos;
    for (unsigned i = max; i != 0; --i) {
        pos = s.find(delimiter);
        if (pos == std::string::npos) {
            break;
        }
        ret.push_back(s.substr(0, pos));
        s.erase(0, pos + delimiter.length());
    }
    ret.push_back(s);
    return ret;
}

