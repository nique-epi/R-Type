#pragma once

#include "PlayfieldConstants.hpp"

namespace rtype::server {

/**
 * @brief The largest playfield size for which the range of a wire coordinate
 * (-4096 to 4095.875 units) holds the playfield and one playfield size beyond
 * each edge, see the Position section of the protocol page.
 */
constexpr float LARGEST_PLAYFIELD_SIZE_ON_THE_WIRE = 2047.0F;

static_assert(game::PLAYFIELD_WIDTH <= LARGEST_PLAYFIELD_SIZE_ON_THE_WIRE &&
                  game::PLAYFIELD_HEIGHT <= LARGEST_PLAYFIELD_SIZE_ON_THE_WIRE,
              "a larger playfield no longer fits a wire coordinate");

}  // namespace rtype::server
