#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include "StripFrame.hpp"

namespace rtype::client {

/** @returns The rectangle of the frame @p frame in its image, @p imageSize
 * pixels wide and high: the frames share its width equally, side by side,
 * and each is as high as it. An empty rectangle for a strip of no frame. */
[[nodiscard]] sf::FloatRect stripFrameRect(sf::Vector2u imageSize,
                                           const StripFrame& frame);

}  // namespace rtype::client
