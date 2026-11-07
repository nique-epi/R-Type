#pragma once

#include <array>
#include <cstddef>
#include <vector>
#include "ComponentRegistry.hpp"
#include "EventBus.hpp"
#include "ISystem.hpp"
#include "LagCompensationConstants.hpp"
#include "PastEnemy.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

/**
 * @brief Tests the missiles of the players against the enemies where their
 * players saw them, the lag compensation of the shots.
 *
 * Each update first records the box of every enemy, keeping the present and
 * the MAXIMUM_REWIND_TICKS ticks before it. Then every missile that carries a
 * ViewDelay is tested against the enemies of that many ticks ago, at most
 * MAXIMUM_REWIND_TICKS and at most as far back as was recorded: its box,
 * stretched back over the way it moved this tick so it never jumps over an
 * enemy, against their boxes then. An enemy destroyed since is not hit: a
 * missile never kills what is already dead. The first enemy it touches gives
 * a MissileHit, the same event CollisionRules publishes for a missile without
 * ViewDelay. Run it after MovementSystem.
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
  /** @brief Records the box of every enemy of @p components as the newest
   * tick, in place of the oldest one kept; an enemy still summoning is left
   * out, as CollisionSystem leaves it out. */
  void recordEnemies(engine::ComponentRegistry& components);

  /** @brief Forgets every box kept, and keeps @p components as the world they
   * come from. */
  void startWith(const engine::ComponentRegistry& components);

  /** @returns The enemies recorded @p ticksAgo ticks before the newest one,
   * fewer when fewer ticks were recorded: at most KEPT_ENEMY_TICKS - 1, which
   * is MAXIMUM_REWIND_TICKS. */
  [[nodiscard]] const std::vector<PastEnemy>& enemiesAgo(
      std::size_t ticksAgo) const;

  engine::EventBus* events_;
  std::array<std::vector<PastEnemy>, KEPT_ENEMY_TICKS> history_;
  std::size_t newest_{0};
  std::size_t recorded_{0};
  const engine::ComponentRegistry* world_{nullptr};
};

}  // namespace rtype::game
