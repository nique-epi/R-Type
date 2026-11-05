#include "PilotSystemFixture.hpp"
#include <string_view>
#include "BehaviourTestConstants.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "Intent.hpp"
#include "Pilot.hpp"
#include "Position.hpp"
#include "RtypeBehaviours.hpp"
#include "SteadyMotion.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

PilotSystemFixture::PilotSystemFixture() : behaviours_(rtypeBehaviours()) {}

engine::Entity PilotSystemFixture::addBody(std::string_view behaviour,
                                           Position start) {
  const engine::Entity body = entities_.create();
  components_.add(body, start);
  components_.add(body, Intent{});
  components_.add(body, SteadyMotion{.topSpeed = BODY_TOP_SPEED});
  components_.add(body, Pilot{.behaviour = behaviours_.named(behaviour),
                              .origin = start,
                              .age = engine::Duration::zero()});
  return body;
}

void PilotSystemFixture::run(engine::Duration elapsed) {
  pilots_.update(components_, elapsed);
  movement_.update(components_, elapsed);
}

Position PilotSystemFixture::positionOf(engine::Entity body) {
  const Position* const position = components_.get<Position>(body);
  return position == nullptr ? Position{.x = 0.0F, .y = 0.0F} : *position;
}

engine::ComponentRegistry& PilotSystemFixture::components() {
  return components_;
}

}  // namespace rtype::game::testing
