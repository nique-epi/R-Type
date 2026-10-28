#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace rtype::client {

class ScreenStack;
class ScrollingBackground;

/** @brief Draws one frame on @p target, in playfield units: @p sky, then the
 * screens of @p screens over it, from the bottom up. The frame loop draws the
 * window with it. */
void drawFrame(sf::RenderTarget& target, ScrollingBackground& sky,
               const ScreenStack& screens);

/**
 * @returns The picture the window would show now, the size of the playfield
 * in pixels, as drawFrame() draws it.
 * @throws RenderTextureNotCreatedException when no render texture of that
 * size can be made.
 */
[[nodiscard]] sf::Texture framePictureOf(ScrollingBackground& sky,
                                         const ScreenStack& screens);

}  // namespace rtype::client
