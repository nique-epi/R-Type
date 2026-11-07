#include "GameEventBroadcaster.hpp"
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include "BossArrival.hpp"
#include "BossHealthChanged.hpp"
#include "EnemyActionStarted.hpp"
#include "EntityDamage.hpp"
#include "EntityDestruction.hpp"
#include "EntitySpawn.hpp"
#include "EventBus.hpp"
#include "EventMessage.hpp"
#include "GameEventBroadcasterParts.hpp"
#include "GameEventConstants.hpp"
#include "NetworkException.hpp"
#include "PlayerDeparture.hpp"
#include "PlayerElimination.hpp"
#include "PlayerHit.hpp"
#include "ReplicationConstants.hpp"
#include "SubscriptionHandle.hpp"

namespace rtype::server {

template <typename Event>
void GameEventBroadcaster::subscribeTo(std::string_view eventName) {
  subscriptions_.push_back(
      events_.subscribe<Event>([this, eventName](const Event& event) {
        const std::optional<EventMessage> message = tryEncode(event, eventName);
        if (message.has_value()) {
          broadcast(*message);
        }
      }));
}

template <typename Event>
std::optional<EventMessage> GameEventBroadcaster::tryEncode(
    const Event& event, std::string_view eventName) {
  try {
    return encoder_.encode(event);
  } catch (const network::NetworkException& error) {
    logger_.warn("game event ", eventName, " not sent: ", error.what());
    return std::nullopt;
  }
}

GameEventBroadcaster::GameEventBroadcaster(GameEventBroadcasterParts parts)
    : events_(parts.events.get()),
      sendToEveryPlayer_(std::move(parts.sendToEveryPlayer)),
      guaranteeToEveryPlayer_(std::move(parts.guaranteeToEveryPlayer)),
      logger_(std::string{REPLICATION_LOGGER_NAME}) {
  subscribeTo<game::EntitySpawn>(ENTITY_SPAWN_EVENT_NAME);
  subscribeTo<game::EntityDestruction>(ENTITY_DESTRUCTION_EVENT_NAME);
  subscribeTo<game::EntityDamage>(ENTITY_DAMAGE_EVENT_NAME);
  subscribeTo<game::EnemyActionStarted>(ENEMY_ACTION_STARTED_EVENT_NAME);
  subscribeTo<game::BossArrival>(BOSS_ARRIVAL_EVENT_NAME);
  subscribeTo<game::BossHealthChanged>(BOSS_HEALTH_CHANGED_EVENT_NAME);
  subscribeTo<game::PlayerHit>(PLAYER_HIT_EVENT_NAME);
  subscribeTo<game::PlayerElimination>(PLAYER_ELIMINATION_EVENT_NAME);
  subscribeTo<game::PlayerDeparture>(PLAYER_DEPARTURE_EVENT_NAME);
}

GameEventBroadcaster::~GameEventBroadcaster() {
  for (const engine::SubscriptionHandle handle : subscriptions_) {
    events_.unsubscribe(handle);
  }
}

void GameEventBroadcaster::broadcast(const EventMessage& event) const {
  if (event.guaranteed) {
    guaranteeToEveryPlayer_(event.message);
    return;
  }
  sendToEveryPlayer_(event.message);
}

}  // namespace rtype::server
