#include "GameEventEncoder.hpp"
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include "EncodedMessage.hpp"
#include "EntityDamage.hpp"
#include "EntityDestroyed.hpp"
#include "EntityDestruction.hpp"
#include "EntitySpawn.hpp"
#include "EntitySpawned.hpp"
#include "GameEvent.hpp"
#include "GameEventConstants.hpp"
#include "MessagePayload.hpp"
#include "PlayerElimination.hpp"
#include "PlayerHit.hpp"
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

}  // namespace

EncodedMessage GameEventEncoder::encode(const game::EntitySpawn& event) {
  const network::EntitySpawned message{
      .entityId = event.networkId,
      .entityType = toWireEntityType(event.type),
      .x = event.x,
      .y = event.y};
  return EncodedMessage{.type = network::EntitySpawned::TYPE,
                        .payload = network::encodeMessage(message),
                        .copies = SINGLE_COPY};
}

EncodedMessage GameEventEncoder::encode(const game::EntityDestruction& event) {
  const network::EntityDestroyed message{.entityId = event.networkId,
                                         .reason = toWireReason(event.cause)};
  return EncodedMessage{.type = network::EntityDestroyed::TYPE,
                        .payload = network::encodeMessage(message),
                        .copies = REPEATED_MESSAGE_COPIES};
}

EncodedMessage GameEventEncoder::encode(const game::EntityDamage& event) {
  return encodeGameEvent(network::GameEventKind::EntityDamaged,
                         network::NO_PLAYER_NUMBER, event.networkId);
}

EncodedMessage GameEventEncoder::encode(const game::PlayerHit& event) {
  return encodeGameEvent(network::GameEventKind::PlayerHit, event.playerNumber,
                         network::NO_ENTITY_ID);
}

EncodedMessage GameEventEncoder::encode(const game::PlayerElimination& event) {
  return encodeGameEvent(network::GameEventKind::PlayerEliminated,
                         event.playerNumber, network::NO_ENTITY_ID);
}

EncodedMessage GameEventEncoder::encodeGameEvent(network::GameEventKind kind,
                                                 std::uint8_t playerNumber,
                                                 std::uint32_t entityId) {
  const network::GameEvent message{.eventNumber = nextEventNumber_,
                                   .kind = kind,
                                   .playerNumber = playerNumber,
                                   .entityId = entityId};
  std::vector<std::byte> payload = network::encodeMessage(message);
  ++nextEventNumber_;
  return EncodedMessage{.type = network::GameEvent::TYPE,
                        .payload = std::move(payload),
                        .copies = REPEATED_MESSAGE_COPIES};
}

}  // namespace rtype::server
