#include "GameEventBroadcaster.hpp"
#include <cstddef>
#include <functional>
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
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerElimination.hpp"
#include "PlayerHit.hpp"
#include "TimeConstants.hpp"
#include "TimerScheduler.hpp"

namespace rtype::server {

GameEventBroadcaster::GameEventBroadcaster(
    engine::EventBus& events, engine::TimerScheduler& timers,
    const std::vector<network::Endpoint>& recipients,
    std::function<network::PacketHeader(const network::Endpoint&)> headerFor,
    std::function<void(network::OutgoingDatagram)> send)
    : timers_(timers),
      recipients_(recipients),
      headerFor_(std::move(headerFor)),
      send_(std::move(send)) {
  subscribeTo<game::EntitySpawn>(events);
  subscribeTo<game::EntityDestruction>(events);
  subscribeTo<game::EntityDamage>(events);
  subscribeTo<game::PlayerHit>(events);
  subscribeTo<game::PlayerElimination>(events);
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
