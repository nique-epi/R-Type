#pragma once

#include <cstdint>
#include <functional>
#include "PlayerPalette.hpp"
#include "SpriteFrame.hpp"

namespace rtype::client {

/**
 * @brief A frame of the ship of a pilot color, 0 to PLAYER_COLOR_COUNT - 1,
 * painted in the shades of a palette.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct PilotShipPicture {
  game::SpriteFrame frame;
  int color{};
  /** @brief The hull the pilot flies: NO_SHIP_LOOK_ITEM for the standard
   * ship, else the number of a hull of the catalog. */
  std::uint8_t hull{};
  std::reference_wrapper<const PlayerPalette> palette;
};

}  // namespace rtype::client
