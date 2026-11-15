#pragma once

#include <functional>
#include <vector>
#include "BossHealthChanged.hpp"
#include "Entity.hpp"
#include "ISystem.hpp"
#include "SubscriptionHandle.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;
class EventBus;

}  // namespace rtype::engine

namespace rtype::game {

struct EntityDestruction;

/**
 * @brief Moves every Boss from one phase of its fight to the next, each
 * once, and tells the players the health of a boss in its fight with a
 * BossHealthChanged, whenever what they were last told of it changed:
 *
 * - its fight starts once its arrival is over (it ended an action and is no
 *   longer in its arrival attack): told with its full health;
 * - a hit took points from it: told at the tick after the hit;
 * - it enrages once its health is below its rage threshold: given its Rage,
 *   its rage attack started in the phase Enraged (startEnemyAction()
 *   interrupts whatever it was doing and announces it), then told enraged;
 * - it is killed: told with no point left, as its EntityDestruction Killed is
 *   delivered. A boss removed any other way is told of no more.
 *
 * Subscribes to EntityDestruction on construction and unsubscribes on
 * destruction. Run it before PilotSystem, so the behaviour sees the rage at
 * the same tick. The bus must outlive the system.
 */
class BossSystem final : public engine::ISystem {
 public:
  explicit BossSystem(engine::EventBus& events);
  BossSystem(const BossSystem&) = delete;
  BossSystem& operator=(const BossSystem&) = delete;
  BossSystem(BossSystem&&) = delete;
  BossSystem& operator=(BossSystem&&) = delete;
  ~BossSystem() override;

  void update(engine::ComponentRegistry& components,
              engine::Duration elapsed) override;

 private:
  /** @brief Gives @p boss its Rage and starts its rage attack. Never called
   * while the components are visited. */
  void enrage(engine::ComponentRegistry& components, engine::Entity boss);

  /** @brief Tells the players the health of every boss in its fight whose
   * health or rage changed since they were last told. */
  void tellChangedHealths(engine::ComponentRegistry& components);

  /** @brief Tells the players a boss in its fight that was killed has no
   * point left, and forgets any boss that is gone. */
  void onDestruction(const EntityDestruction& destruction);

  std::reference_wrapper<engine::EventBus> events_;
  engine::SubscriptionHandle destructions_;
  std::vector<engine::Entity> enraging_;
  /** What the players were last told of each boss in its fight. */
  std::vector<BossHealthChanged> told_;
  std::vector<BossHealthChanged> healths_;
};

}  // namespace rtype::game
