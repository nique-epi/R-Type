#include "DamageRules.hpp"
#include <cstdint>
#include "ComponentRegistry.hpp"
#include "DamageConstants.hpp"
#include "EntityDamage.hpp"
#include "EntityDestruction.hpp"
#include "EventBus.hpp"
#include "Health.hpp"
#include "MissileHit.hpp"
#include "NetworkIdentity.hpp"

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
  components_.destroy(hit.target);
  events_.publish(EntityDestruction{.networkId = targetId,
                                    .cause = DestructionCause::Killed});
}

}  // namespace rtype::game
