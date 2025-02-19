#include<SDL_GUI_attributable.h>

#include <algorithm>

using namespace SDL_GUI;

void Attributable::add_attribute(std::string attribute) {
    this->_attributes.push_back(attribute);
}

void Attributable::add_attributes(std::vector<std::string> attributes) {
    this->_attributes.insert(this->_attributes.end(), attributes.begin(), attributes.end());
}

void Attributable::remove_attribute(std::string attribute) {
    auto it = std::find(this->_attributes.begin(), this->_attributes.end(), attribute);
    if (it != this->_attributes.end()) {
        this->_attributes.erase(it);
    }
}

void Attributable::clear_attributes() {
    this->_attributes.clear();
}

bool Attributable::has_attribute(std::string attribute) const {
    auto found = std::find(this->_attributes.begin(), this->_attributes.end(), attribute);
    return found != this->_attributes.end();
}

std::vector<std::string> Attributable::attributes() const {
    return this->_attributes;
}
