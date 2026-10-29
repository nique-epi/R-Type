#pragma once

#include <functional>
#include <span>
#include <string>
#include "BarState.hpp"
#include "BookExit.hpp"
#include "FinalPilot.hpp"
#include "FlashPreference.hpp"
#include "FlightLogSchedule.hpp"
#include "MotionPreference.hpp"
#include "PlayerPalette.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief Everything one picture of the flight log is painted from: the time
 * since the screen opened and its schedule, the pilots best first, the lines
 * of the taunt, how the book leaves, the state of PLAY AGAIN and of QUIT,
 * the motion and flash preferences, and the palette the ships and markers of
 * the pilots are painted in.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member without
 * a default: the member-init checks, which report its implicit default
 * constructor, see a constructor that cannot be called. */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct FlightLogLook {
  engine::Duration sinceStart{};
  FlightLogSchedule schedule;
  std::span<const FinalPilot> pilots;
  std::span<const std::string> taunt;
  BookExit exit;
  BarState replay{};
  BarState quit{};
  MotionPreference motion{};
  FlashPreference flashes{};
  std::reference_wrapper<const PlayerPalette> palette;
};

}  // namespace rtype::client
