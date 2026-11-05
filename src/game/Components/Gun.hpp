#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::game {

/**
 * @brief Lets a body fire projectiles of its own, one every shot interval,
 * while its Intent wants to shoot.
 *
 * The ship of a player and an enemy carry one, and one system fires them all.
 * It works on a body that also has an Intent, and a firing body needs a
 * Position, a CollisionBox, an EntityType and a NetworkIdentity too. At each
 * tick of a game in progress, the server counts one tick off ticksUntilReady
 * while it is above 0. A body whose Intent wants to shoot and whose count is
 * 0 fires and starts the count again from intervalTicks. The count goes down
 * whether the body wants to shoot or not, so releasing shoot and pressing it
 * again fires no sooner.
 */
struct Gun {
  /** @brief Simulation ticks between two shots. */
  std::size_t intervalTicks{0};
  /** @brief Simulation ticks left before the gun may fire: 0 for a gun that
   * fires at once, intervalTicks for one that waits a whole interval first. */
  std::size_t ticksUntilReady{0};
  /** @brief The number of the sheet of what the gun fires, as
   * EntitySheets::numbered() finds it. */
  std::uint8_t projectileNumber{0};
};

}  // namespace rtype::game
