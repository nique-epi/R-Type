#include "GameEventEncoder.hpp"
#include <cstdint>
#include <utility>
#include "EncodedMessage.hpp"
#include "EntityDamage.hpp"
#include "EntityIdConstants.hpp"
#include "EventMessage.hpp"
#include "GameEvent.hpp"
#include "MessagePayload.hpp"
#include "PlayerElimination.hpp"
#include "PlayerHit.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::server {

EventMessage GameEventEncoder::encode(const game::EntityDamage& event) {
  return encodeGameEvent(network::GameEventKind::EntityDamaged,
                         network::NO_PLAYER_NUMBER, event.networkId);
}

EventMessage GameEventEncoder::encode(const game::PlayerHit& event) {
  return encodeGameEvent(network::GameEventKind::PlayerHit, event.playerNumber,
                         identifiers::NO_ENTITY_ID);
}

EventMessage GameEventEncoder::encode(const game::PlayerElimination& event) {
  return encodeGameEvent(network::GameEventKind::PlayerEliminated,
                         event.playerNumber, identifiers::NO_ENTITY_ID);
}

EventMessage GameEventEncoder::encodeGameEvent(network::GameEventKind kind,
                                               std::uint8_t playerNumber,
                                               std::uint32_t entityId) {
  const network::GameEvent message{.eventNumber = nextEventNumber_,
                                   .kind = kind,
                                   .playerNumber = playerNumber,
                                   .entityId = entityId};
  network::EncodedMessage encoded = network::encodedMessage(message);
  ++nextEventNumber_;
  return EventMessage{.message = std::move(encoded), .guaranteed = true};
}

}  // namespace rtype::server
