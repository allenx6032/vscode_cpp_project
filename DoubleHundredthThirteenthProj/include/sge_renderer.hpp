#ifndef __SGE_RENDERER_HPP
#define __SGE_RENDERER_HPP

#include <sge_image.hpp>
#include <sge_font.hpp>
#include <sge_vector.hpp>
#include <sge_shape.hpp>

#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <functional>

namespace sge
{
    using DrawRequest = std::function<bool (void)>;

    class Renderer
    {
        public:
            void set_renderer(SDL_Renderer *rdr);

            bool clear();
            void present();

            bool draw_point(const Vector &pos, const SDL_Color &color);
            bool draw_line(const Vector &start, const Vector &end, const SDL_Color &color);

            bool draw_rect(const SDL_Rect &r, const SDL_Color &color);
            bool draw_rect(const SDL_Rect &r, float angle, const SDL_Color &color);
            
            bool draw_filled_rect(const SDL_Rect &r, const SDL_Color &color);
            bool draw_filled_rect(const SDL_Rect &r, float angle, const SDL_Color &color);

            bool draw_circle(const Vector &center, int radius, const SDL_Color &color);
            bool draw_filled_circle(const Vector &center, int radius, const SDL_Color &color);

            bool draw_ellipse(const Vector &center, int hradius, int vradius, const SDL_Color &color);
            bool draw_ellipse(const Vector &center, int hradius, int vradius, float angle, const SDL_Color &color);
            bool draw_ellipse(const Vector &center, int hradius, int vradius, SDL_RendererFlip flip, const SDL_Color &color);
            bool draw_ellipse(const Vector &center, int hradius, int vradius, float angle, SDL_RendererFlip flip, const SDL_Color &color);

            bool draw_filled_ellipse(const Vector &center, int hradius, int vradius, const SDL_Color &color);
            bool draw_filled_ellipse(const Vector &center, int hradius, int vradius, float angle, const SDL_Color &color);
            bool draw_filled_ellipse(const Vector &center, int hradius, int vradius, SDL_RendererFlip flip, const SDL_Color &color);
            bool draw_filled_ellipse(const Vector &center, int hradius, int vradius, float angle, SDL_RendererFlip flip, const SDL_Color &color);

            bool draw_shape(const Shape &shape, const SDL_Color &color);
            bool draw_filled_shape(const Shape &shape, const SDL_Color &color);

            bool draw_image(std::shared_ptr<Image> asset, const SDL_Rect &dest);
            bool draw_image(std::shared_ptr<Image> asset, const SDL_Rect &src, const SDL_Rect &dest);
            bool draw_image(std::shared_ptr<Image> asset, const SDL_Rect &dest, float angle, const SDL_Point &center, SDL_RendererFlip flip);
            bool draw_image(std::shared_ptr<Image> asset, const SDL_Rect &src, const SDL_Rect &dest, float angle, const SDL_Point &center, SDL_RendererFlip flip);

            bool draw_text(std::shared_ptr<Font> asset, const std::string &text, const Vector &pos, const SDL_Color &color);

            std::string get_error() const;

        private:
            bool draw_with_color(DrawRequest req, const SDL_Color &color);
            bool draw_with_target(DrawRequest req, float angle, const SDL_Point &center, SDL_RendererFlip flip);
            void set_error(const std::string &text);

        private:
            SDL_Renderer *renderer;
            std::string error;
    };
}

#endif /* __SGE_RENDERER_HPP */