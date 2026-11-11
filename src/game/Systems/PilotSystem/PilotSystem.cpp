#include "PilotSystem.hpp"
#include <algorithm>
#include <optional>
#include <span>
#include "BehaviourView.hpp"
#include "Boss.hpp"
#include "CollisionBox.hpp"
#include "ComponentRegistry.hpp"
#include "CurrentAction.hpp"
#include "EffectStarted.hpp"
#include "EnemyActionStart.hpp"
#include "EnemyActions.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "EventBus.hpp"
#include "Health.hpp"
#include "Intent.hpp"
#include "NetworkIdentity.hpp"
#include "Pilot.hpp"
#include "Position.hpp"
#include "SheetVisualNames.hpp"
#include "ShownLook.hpp"
#include "SteadyMotion.hpp"
#include "Summoning.hpp"
#include "TimeConstants.hpp"
#include "VisualNames.hpp"

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
  auto* const current = components.get<CurrentAction>(body);
  if (current != nullptr) {
    current->since += elapsed;
  }
  return current;
}

/** @returns The center among @p ships nearest to @p from; nothing when
 * @p ships is empty. */
std::optional<Position> nearestOf(std::span<const Position> ships,
                                  Position from) {
  const auto squaredDistance = [from](const Position& ship) noexcept {
    const float across = ship.x - from.x;
    const float down = ship.y - from.y;
    return (across * across) + (down * down);
  };
  const auto nearest = std::ranges::min_element(ships, {}, squaredDistance);
  if (nearest == ships.end()) {
    return std::nullopt;
  }
  return *nearest;
}

/** @returns Whether @p health has fewer points than it appeared with. */
bool isWounded(const Health* health) {
  return health != nullptr && health->points < health->full;
}

/** @returns The share of its health points @p health has left, 1 without
 * one. */
float healthShareOf(const Health* health) {
  if (health == nullptr || health->full <= 0) {
    return 1.0F;
  }
  return static_cast<float>(health->points) / static_cast<float>(health->full);
}

/** @brief Applies to @p body what @p intent asks of its body: a jump to a
 * position, the size of its collision box (its offset stays), a top speed;
 * each only when asked and when the body has the component. */
void reshapeAsAsked(engine::ComponentRegistry& components, engine::Entity body,
                    const Intent& intent) {
  auto* const position = components.get<Position>(body);
  if (intent.jumpsTo.has_value() && position != nullptr) {
    *position = *intent.jumpsTo;
  }
  auto* const box = components.get<CollisionBox>(body);
  if (intent.resizesTo.has_value() && box != nullptr) {
    box->width = intent.resizesTo->width;
    box->height = intent.resizesTo->height;
  }
  auto* const motion = components.get<SteadyMotion>(body);
  if (intent.topSpeedTo.has_value() && motion != nullptr) {
    motion->topSpeed = *intent.topSpeedTo;
  }
}

/** @returns Whether @p boss enraged; false for a body that is no boss. */
bool isEnraged(const Boss* boss) { return boss != nullptr && boss->enraged; }

/** @returns @p view with what the components of @p body tell of it: its
 * health, its rage, the actions it ended and its network id, and with the
 * body itself, for a behaviour that reaches its components. */
BehaviourView withConditionOf(BehaviourView view,
                              engine::ComponentRegistry& components,
                              engine::Entity body) {
  const Health* const health = components.get<Health>(body);
  view.wounded = isWounded(health);
  view.healthShare = healthShareOf(health);
  view.enraged = isEnraged(components.get<Boss>(body));
  const CurrentAction* const current = components.get<CurrentAction>(body);
  view.actionsEnded = current == nullptr ? 0U : current->ended;
  const NetworkIdentity* const identity = components.get<NetworkIdentity>(body);
  view.networkId = identity == nullptr ? 0U : identity->networkId;
  view.entity = body;
  return view;
}

}  // namespace

PilotSystem::PilotSystem(engine::EventBus& events) : events_(events) {}

void PilotSystem::collectShips(engine::ComponentRegistry& components) {
  ships_.clear();
  components.forEach<EntityType, Position>(
      [this](engine::Entity, const EntityType& type, const Position& position) {
        if (type == EntityType::Player) {
          ships_.push_back(position);
        }
      });
}

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

void PilotSystem::announceEffectOf(engine::ComponentRegistry& components,
                                   engine::Entity body, const Intent& intent) {
  if (!intent.playsEffect.has_value()) {
    return;
  }
  const VisualNames* const visuals = components.get<VisualNames>(body);
  const NetworkIdentity* const identity = components.get<NetworkIdentity>(body);
  if (visuals == nullptr || visuals->names == nullptr || identity == nullptr ||
      !namesEffect(*visuals->names, *intent.playsEffect)) {
    return;
  }
  events_.get().publish(EffectStarted{.networkId = identity->networkId,
                                      .effect = *intent.playsEffect});
}

void PilotSystem::applyLookOf(engine::ComponentRegistry& components,
                              engine::Entity body, const Intent& intent) {
  if (!intent.showsLook.has_value()) {
    return;
  }
  const VisualNames* const visuals = components.get<VisualNames>(body);
  auto* const shown = components.get<ShownLook>(body);
  if (visuals == nullptr || visuals->names == nullptr || shown == nullptr) {
    return;
  }
  shown->number =
      lookNumberOf(*visuals->names, *intent.showsLook).value_or(shown->number);
}

void PilotSystem::carryOutIntentOf(engine::ComponentRegistry& components,
                                   engine::Entity body, const Intent& intent) {
  passOnActionOf(components, body, intent);
  announceEffectOf(components, body, intent);
  applyLookOf(components, body, intent);
  reshapeAsAsked(components, body, intent);
}

void PilotSystem::update(engine::ComponentRegistry& components,
                         engine::Duration elapsed) {
  collectShips(components);
  components.forEach<Pilot, Intent>(
      [this, &components, elapsed](engine::Entity body, Pilot& pilot,
                                   Intent& intent) {
        auto* const position = components.get<Position>(body);
        if (position == nullptr) {
          return;
        }
        if (components.has<Summoning>(body)) {
          intent = Intent{};
          return;
        }
        pilot.age += elapsed;
        const CurrentAction* const current =
            tickedActionOf(components, body, elapsed);
        intent = pilot.behaviour.get().steer(withConditionOf(
            BehaviourView{
                .position = *position,
                .origin = pilot.origin,
                .age = pilot.age,
                .elapsed = elapsed,
                .topSpeed = topSpeedOf(components, body),
                .settings = pilot.settings,
                .action = current == nullptr ? std::nullopt : current->key,
                .actionAge = current == nullptr ? engine::Duration::zero()
                                                : current->since,
                .nearestShip = nearestOf(ships_, *position),
                .behaviourState = &pilot.behaviourState},
            components, body));
        carryOutIntentOf(components, body, intent);
      });
}

}  // namespace rtype::game
