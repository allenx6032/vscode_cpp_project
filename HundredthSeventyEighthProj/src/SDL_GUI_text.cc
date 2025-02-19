#include<SDL_GUI_text.h>

#include <cassert>

#include <SDL_GUI_string.h>

using namespace SDL_GUI;

Text::Text(TTF_Font *font, const std::string text, Position position)
    : Drawable("Text", position), _font(font), _text(text) {
    this->create_surfaces();
}

Text::~Text() {
    for (SDL_Surface *s: this->_surfaces) {
        SDL_FreeSurface(s);
    }
    if (this->_surface != nullptr) {
        SDL_FreeSurface(this->_surface);
    }
}

Drawable *Text::clone() const {
    return new Text(this->_font, this->_text, this->_position);
}

void Text::create_surfaces() {
    for (SDL_Surface *s: this->_surfaces) {
        SDL_FreeSurface(s);
    }
    this->_surfaces.clear();
    if (this->_surface != nullptr) {
        SDL_FreeSurface(this->_surface);
    }

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 rmask = 0xff000000;
    Uint32 gmask = 0x00ff0000;
    Uint32 bmask = 0x0000ff00;
    Uint32 amask = 0x000000ff;
#else
    Uint32 rmask = 0x000000ff;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x00ff0000;
    Uint32 amask = 0xff000000;
#endif

    std::vector<std::string> lines = split_string(this->_text, "\n");
    for (std::string line: lines) {
        SDL_Surface *s = TTF_RenderText_Blended(this->_font, line.c_str(),
                                                this->_style._color);
        if (s == nullptr) {
            continue;
        }
        this->_surfaces.push_back(s);
    }

    this->_surface = SDL_CreateRGBSurface(0, this->width(), this->height(), 32, rmask, gmask, bmask, amask);

    int lineskip = TTF_FontLineSkip(this->_font);
    SDL_Rect dstrect = {0,0,0,0};
    for (SDL_Surface *s: this->_surfaces) {
        SDL_BlitSurface(s, NULL, this->_surface, &dstrect);
        dstrect.y += lineskip;
    }
}

void Text::draw(SDL_Renderer *renderer, Position position) const {
    SDL_Rect dstrect{position._x, position._y, this->_surface->w, this->_surface->h};
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, this->_surface);
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_DestroyTexture(texture);
}

void Text::set_text(const std::string text) {
    this->_text = text;
    this->create_surfaces();
}

void Text::set_color(RGB color) {
    this->_style._color = color;
    this->create_surfaces();
}

unsigned Text::height() const {
    return this->_surfaces.size() * TTF_FontLineSkip(this->_font);
}

unsigned Text::width() const {
    unsigned width = 0;
    for (SDL_Surface *s: this->_surfaces) {
        width = std::max(width, static_cast<unsigned>(s->w));
    }
    return width;
}
