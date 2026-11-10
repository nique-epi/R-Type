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
 * When the Intent a behaviour wrote asks the body to jump, it moves it there.
 * When the Intent a behaviour wrote ends an action or starts one, it hands
 * that to endEnemyAction() and startEnemyAction(), which publish on the event
 * bus it was given. When it plays an effect of its sheet, it publishes an
 * EffectStarted; when it asks for a look of its sheet, it writes its number in
 * the ShownLook of the body, which every body made from a sheet has.
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

  /** @brief Carries out what the behaviour of @p body wrote in @p intent
   * besides moving: its action, its effect, its look, and its box and top
   * speed. */
  void carryOutIntentOf(engine::ComponentRegistry& components,
                        engine::Entity body, const Intent& intent);

  /** @brief Records the action @p intent of @p body ends or starts. */
  void passOnActionOf(engine::ComponentRegistry& components,
                      engine::Entity body, const Intent& intent);

  /** @brief Publishes an EffectStarted for the effect @p intent of @p body
   * plays, when the sheet of the body names it (VisualNames); a name the sheet
   * does not hold is dropped. */
  void announceEffectOf(engine::ComponentRegistry& components,
                        engine::Entity body, const Intent& intent);

  /** @brief Writes in the ShownLook of @p body the number of the look
   * @p intent asks for, when the sheet of the body names it; a name the sheet
   * does not hold leaves the look as it is. */
  static void applyLookOf(engine::ComponentRegistry& components,
                          engine::Entity body, const Intent& intent);

  std::reference_wrapper<engine::EventBus> events_;
  /** @brief The ships of this tick; kept between ticks so collecting them
   * allocates nothing once it holds four. */
  std::vector<Position> ships_;
};

}  // namespace rtype::game
