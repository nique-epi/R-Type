#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/** @returns The threshold of the dither at @p pixel, BAYER_CELL_CENTRE /
 * BAYER_LEVELS to (BAYER_LEVELS - BAYER_CELL_CENTRE) / BAYER_LEVELS. */
[[nodiscard]] double bayerThresholdAt(sf::Vector2i pixel);

}  // namespace rtype::client
