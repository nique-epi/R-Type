#pragma once

#include <functional>
#include "ISystem.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;
class EventBus;

}  // namespace rtype::engine

namespace rtype::game {

/**
 * @brief Moves every Boss from one phase of its fight to the next, each
 * once, and tells the players with a BossHealthChanged:
 *
 * - its fight starts once its arrival is over (it ended an action and is no
 *   longer in its arrival attack): announced with its full health;
 * - it enrages once its health is below its rage threshold: marked enraged,
 *   its rage attack started in the phase Enraged (startEnemyAction()
 *   interrupts whatever it was doing and announces it), then announced.
 *
 * Run it before PilotSystem, so the behaviour sees the rage at the same tick.
 * The bus must outlive the system.
 */
class BossPhaseSystem final : public engine::ISystem {
 public:
  explicit BossPhaseSystem(engine::EventBus& events);

  void update(engine::ComponentRegistry& components,
              engine::Duration elapsed) override;

 private:
  std::reference_wrapper<engine::EventBus> events_;
};

}  // namespace rtype::game
