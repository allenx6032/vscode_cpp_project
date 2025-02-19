#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <rapidxml.hpp>

#include "SDL_GUI_drawable.h"
#include "SDL_GUI_tree_builder.h"


namespace SDL_GUI {
/**
 * Parser for xml templates.
 * @tparam TREEBUILDER Class that will process the parsed xml
 * @tparam T The output of a single parsed xml node
 */
template <typename TREEBUILDER, typename T>
class XmlParser {
    TREEBUILDER *_builder = nullptr;    /**< the adapter */

    /**
     * Parse all attributes of an xml node
     * @param node xml node
     * @return attribute map
     */
    std::map<std::string, std::string> parse_attributes(rapidxml::xml_node<> *node) const {
        std::map<std::string, std::string> attributes;
        for (rapidxml::xml_attribute<> *attribute = node->first_attribute(); attribute != nullptr; attribute = attribute->next_attribute()) {
            attributes.insert({attribute->name(), attribute->value()});
        }
        return attributes;
    }

    /**
     * Parse an xml node
     * @param node xml node
     * @return the adapted node
     */
    T *parse_node(rapidxml::xml_node<> *node) const {
        T *parsed_object = this->_builder->construct_node(node->name(), node->value(), this->parse_attributes(node));
        for (rapidxml::xml_node<> *child = node->first_node(); child != nullptr; child = child->next_sibling()) {
            parsed_object->add_child(this->parse_node(child));
        }
        return parsed_object;
    }

public:
    /**
     * Constructor
     * @param builder the adapter
     */
    XmlParser(TREEBUILDER *builder) : _builder(builder) {}

    /**
     * parse an input file
     * @param path input files path
     * @return root of the adapted tree
     */
    T *parse_file(std::string path) const {
        std::ifstream file(path);
        std::vector<char> buffer{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
        buffer.push_back('\0');
        rapidxml::xml_document<> doc;
        doc.parse<0>(&buffer[0]);
        return this->parse_node(doc.first_node());
    }
};
}
