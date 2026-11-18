#pragma once

namespace rtype::client {

/** @brief Which bars of the table can act. */
struct RouletteBarStates {
  bool clearOpen{};
  bool rebetOpen{};

  bool operator==(const RouletteBarStates&) const = default;
};

}  // namespace rtype::client
