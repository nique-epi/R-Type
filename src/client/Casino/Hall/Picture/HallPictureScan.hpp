#pragma once

#include <SFML/Graphics/Image.hpp>
#include <vector>
#include "HallPicture.hpp"
#include "Spectator.hpp"

namespace rtype::client {

/** @returns What the hall reads in @p picture, the picture of the hall,
 * HALL_PICTURE_ID: its bulbs, crystals, front ropes and spectators. */
[[nodiscard]] HallPicture hallPictureOf(const sf::Image& picture);

/** @returns The spectators of the crowd of @p picture, from the left, each
 * the stretch of the crowd between two dips of the line of its heads. */
[[nodiscard]] std::vector<Spectator> spectatorsOf(const sf::Image& picture);

}  // namespace rtype::client
