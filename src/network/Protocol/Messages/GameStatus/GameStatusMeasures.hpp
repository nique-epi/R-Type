#pragma once

#include <cstdint>
#include <optional>

namespace rtype::network {

/**
 * @brief What a game process measured of itself, as its GameStatus tells its
 * supervisor, each in whole microseconds. A measure the process has not taken
 * is empty, and left out of the payload: it is never sent as 0.
 */
struct GameStatusMeasures {
  /** @brief The median and the 95th percentile of the duration of a tick. */
  std::optional<std::uint32_t> tickMedianMicroseconds;
  std::optional<std::uint32_t> tickNinetyFifthPercentileMicroseconds;
  /** @brief The mean round trip to the players of the game. */
  std::optional<std::uint32_t> averageRoundTripMicroseconds;

  bool operator==(const GameStatusMeasures&) const = default;
};

}  // namespace rtype::network
