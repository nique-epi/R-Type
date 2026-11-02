#include "GameEventEncoder.hpp"
#include <cstdint>
#include <utility>
#include "EncodedMessage.hpp"
#include "EntityDamage.hpp"
#include "EntityDestroyed.hpp"
#include "EntityDestruction.hpp"
#include "EntityIdConstants.hpp"
#include "EntitySpawn.hpp"
#include "EntitySpawned.hpp"
#include "EventMessage.hpp"
#include "GameEvent.hpp"
#include "GameEventConstants.hpp"
#include "MessagePayload.hpp"
#include "PlayerDeparture.hpp"
#include "PlayerElimination.hpp"
#include "PlayerHit.hpp"
#include "PlayerLeft.hpp"
#include "ProtocolConstants.hpp"
#include "WireEntityTypeMapping.hpp"

namespace rtype::server {

namespace {

network::DestructionReason toWireReason(game::DestructionCause cause) {
  switch (cause) {
    case game::DestructionCause::Killed:
      return network::DestructionReason::Killed;
    case game::DestructionCause::Removed:
      return network::DestructionReason::Removed;
  }
  return network::DestructionReason::Removed;
}

network::DepartureReason toWireReason(game::DepartureCause cause) {
  switch (cause) {
    case game::DepartureCause::Left:
      return network::DepartureReason::Left;
    case game::DepartureCause::TimedOut:
      return network::DepartureReason::TimedOut;
  }
  return network::DepartureReason::TimedOut;
}

/** @returns @p message encoded with its type. */
template <network::EncodableMessage Message>
network::EncodedMessage encodedMessage(const Message& message) {
  return network::EncodedMessage{.type = Message::TYPE,
                                 .payload = network::encodeMessage(message)};
}

}  // namespace

EventMessage GameEventEncoder::encode(const game::EntitySpawn& event) {
  const network::EntitySpawned message{
      .entityId = event.networkId,
      .entityType = toWireEntityType(event.type),
      .x = event.x,
      .y = event.y};
  return EventMessage{.message = encodedMessage(message), .guaranteed = false};
}

EventMessage GameEventEncoder::encode(const game::EntityDestruction& event) {
  const network::EntityDestroyed message{.entityId = event.networkId,
                                         .reason = toWireReason(event.cause)};
  return EventMessage{.message = encodedMessage(message), .guaranteed = true};
}

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

EventMessage GameEventEncoder::encode(const game::PlayerDeparture& event) {
  const network::PlayerLeft message{.playerNumber = event.playerNumber,
                                    .reason = toWireReason(event.cause)};
  return EventMessage{.message = encodedMessage(message), .guaranteed = true};
}

EventMessage GameEventEncoder::encodeGameEvent(network::GameEventKind kind,
                                               std::uint8_t playerNumber,
                                               std::uint32_t entityId) {
  const network::GameEvent message{.eventNumber = nextEventNumber_,
                                   .kind = kind,
                                   .playerNumber = playerNumber,
                                   .entityId = entityId};
  network::EncodedMessage encoded = encodedMessage(message);
  ++nextEventNumber_;
  return EventMessage{.message = std::move(encoded), .guaranteed = true};
}

}  // namespace rtype::server
