#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief Event published once a boss appeared, for the players to be warned
 * of it: the number of its sheet (see EntityTypeNumber), and how long the
 * warning lasts before its behaviour brings it in, in milliseconds.
 */
struct BossArrival {
  std::uint8_t typeNumber{};
  std::uint32_t warningMilliseconds{};

  bool operator==(const BossArrival&) const = default;
};

}  // namespace rtype::game
