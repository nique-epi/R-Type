#pragma once

#include <span>
#include <string>
#include "BarState.hpp"
#include "BookExit.hpp"
#include "FinalPilot.hpp"
#include "FlashPreference.hpp"
#include "FlightLogSchedule.hpp"
#include "MotionPreference.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief Everything one picture of the flight log is painted from: the time
 * since the screen opened and its schedule, the pilots best first, the lines
 * of the taunt, how the book leaves, the state of PLAY AGAIN and of QUIT,
 * the motion and flash preferences. */
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
};

}  // namespace rtype::client
