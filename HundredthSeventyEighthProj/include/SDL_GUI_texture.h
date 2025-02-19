#pragma once

#include <string>

#include "SDL_GUI_drawable.h"

namespace SDL_GUI {
/** primitive for rendering a texture */
class Texture : public Drawable {
    std::string _path;      /**< path to texture file */
    SDL_Texture *_texture;  /**< actual texture object */
    virtual Drawable *clone() const override;
protected:
    Texture(std::string type, std::string path, SDL_Renderer *renderer);
public:
    /** mapping from paths to already existent textures */
    static std::map<std::string, SDL_Texture *> _textures;

    /**
     * Constructor
     * @param path path to texture file
     * @param renderer the applications renderer
     */
    Texture(std::string path, SDL_Renderer *renderer)
        : Texture("Texture", path, renderer) {}

    void draw(SDL_Renderer *renderer, Position position) const override;
};
}
