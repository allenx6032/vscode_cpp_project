#pragma once

#include <functional>
#include <map>
#include <string>

namespace SDL_GUI {
/**
 * Base class for an xml Adapter
 * @tparam T output Type
 */
template <typename T>
class TreeBuilder {
public:
    /**
     * adapt a single xml node.
     * @param type the xml nodes type
     * @param attributes the xml nodes attribute map
     * @return adapted node
     */
    virtual T *construct_node(std::string type, std::string value,
                              std::map<std::string, std::string> attributes) const = 0;
};
}
