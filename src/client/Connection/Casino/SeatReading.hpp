#pragma once

#include <cstdint>
#include <optional>
#include "CasinoRefusal.hpp"
#include "CasinoSeat.hpp"

namespace rtype::client {

/** @brief What the master answered to a seat taken, renewed or left: no
 * refusal once it made it. */
struct SeatReading {
  /** @brief The number of the ask it answers (see CasinoAsks). */
  std::uint64_t ask{};
  CasinoSeat seat;
  std::optional<CasinoRefusal> refusal;
};

}  // namespace rtype::client
