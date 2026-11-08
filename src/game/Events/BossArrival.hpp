#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief Event published when the last wave of a level with a boss is over:
 * the number of the sheet of the boss (see EntityTypeNumber), and how long
 * the players hear the warning before it appears, in milliseconds.
 */
struct BossArrival {
  std::uint8_t typeNumber{};
  std::uint32_t warningMilliseconds{};

  bool operator==(const BossArrival&) const = default;
};

}  // namespace rtype::game
