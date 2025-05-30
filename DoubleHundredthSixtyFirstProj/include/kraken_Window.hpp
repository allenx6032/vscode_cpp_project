#pragma once

#include <string>

#include <SDL.h>

#include "Kraken_Color.hpp"
#include "Kraken_Constants.hpp"
#include "Kraken_Math.hpp"
#include "Kraken_Rect.hpp"

namespace kn
{

class Texture;

/**
 * @brief A global camera position.
 */
inline math::Vec2 camera;

namespace window
{
/**
 * @brief Initialize the window.
 *
 * @param resolution The size of the window.
 * @param title The title of the window.
 * @param scaled Whether to scale the window to fit the largest resolution.
 *
 * @return ``true`` if the window was created successfully, ``false`` otherwise.
 */
[[maybe_unused]] bool init(const math::Vec2& resolution, const std::string& title = "Kraken Window",
                           bool scaled = false);

/**
 * @brief Get whether the window is open.
 *
 * @return ``true`` if the window is open, ``false`` otherwise.
 */
[[nodiscard]] bool isOpen();

/**
 * @brief Close the window.
 */
void close();

/**
 * @brief Clear the screen.
 *
 * @param color The color to clear the screen.
 */
void clear(const Color& color = {0, 0, 0, 255});

/**
 * @brief Flip the render frame buffer.
 */
void flip();

/**
 * @brief Draw a texture using rects.
 *
 * @param texture The texture to draw.
 * @param dstRect The rectangle to draw to.
 * @param srcRect The rectangle to draw from.
 */
void blit(const Texture& texture, const Rect& dstRect, const Rect& srcRect = {});

/**
 *  @brief Draw a texture to a position.
 *
 *  @param texture The texture to draw.
 *  @param position The position to draw at.
 *  @param anchor The anchor point to draw from.
 */
void blit(const Texture& texture, const math::Vec2& position = {}, Anchor anchor = TOP_LEFT);

/**
 *  @brief Get the window renderer.
 *
 *  @return The window renderer.
 */
[[nodiscard]] SDL_Renderer* getRenderer();

/**
 *  @brief Populate the event object with the concurrent user events.
 *
 *  @param event The event object to populate.
 *
 *  @return
 */
int pollEvent(Event& event);

/**
 * @brief Get whether the window is fullscreen or not.
 *
 * @return ``true`` if the window is fullscreen, ``false`` otherwise.
 */
[[nodiscard]] bool getFullscreen();

/**
 * @brief Get the scale of the window.
 *
 * @return The scale of the window.
 */
[[nodiscard]] int getScale();

/**
 * @brief Get the size of the screen.
 *
 * @return The size of the screen.
 */
[[nodiscard]] math::Vec2 getSize();

/**
 * @brief Set the title of the window.
 *
 * @param newTitle The new title for the window.
 */
void setTitle(const std::string& newTitle);

/**
 * @brief Get the title of the window.
 *
 * @return The title of the window.
 */
[[nodiscard]] std::string getTitle();

/**
 * @brief Set whether the window is fullscreen or not.
 *
 * @param fullscreen ``true`` if setting fullscreen, ``false`` otherwise.
 */
void setFullscreen(bool fullscreen);

/**
 * @brief Quit SDL and destroy the window.
 */
void quit();

/**
 * @brief Set the window icon.
 *
 * @param path The path to the icon image file.
 */
void setIcon(const std::string& path);

/**
 * @brief Save a screenshot of the current frame to a file.
 *
 * @param path The path to save the screenshot to.
 * @return ``true`` if the screenshot was saved successfully, ``false`` otherwise.
 */
bool saveScreenshot(const std::string& filePath);

} // namespace window
} // namespace kn
