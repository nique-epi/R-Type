#pragma once

#include <cstdint>

namespace rtype::game {

/** @brief How far a boss is: Created as it appears, Warned once the players
 * were warned of it, for as long as its warning lasts, Arriving while its
 * behaviour brings it in, then Fighting. */
enum class BossProgress : std::uint8_t {
  Created,
  Warned,
  Arriving,
  Fighting,
};

}  // namespace rtype::game
