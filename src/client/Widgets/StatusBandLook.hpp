#pragma once

#include <functional>
#include "FlashPreference.hpp"
#include "PlayerPalette.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief How the status band is painted: in the shades of the palette the
 * player chose, at a moment of the game, which runs the names too long for
 * their bay and the parts of a bay at a larger text size, and still or not
 * as REDUCE FLASHES asks. */
struct StatusBandLook {
  std::reference_wrapper<const PlayerPalette> palette;
  engine::Duration sinceStart{};
  FlashPreference flashes = FlashPreference::Full;
};

}  // namespace rtype::client
