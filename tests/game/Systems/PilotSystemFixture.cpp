#include "PilotSystemFixture.hpp"
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>
#include <vector>
#include "ActionKey.hpp"
#include "BehaviourTestConstants.hpp"
#include "BuiltInBehaviours.hpp"
#include "ComponentRegistry.hpp"
#include "CurrentAction.hpp"
#include "EnemyActionStarted.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "IBehaviour.hpp"
#include "Intent.hpp"
#include "NetworkIdentity.hpp"
#include "Pilot.hpp"
#include "Position.hpp"
#include "SteadyMotion.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

PilotSystemFixture::PilotSystemFixture()
    : behaviours_(builtInBehaviours()), pilots_(events_) {
  events_.subscribe<EnemyActionStarted>(
      [this](const EnemyActionStarted& started) {
        startedActions_.push_back(started);
      });
}

engine::Entity PilotSystemFixture::addBody(
    std::string_view behaviour, std::span<const std::int64_t> settings,
    Position start) {
  const engine::Entity body = entities_.create();
  components_.add(body, start);
  components_.add(body, Intent{});
  components_.add(body, SteadyMotion{.topSpeed = BODY_TOP_SPEED});
  components_.add(body, Pilot{.behaviour = *behaviours_.find(behaviour),
                              .origin = start,
                              .age = engine::Duration::zero(),
                              .settings = settings});
  return body;
}

engine::Entity PilotSystemFixture::addEnemy(const IBehaviour& behaviour,
                                            std::uint32_t networkId) {
  const engine::Entity enemy = entities_.create();
  components_.add(enemy, Position{.x = 0.0F, .y = 0.0F});
  components_.add(enemy, Intent{});
  components_.add(enemy, CurrentAction{});
  components_.add(enemy, NetworkIdentity{.networkId = networkId});
  components_.add(enemy, Pilot{.behaviour = behaviour,
                               .origin = Position{.x = 0.0F, .y = 0.0F},
                               .age = engine::Duration::zero(),
                               .settings = {}});
  return enemy;
}

void PilotSystemFixture::addShip(Position position) {
  const engine::Entity ship = entities_.create();
  components_.add(ship, position);
  components_.add(ship, EntityType::Player);
}

std::optional<ActionKey> PilotSystemFixture::actionOf(engine::Entity enemy) {
  const CurrentAction* const current = components_.get<CurrentAction>(enemy);
  return current == nullptr ? std::nullopt : current->key;
}

std::vector<EnemyActionStarted> PilotSystemFixture::takeStartedActions() {
  events_.dispatch();
  std::vector<EnemyActionStarted> taken;
  taken.swap(startedActions_);
  return taken;
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
