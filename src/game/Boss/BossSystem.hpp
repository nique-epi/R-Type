#pragma once

#include <vector>
#include "BossHealthChanged.hpp"
#include "BossSystemParts.hpp"
#include "Entity.hpp"
#include "ISystem.hpp"
#include "SubscriptionHandle.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::game {

struct EntityDestruction;
struct Health;

/**
 * @brief Everything that makes a boss more than an enemy: it moves every
 * Boss on through its BossProgress, each step once, and tells the players
 * about it. A boss is created as any body is; this system alone reads Boss.
 *
 * - Created: the players are warned with a BossArrival, and the boss gets a
 *   Summoning of BOSS_ARRIVAL_WARNING_TICKS ticks, which keeps it harmless
 *   and unsteered, where it appeared, while the warning lasts
 *   (SummoningSystem counts it down);
 * - Warned: once nothing summons it any more, its health is scaled for the
 *   players and the difficulty (see scaledBossHealth()), and its behaviour
 *   brings it in;
 * - Arriving: its fight starts once its arrival is over (it ended an action
 *   and is no longer in its arrival attack);
 * - Fighting: it enrages once its health is below its rage threshold: given
 *   its Rage, its rage attack started in the phase Enraged
 *   (startEnemyAction() interrupts whatever it was doing and announces it).
 *
 * The players are told the health of a boss in its fight with a
 * BossHealthChanged whenever what they were last told of it changed: as the
 * fight starts, at the tick after a hit, as it enrages, and with no point
 * left as its EntityDestruction Killed is delivered. A boss removed any other
 * way is told of no more.
 *
 * Subscribes to EntityDestruction on construction and unsubscribes on
 * destruction. Run it before PilotSystem, so the behaviour sees the rage at
 * the same tick and never steers a boss before its warning.
 */
class BossSystem final : public engine::ISystem {
 public:
  explicit BossSystem(BossSystemParts parts);
  BossSystem(const BossSystem&) = delete;
  BossSystem& operator=(const BossSystem&) = delete;
  BossSystem(BossSystem&&) = delete;
  BossSystem& operator=(BossSystem&&) = delete;
  ~BossSystem() override;

  void update(engine::ComponentRegistry& components,
              engine::Duration elapsed) override;

 private:
  /** @brief Warns the players of @p boss and keeps it harmless for the
   * warning. Never called while the components are visited. */
  void warn(engine::ComponentRegistry& components, engine::Entity boss) const;

  /** @brief Scales @p health, the health of a boss whose warning just ended,
   * for the players and the difficulty. */
  void strengthen(Health& health) const;

  /** @brief Gives @p boss its Rage and starts its rage attack. Never called
   * while the components are visited. */
  void enrage(engine::ComponentRegistry& components, engine::Entity boss) const;

  /** @brief Tells the players the health of every boss in its fight whose
   * health or rage changed since they were last told. */
  void tellChangedHealths(engine::ComponentRegistry& components);

  /** @brief Tells the players a boss in its fight that was killed has no
   * point left, and forgets any boss that is gone. */
  void onDestruction(const EntityDestruction& destruction);

  BossSystemParts parts_;
  engine::SubscriptionHandle destructions_;
  std::vector<engine::Entity> warned_;
  std::vector<engine::Entity> enraging_;
  /** What the players were last told of each boss in its fight. */
  std::vector<BossHealthChanged> told_;
  std::vector<BossHealthChanged> healths_;
};

}  // namespace rtype::game
