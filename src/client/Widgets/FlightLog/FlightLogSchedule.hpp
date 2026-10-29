#pragma once

#include <cstddef>
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief The moments of one flight log, from the start of the screen: the
 * cover starts to turn, the book lies open, the stamp lands, the first row is
 * written, the call to play again starts, the choices take input; and its
 * rows. */
struct FlightLogSchedule {
  engine::Duration openStart{};
  engine::Duration opened{};
  engine::Duration stamped{};
  engine::Duration rowsStart{};
  engine::Duration promptStart{};
  engine::Duration ready{};
  std::size_t rowCount{};
};

}  // namespace rtype::client
