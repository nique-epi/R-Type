#pragma once

namespace rtype::game {

/**
 * @brief The health points a missile takes from the body it hits, as its
 * sheet names them in `[gameplay.damage]`; a missile without it takes
 * MISSILE_DAMAGE.
 */
struct MissileDamage {
  int points{};
};

}  // namespace rtype::game
