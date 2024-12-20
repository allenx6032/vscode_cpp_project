#ifndef SDLRENDERER_H_
#define SDLRENDERER_H_

#include "SLO_SDLColor.h"
#include "SLO_Geometry.h"
#include "SLO_Misc.h"
#include <SDL_render.h>
#include <SDL_stdinc.h>
#include <SDL_blendmode.h>
#include <memory>


namespace SDL_
{

class Window;
class Image;
class Texture;
class Renderer final
{
public:
	UNCOPYABLE(Renderer);
	explicit Renderer(const Image &surface);
	Renderer(const Window &window, Uint32 flags);
	~Renderer();

	SDL_Renderer *get() const { return renderer_; }

	SDL_BlendMode getDrawBlendMode()
	{
		SDL_BlendMode blendMode;
		::SDL_GetRenderDrawBlendMode(get(), &blendMode);
		return blendMode;
	}
	const Color getDrawColor()
	{
		Uint8 r, g, b, a;
		::SDL_GetRenderDrawColor(get(), &r, &g, &b, &a);
		return Color(r, g, b, a);
	}
	//SDL_Texture* SDL_GetRenderTarget(SDL_Renderer* renderer);
	//int SDL_GetRendererInfo(SDL_Renderer* renderer, SDL_RendererInfo* info);
	const Size getOutputSize()
	{
		int w, h;
		::SDL_GetRendererOutputSize(get(),&w, &h);
		return Size(w, h);
	}
	void clear() { ::SDL_RenderClear(renderer_); }
	void copy(std::shared_ptr<Texture> texture, const Rect *srcrect, const Rect *dstrect);
	void copyEx(std::shared_ptr<Texture> texture, const Rect *srcrect, const Rect *dstrect, const double angle, const Point *center, const SDL_RendererFlip flip);
	void drawLine(int x1, int y1, int x2, int y2) { ::SDL_RenderDrawLine(get(), x1, y1, x2, y2); }
	//int SDL_RenderDrawLines(SDL_Renderer* renderer, const SDL_Point* points, int count);
	void drawPoint(int x, int y) { ::SDL_RenderDrawPoint(get(), x, y); }
	//int SDL_RenderDrawPoints(SDL_Renderer* renderer, const SDL_Point* points, int count);
	void drawRect(const Rect &rect) { ::SDL_RenderDrawRect(get(), &rect); }
	//int SDL_RenderDrawRects(SDL_Renderer* renderer, const SDL_Rect* rects, int count);
	void fillRect(const Rect &rect) { ::SDL_RenderFillRect(get(), &rect); }
	//int SDL_RenderFillRects(SDL_Renderer* renderer, const SDL_Rect* rects, int count);
	const Rect getClipRect()
	{
		Rect clip;
		::SDL_RenderGetClipRect(get(), &clip);
		return clip;
	}
	const Size getLogicalSize()
	{
		int w, h;
		::SDL_RenderGetLogicalSize(get(), &w, &h);
		return {w, h};
	}
	const Sizef getScale()
	{
		Sizef::type w, h;
		::SDL_RenderGetScale(get(), &w, &h);
		return {w, h};
	}
	const Rect getViewport()
	{
		Rect rect;
		::SDL_RenderGetViewport(get(), &rect);
		return rect;
	}
	bool isClipEnabled()
	{
		//return ::SDL_RenderIsClipEnabled(get());
		return clip_enable_;
	}
	void present() { ::SDL_RenderPresent(get()); }
	//int SDL_RenderReadPixels(SDL_Renderer* renderer, const SDL_Rect* rect, Uint32 format, void* pixels, int pitch);
	void setClipRect(const Rect &rect) {
		::SDL_RenderSetClipRect(get(), &rect);
		clip_enable_ = true;
	}
	void clearClipRect() {
		::SDL_RenderSetClipRect(get(), nullptr);
		clip_enable_ = false;
	}
	void setLogicalSize(int w, int h) { ::SDL_RenderSetLogicalSize(get(), w, h); }
	void setLogicalSize(const Size &size) { ::SDL_RenderSetLogicalSize(get(), size.get_width(), size.get_height()); }
	void setScale(float scaleX, float scaleY) { ::SDL_RenderSetScale(get(), scaleX, scaleY); }
	void setScale(const Sizef &scale) { ::SDL_RenderSetScale(get(), scale.getWidth(), scale.getHeight()); }
	void setViewport(const SDL_Rect &rect) { ::SDL_RenderSetViewport(get(), &rect); }
	bool isTargetSupported() { return ::SDL_RenderTargetSupported(get()); }
	void setDrawBlendMode(SDL_BlendMode blendMode) { ::SDL_SetRenderDrawBlendMode(get(), blendMode); }
	void setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) { ::SDL_SetRenderDrawColor(get(), r, g, b, a); }
	void setDrawColor(const Color &color) { ::SDL_SetRenderDrawColor(get(), color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha()); }
	void setTarget(std::shared_ptr<Texture> texture);

private:
	SDL_Renderer * const renderer_;
	bool clip_enable_;
};

} // SDL_

#endif // SDLRENDERER_H_
