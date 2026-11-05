#pragma once

#include <functional>
#include <vector>
#include "Entity.hpp"
#include "ISystem.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;
class EventBus;

}  // namespace rtype::engine

namespace rtype::game {

struct Intent;

/**
 * @brief Lets the behaviour of every body that has a Pilot, an Intent and a
 * Position write that Intent.
 *
 * It adds the elapsed time to the age of the Pilot and to the clock of the
 * CurrentAction first, so a behaviour steers for the end of the tick. A body
 * without a Pilot keeps its Intent: the ship of a player is steered by its
 * inputs. Run it before every system that reads the Intent: MovementSystem and
 * the weapon system.
 *
 * When the Intent a behaviour wrote ends an action or starts one, it hands
 * that to endEnemyAction() and startEnemyAction(), which publish on the event
 * bus it was given.
 */
class PilotSystem final : public engine::ISystem {
 public:
  explicit PilotSystem(engine::EventBus& events);

  void update(engine::ComponentRegistry& components,
              engine::Duration elapsed) override;

 private:
  /** @brief Lists in ships_ the center of every ship of a player, for the
   * behaviours to see the nearest one. */
  void collectShips(engine::ComponentRegistry& components);

  /** @brief Records the action @p intent of @p body ends or starts. */
  void passOnActionOf(engine::ComponentRegistry& components,
                      engine::Entity body, const Intent& intent);

  std::reference_wrapper<engine::EventBus> events_;
  /** @brief The ships of this tick; kept between ticks so collecting them
   * allocates nothing once it holds four. */
  std::vector<Position> ships_;
};

}  // namespace rtype::game
