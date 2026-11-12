#pragma once

#include <array>
#include <cstddef>
#include "ActiveHurts.hpp"
#include "ComponentRegistry.hpp"
#include "EventBus.hpp"
#include "ISystem.hpp"
#include "LagCompensationConstants.hpp"
#include "PastTick.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

/**
 * @brief Tests the missiles of the players against the enemies where their
 * players saw them, and the ships of the players against the enemies and
 * their missiles where their players saw them: the lag compensation of the
 * shots, and of the hits on the ships.
 *
 * Each update first records the boxes of the present: the body of every
 * enemy, every missile of an enemy and every active box of a HurtZone,
 * keeping the present and the MAXIMUM_REWIND_TICKS ticks before it. Then
 * every entity that carries a ViewDelay is tested against the boxes of that
 * many ticks ago, at most MAXIMUM_REWIND_TICKS and at most as far back as was
 * recorded; the mode of play may give a shorter ViewDelay (see
 * IHitArbitration):
 * - a missile of a player, its box stretched back over the way it moved this
 *   tick so it never jumps over an enemy, against the bodies: the first it
 *   touches gives a MissileHit, the event CollisionRules publishes for a
 *   missile without ViewDelay;
 * - a ship of a player against every box: the first it touches gives a
 *   MissileHit from the missile of the enemy, or a ShipContact with the enemy
 *   of a body or a hurt box, the events CollisionRules and HurtZoneSystem
 *   publish for a ship without ViewDelay.
 * A box is touched only when the engine::CollisionFilter of the entity that
 * is tested and the one of the enemy, as it was recorded, pass
 * engine::mayCollide(), as for CollisionSystem; an entity without a filter is
 * neither recorded nor tested.
 * An entity destroyed since is never touched: a missile never kills what is
 * already dead, and a missile already gone never hits. Which ships get a
 * ViewDelay is the choice of the game (see IHitArbitration). Run it after
 * MovementSystem.
 *
 * The boxes kept belong to one world: given the registry of another one, as
 * when a new game builds its world, it forgets them, since an entity of the
 * old world may have the handle of an entity of the new one. A new world
 * never has the address of the one before it: WorldSlot builds it before it
 * frees the old one.
 */
class LagCompensatedHitSystem final : public engine::ISystem {
 public:
  explicit LagCompensatedHitSystem(engine::EventBus& events);

  void update(engine::ComponentRegistry& components,
              engine::Duration elapsed) override;

 private:
  /** @brief Tests every missile of a player that carries a ViewDelay against
   * the bodies of the enemies its player saw, @p elapsed being the tick. */
  void testPlayerMissiles(engine::ComponentRegistry& components,
                          engine::Duration elapsed);

  /** @brief Tests every ship that carries a ViewDelay against the boxes and
   * the beams its player saw. */
  void testShips(engine::ComponentRegistry& components);

  /** @brief Records the boxes and the beams of the enemies of @p components
   * as the newest tick, in place of the oldest one kept; an enemy still
   * summoning is left out, as CollisionSystem and HurtZoneSystem leave it
   * out. */
  void recordEnemies(engine::ComponentRegistry& components);

  /** @brief Forgets every box kept, and keeps @p components as the world they
   * come from. */
  void startWith(const engine::ComponentRegistry& components);

  /** @returns The enemies recorded @p ticksAgo ticks before the newest one,
   * fewer when fewer ticks were recorded: at most KEPT_ENEMY_TICKS - 1, which
   * is MAXIMUM_REWIND_TICKS. */
  [[nodiscard]] const PastTick& enemiesAgo(std::size_t ticksAgo) const;

  engine::EventBus* events_;
  std::array<PastTick, KEPT_ENEMY_TICKS> history_;
  std::size_t newest_{0};
  std::size_t recorded_{0};
  const engine::ComponentRegistry* world_{nullptr};
  ActiveHurts hurts_;
};

}  // namespace rtype::game
