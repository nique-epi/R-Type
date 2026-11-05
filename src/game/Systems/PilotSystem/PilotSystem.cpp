#include "PilotSystem.hpp"
#include <optional>
#include "BehaviourView.hpp"
#include "ComponentRegistry.hpp"
#include "CurrentAction.hpp"
#include "EnemyActionStart.hpp"
#include "EnemyActions.hpp"
#include "Entity.hpp"
#include "EventBus.hpp"
#include "Intent.hpp"
#include "Pilot.hpp"
#include "Position.hpp"
#include "SteadyMotion.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

namespace {

/** @returns The top speed of @p body in @p components, 0 when it has no
 * SteadyMotion. */
float topSpeedOf(engine::ComponentRegistry& components, engine::Entity body) {
  const SteadyMotion* const motion = components.get<SteadyMotion>(body);
  return motion == nullptr ? 0.0F : motion->topSpeed;
}

/** @returns The CurrentAction of @p body in @p components, its clock moved
 * on by @p elapsed; nullptr for a body without one. */
CurrentAction* tickedActionOf(engine::ComponentRegistry& components,
                              engine::Entity body, engine::Duration elapsed) {
  CurrentAction* const current = components.get<CurrentAction>(body);
  if (current != nullptr) {
    current->since += elapsed;
  }
  return current;
}

}  // namespace

PilotSystem::PilotSystem(engine::EventBus& events) : events_(events) {}

void PilotSystem::passOnActionOf(engine::ComponentRegistry& components,
                                 engine::Entity body, const Intent& intent) {
  if (intent.endsAction) {
    endEnemyAction(components, body);
  }
  if (intent.startsAction.has_value()) {
    startEnemyAction(
        events_.get(), components,
        EnemyActionStart{.enemy = body, .action = *intent.startsAction});
  }
}

void PilotSystem::update(engine::ComponentRegistry& components,
                         engine::Duration elapsed) {
  components.forEach<Pilot, Intent>(
      [this, &components, elapsed](engine::Entity body, Pilot& pilot,
                                   Intent& intent) {
        const Position* const position = components.get<Position>(body);
        if (position == nullptr) {
          return;
        }
        pilot.age += elapsed;
        const CurrentAction* const current =
            tickedActionOf(components, body, elapsed);
        intent = pilot.behaviour.get().steer(BehaviourView{
            .position = *position,
            .origin = pilot.origin,
            .age = pilot.age,
            .elapsed = elapsed,
            .topSpeed = topSpeedOf(components, body),
            .settings = pilot.settings,
            .action = current == nullptr ? std::nullopt : current->key,
            .actionAge = current == nullptr ? engine::Duration::zero()
                                            : current->since});
        passOnActionOf(components, body, intent);
      });
}

}  // namespace rtype::game
