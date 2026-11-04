#include "EnemyActions.hpp"
#include "ComponentRegistry.hpp"
#include "CurrentAction.hpp"
#include "EnemyActionStarted.hpp"
#include "EventBus.hpp"
#include "NetworkIdentity.hpp"

namespace rtype::game {

void startEnemyAction(engine::EventBus& events,
                      engine::ComponentRegistry& components,
                      const EnemyActionStart& start) {
  CurrentAction* const current = components.get<CurrentAction>(start.enemy);
  const NetworkIdentity* const identity =
      components.get<NetworkIdentity>(start.enemy);
  if (current == nullptr || identity == nullptr) {
    return;
  }
  current->key = start.action;
  events.publish(EnemyActionStarted{.networkId = identity->networkId,
                                    .attack = start.action.attack,
                                    .phase = start.action.phase});
}

void endEnemyAction(engine::ComponentRegistry& components,
                    engine::Entity enemy) {
  if (CurrentAction* const current = components.get<CurrentAction>(enemy)) {
    current->key.reset();
  }
}

}  // namespace rtype::game
