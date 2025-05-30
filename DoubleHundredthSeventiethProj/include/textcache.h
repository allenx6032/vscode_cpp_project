#ifndef TEXTCACHE_H
#define TEXTCACHE_H

#include <SDL_render.h>

typedef struct stbtt_fontinfo stbtt_fontinfo;

int          text_cache_cleanup();
SDL_Texture* text_cache_get_texture(stbtt_fontinfo const* font, const char* text, int font_size, SDL_Color color);
int          text_cache_finalize();

#endif //TEXTCACHE_H
