#include "DamageRules.hpp"
#include <cstdint>
#include <optional>
#include "ComponentRegistry.hpp"
#include "DamageConstants.hpp"
#include "EnemyKilled.hpp"
#include "EntityDamage.hpp"
#include "EntityDestruction.hpp"
#include "EventBus.hpp"
#include "Health.hpp"
#include "MissileHit.hpp"
#include "MissileOwner.hpp"
#include "NetworkIdentity.hpp"
#include "ScoreValue.hpp"

namespace rtype::game {

DamageRules::DamageRules(engine::EventBus& events,
                         engine::ComponentRegistry& components)
    : events_(events),
      components_(components),
      subscription_(events.subscribe<MissileHit>(
          [this](const MissileHit& hit) { onMissileHit(hit); })) {}

DamageRules::~DamageRules() { events_.unsubscribe(subscription_); }

void DamageRules::onMissileHit(const MissileHit& hit) {
  const NetworkIdentity* missileIdentity =
      components_.get<NetworkIdentity>(hit.missile);
  const NetworkIdentity* targetIdentity =
      components_.get<NetworkIdentity>(hit.target);
  if (missileIdentity == nullptr || targetIdentity == nullptr) {
    return;
  }
  const std::uint32_t missileId = missileIdentity->networkId;
  const std::uint32_t targetId = targetIdentity->networkId;

  const MissileOwner* owner = components_.get<MissileOwner>(hit.missile);
  const std::optional<std::uint8_t> shooter =
      owner == nullptr ? std::nullopt
                       : std::optional<std::uint8_t>{owner->playerNumber};

  components_.destroy(hit.missile);
  events_.publish(EntityDestruction{.networkId = missileId,
                                    .cause = DestructionCause::Removed});

  auto* health = components_.get<Health>(hit.target);
  if (health == nullptr) {
    return;
  }
  health->points -= MISSILE_DAMAGE;
  if (health->points > 0) {
    events_.publish(EntityDamage{.networkId = targetId});
    return;
  }
  const ScoreValue* score = components_.get<ScoreValue>(hit.target);
  const std::uint32_t points = score == nullptr ? 0 : score->points;
  components_.destroy(hit.target);
  events_.publish(EntityDestruction{.networkId = targetId,
                                    .cause = DestructionCause::Killed});
  if (shooter.has_value() && points > 0) {
    events_.publish(
        EnemyKilled{.playerNumber = shooter.value(), .points = points});
  }
}

}  // namespace rtype::game
