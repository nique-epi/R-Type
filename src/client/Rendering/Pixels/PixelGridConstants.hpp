#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include "PlayfieldConstants.hpp"

namespace rtype::client {

/**
 * @brief Size of one background pixel, in logical units of the playfield.
 *
 * The background is painted at a quarter of the playfield resolution, then
 * enlarged over it: see EnlargedPicture for how its pixels stay sharp.
 */
constexpr float BACKGROUND_PIXEL_SIZE = 4.0F;

/** @brief Size of the background, in background pixels. */
constexpr float BACKGROUND_WIDTH =
    game::PLAYFIELD_WIDTH / BACKGROUND_PIXEL_SIZE;
constexpr float BACKGROUND_HEIGHT =
    game::PLAYFIELD_HEIGHT / BACKGROUND_PIXEL_SIZE;
constexpr sf::Vector2u BACKGROUND_SIZE(
    static_cast<unsigned int>(BACKGROUND_WIDTH),
    static_cast<unsigned int>(BACKGROUND_HEIGHT));

/**
 * @brief Offset from the top-left corner of a pixel to its center: a point
 * drawn there lights exactly that pixel.
 */
constexpr sf::Vector2f PIXEL_CENTER(0.5F, 0.5F);

/** @brief Offsets to the pixels around a pixel. */
constexpr sf::Vector2i PIXEL_ABOVE(0, -1);
constexpr sf::Vector2i PIXEL_BELOW(0, 1);
constexpr std::array CROSS_DIRECTIONS{sf::Vector2i(-1, 0), sf::Vector2i(1, 0),
                                      PIXEL_ABOVE, PIXEL_BELOW};
constexpr std::array DIAGONAL_DIRECTIONS{
    sf::Vector2i(1, 1), sf::Vector2i(-1, -1), sf::Vector2i(1, -1),
    sf::Vector2i(-1, 1)};

}  // namespace rtype::client
