#pragma once

#include <cstdint>

namespace rtype::network {

/** @brief A round of the wheel of one room of the casino, as the path of a
 * route of the bets names it. */
struct RoomRound {
  std::uint8_t room{};
  std::uint64_t round{};

  bool operator==(const RoomRound&) const = default;
};

}  // namespace rtype::network
