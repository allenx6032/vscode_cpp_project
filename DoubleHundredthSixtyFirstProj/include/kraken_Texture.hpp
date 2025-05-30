#pragma once

#include <SDL.h>
#include <string>

#include "Kraken_Math.hpp"
#include "Kraken_Rect.hpp"

namespace kn
{
class Surface;
struct Color;

class Texture
{
  public:
    /**
     * @brief The rotation angle of the texture, in degrees.
     */
    double angle = 0.0;

    struct Flip
    {
        /**
         * @brief Whether to flip the texture horizontally.
         */
        bool h = false;
        /**
         * @brief Whether to flip the texture vertically.
         */
        bool v = false;
    } flip;

    /**
     * @brief Create a texture from an existing SDL texture pointer.
     *
     * @param sdlTexture An SDL texture object.
     */
    explicit Texture(SDL_Texture* sdlTexture);

    /**
     * @brief Construct a new Texture object from a Surface object.
     * 
     * @param surface The surface to load the texture from.
     */
    explicit Texture(const Surface& surface);

    /**
     * @brief Loads an image from disk.
     *
     * @param filePath Path to the image file.
     */
    explicit Texture(const std::string& filePath);

    /**
     * @brief Create a solid color texture.
     *
     * @param size The size of the texture.
     * @param color The color of the texture.
     */
    Texture(const math::Vec2& size, const Color& color);

    /**
     * @brief Load a texture from an array of pixel data.
     *
     * @param pixelData The pixel data.
     * @param size The size of the texture.
     * @param depth The depth of the pixel data.
     */
    Texture(const void* pixelData, const math::Vec2& size, int depth = 32);

    Texture() = default;
    ~Texture();

    /**
     * @brief Loads an image from disk.
     *
     * @param filePath Path to the image file.
     *
     * @return ``true`` when successful, ``false`` on failure.
     */
    [[maybe_unused]] bool loadFromFile(const std::string& filePath);

    /**
     * @brief Create a solid color texture.
     *
     * @param size The size of the texture.
     * @param color The color of the texture.
     */
    [[maybe_unused]] bool create(const math::Vec2& size, const Color& color);

    /**
     * @brief Load a texture from an array of pixel data.
     *
     * @param pixelData The pixel data.
     * @param size The size of the texture.
     * @param depth The depth of the pixel data.
     *
     * @return ``true`` when successful, ``false`` on failure.
     */
    [[maybe_unused]] bool loadFromArray(const void* pixelData, const math::Vec2& size,
                                        int depth = 32);

    /**
     * @brief Load a texture from a Surface object.
     * 
     * @param surface The surface to load the texture from.
     * @return ``true`` when successful, ``false`` on failure.
     */
    [[maybe_unused]] bool loadFromSurface(const Surface& surface);

    /**
     * @brief Load a texture from an SDL texture pointer.
     *
     * @param sdlTexture An SDL texture object.
     *
     * @return ``true`` when successful, ``false`` on failure.
     */
    [[maybe_unused]] bool loadFromSDL(SDL_Texture* sdlTexture);

    /**
     * @brief Get the size of the texture.
     *
     * @return The size of the texture.
     */
    [[nodiscard]] math::Vec2 getSize() const;

    /**
     * @brief Get the rect of the texture.
     *
     * @return The rect of the texture.
     */
    [[nodiscard]] Rect getRect() const;

    /**
     * @brief Set the color mod (tint) for the texture.
     * 
     * @param tint The color to multiply the texture color pixels by during rendering.
     */
    void setTint(const Color& tint) const;

    /**
     * @brief Get the color mod (tint) for the texture.
     * 
     * @return The color used to multiply the texture color pixels during rendering. 
     */
    Color getTint() const;

    /**
     * @brief Set the alpha mod for the texture.
     *
     * @param alpha The value to multiply texture pixel alpha values by during rendering.
     */
    void setAlpha(uint8_t alpha) const;

    /**
     * @brief Get the alpha mod for the texture.
     *
     * @return The value used to multiply texture pixel alpha values during rendering.
     */
    uint8_t getAlpha() const;

    /**
     * @brief Set the texture blend mode to additive blending.
     */
    void makeAdditive() const;

    /**
     * @brief Set the texture blend mode to multiplicative blending.
     */
    void makeMultiply() const;

    /**
     * @brief Set the texture blend mode to normal (blend) blending.
     */
    void makeNormal() const;

    /**
     * @brief Get the SDL texture pointer.
     *
     * @return The SDL texture pointer.
     */
    [[nodiscard]] SDL_Texture* getSDL() const;

  protected:
    SDL_Texture* texture = nullptr;
};
} // namespace kn
