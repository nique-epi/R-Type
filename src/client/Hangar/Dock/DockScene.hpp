#pragma once

#include <functional>
#include <optional>
#include "FittingRun.hpp"
#include "MotionPreference.hpp"
#include "PlayerPalette.hpp"
#include "ShipLook.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief What the bay shows in a frame: the palette and the color of the
 * ship, what the settings say it wears, the fitting under way if any, the
 * clock of the screen, and whether things may move.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct DockScene {
  std::reference_wrapper<const PlayerPalette> palette;
  int shipColor{};
  ShipLook worn;
  std::optional<FittingRun> fitting;
  engine::Duration elapsed{};
  MotionPreference motion{};
};

}  // namespace rtype::client
