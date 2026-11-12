#pragma once

#include <cstdint>

namespace rtype::server {

/** @brief How the inputs of a player kept pace with the ticks of its game,
 * since its first input was applied, in totals that only grow. */
struct InputPace {
  /** @brief Ticks that found no input of the player due: it arrived later
   * than the input delay, or was lost and not repeated yet, and the ship kept
   * the input before. */
  std::uint64_t ticksWithoutInput{0};
  /** @brief Ticks that applied two inputs of the player at once, to catch up
   * with inputs that waited more than INPUT_CATCH_UP_TICKS. */
  std::uint64_t catchUps{0};

  bool operator==(const InputPace&) const = default;
};

}  // namespace rtype::server
