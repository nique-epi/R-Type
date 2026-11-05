#include "PilotSystem.hpp"
#include "BehaviourView.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
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

}  // namespace

void PilotSystem::update(engine::ComponentRegistry& components,
                         engine::Duration elapsed) {
  components.forEach<Pilot, Intent>([&components, elapsed](engine::Entity body,
                                                           Pilot& pilot,
                                                           Intent& intent) {
    const Position* const position = components.get<Position>(body);
    if (position == nullptr) {
      return;
    }
    pilot.age += elapsed;
    intent = pilot.behaviour.get().steer(
        BehaviourView{.position = *position,
                      .origin = pilot.origin,
                      .age = pilot.age,
                      .elapsed = elapsed,
                      .topSpeed = topSpeedOf(components, body)});
  });
}

}  // namespace rtype::game
