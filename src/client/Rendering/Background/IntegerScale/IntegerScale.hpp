#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/**
 * @brief Largest whole number of times the background fits in the playfield
 * as shown on screen.
 *
 * The background is first enlarged by this factor without smoothing, so its
 * pixels stay square and equal; only what is left to fill the playfield is
 * smoothed.
 *
 * @param playfieldPixels Size of the playfield on the render target, in
 * pixels.
 * @returns The factor, at least 1: a playfield smaller than the background, or
 * of no size, is still drawn.
 */
[[nodiscard]] unsigned int integerScaleFactor(sf::Vector2i playfieldPixels);

/**
 * @returns How much the texture of a canvas of @p density is enlarged so its
 * pixels fill a picture enlarged @p factor times: factor / density. The
 * factor comes from the background grid, never from the texture, so a denser
 * canvas is shown at the same size and as sharp.
 */
[[nodiscard]] float sharpScaleOf(unsigned int factor, unsigned int density);

}  // namespace rtype::client
