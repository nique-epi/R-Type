#include "EnemyActions.hpp"
#include "ComponentRegistry.hpp"
#include "CurrentAction.hpp"
#include "EnemyActionStart.hpp"
#include "EnemyActionStarted.hpp"
#include "Entity.hpp"
#include "EventBus.hpp"
#include "NetworkIdentity.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

void startEnemyAction(engine::EventBus& events,
                      engine::ComponentRegistry& components,
                      const EnemyActionStart& start) {
  auto* const current = components.get<CurrentAction>(start.enemy);
  const auto* const identity = components.get<NetworkIdentity>(start.enemy);
  if (current == nullptr || identity == nullptr) {
    return;
  }
  current->key = start.action;
  current->since = engine::Duration::zero();
  events.publish(EnemyActionStarted{.networkId = identity->networkId,
                                    .attack = start.action.attack,
                                    .phase = start.action.phase});
}

void endEnemyAction(engine::ComponentRegistry& components,
                    engine::Entity enemy) {
  if (auto* const current = components.get<CurrentAction>(enemy)) {
    current->key.reset();
    current->since = engine::Duration::zero();
  }
}

}  // namespace rtype::game
