#pragma once

#include <cstdint>
#include <optional>
#include "CasinoRoomState.hpp"

namespace rtype::client {

/** @brief What the master answered to an ask for the state of a room, its
 * serverTime the clock of the master as it answered: nothing when it did not
 * answer, refused, or answered what the client cannot read. */
struct CasinoRoomReading {
  /** @brief The number of the ask it answers (see CasinoAsks). */
  std::uint64_t ask{};
  /** @brief The room it asked for. */
  std::uint8_t room{};
  std::optional<network::CasinoRoomState> state;
};

}  // namespace rtype::client
