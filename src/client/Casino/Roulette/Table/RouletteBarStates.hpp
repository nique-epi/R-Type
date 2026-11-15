#pragma once

namespace rtype::client {

/** @brief Which bars of the table can act, and whether SPIN shows the vote
 * of the player as pressed. */
struct RouletteBarStates {
  bool spinOpen{};
  bool spinAsked{};
  bool clearOpen{};
  bool rebetOpen{};

  bool operator==(const RouletteBarStates&) const = default;
};

}  // namespace rtype::client
