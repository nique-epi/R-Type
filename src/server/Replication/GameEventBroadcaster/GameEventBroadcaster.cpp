#include "GameEventBroadcaster.hpp"
#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "DatagramCodec.hpp"
#include "EncodedMessage.hpp"
#include "Endpoint.hpp"
#include "EntityDamage.hpp"
#include "EntityDestruction.hpp"
#include "EntitySpawn.hpp"
#include "EventBus.hpp"
#include "GameEventConstants.hpp"
#include "MessageType.hpp"
#include "NetworkException.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerDeparture.hpp"
#include "PlayerElimination.hpp"
#include "PlayerHit.hpp"
#include "ReplicationConstants.hpp"
#include "TimeConstants.hpp"
#include "TimerScheduler.hpp"

namespace rtype::server {

template <typename Event>
void GameEventBroadcaster::subscribeTo(engine::EventBus& events,
                                       std::string_view eventName) {
  events.subscribe<Event>([this, eventName](const Event& event) {
    const std::optional<EncodedMessage> message = tryEncode(event, eventName);
    if (message.has_value()) {
      broadcast(*message);
    }
  });
}

template <typename Event>
std::optional<EncodedMessage> GameEventBroadcaster::tryEncode(
    const Event& event, std::string_view eventName) {
  try {
    return encoder_.encode(event);
  } catch (const network::NetworkException& error) {
    logger_.warn("game event ", eventName, " not sent: ", error.what());
    return std::nullopt;
  }
}

GameEventBroadcaster::GameEventBroadcaster(
    engine::EventBus& events, engine::TimerScheduler& timers,
    const std::vector<network::Endpoint>& recipients,
    std::function<network::PacketHeader(const network::Endpoint&)> headerFor,
    std::function<void(network::OutgoingDatagram)> send)
    : timers_(timers),
      recipients_(recipients),
      headerFor_(std::move(headerFor)),
      send_(std::move(send)),
      logger_(std::string{REPLICATION_LOGGER_NAME}) {
  subscribeTo<game::EntitySpawn>(events, ENTITY_SPAWN_EVENT_NAME);
  subscribeTo<game::EntityDestruction>(events, ENTITY_DESTRUCTION_EVENT_NAME);
  subscribeTo<game::EntityDamage>(events, ENTITY_DAMAGE_EVENT_NAME);
  subscribeTo<game::PlayerHit>(events, PLAYER_HIT_EVENT_NAME);
  subscribeTo<game::PlayerElimination>(events, PLAYER_ELIMINATION_EVENT_NAME);
  subscribeTo<game::PlayerDeparture>(events, PLAYER_DEPARTURE_EVENT_NAME);
}

void GameEventBroadcaster::broadcast(const EncodedMessage& message) {
  if (recipients_.empty()) {
    return;
  }
  sendCopy(recipients_, message.type, message.payload);
  for (std::size_t copy = 1; copy < message.copies; ++copy) {
    timers_.scheduleOnce(
        REPEATED_MESSAGE_INTERVAL * static_cast<engine::Duration::rep>(copy),
        [this, recipients = recipients_, type = message.type,
         payload = message.payload]() { sendCopy(recipients, type, payload); });
  }
}

void GameEventBroadcaster::sendCopy(
    const std::vector<network::Endpoint>& recipients, network::MessageType type,
    const std::vector<std::byte>& payload) const {
  for (const network::Endpoint& recipient : recipients) {
    send_(network::OutgoingDatagram{.destination = recipient,
                                    .payload = network::DatagramCodec::encode(
                                        headerFor_(recipient), type, payload)});
  }
}

}  // namespace rtype::server
