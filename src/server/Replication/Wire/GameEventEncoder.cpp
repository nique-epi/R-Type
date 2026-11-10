#include "GameEventEncoder.hpp"
#include <cstdint>
#include <utility>
#include "BossArrival.hpp"
#include "BossHealthChanged.hpp"
#include "BossStatus.hpp"
#include "EncodedMessage.hpp"
#include "EnemyAction.hpp"
#include "EnemyActionStarted.hpp"
#include "EnemyPhase.hpp"
#include "EntityDamage.hpp"
#include "EntityDestroyed.hpp"
#include "EntityDestruction.hpp"
#include "EntityIdConstants.hpp"
#include "EntitySpawn.hpp"
#include "EntitySpawned.hpp"
#include "EventMessage.hpp"
#include "GameEvent.hpp"
#include "LevelChange.hpp"
#include "LevelStatus.hpp"
#include "MessagePayload.hpp"
#include "PlayerDeparture.hpp"
#include "PlayerElimination.hpp"
#include "PlayerHit.hpp"
#include "PlayerLeft.hpp"
#include "ProtocolConstants.hpp"

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
    case game::DepartureCause::Expelled:
      return network::DepartureReason::Expelled;
  }
  return network::DepartureReason::TimedOut;
}

network::EnemyActionPhase toWirePhase(game::EnemyPhase phase) {
  switch (phase) {
    case game::EnemyPhase::Signal:
      return network::EnemyActionPhase::Signal;
    case game::EnemyPhase::Strike:
      return network::EnemyActionPhase::Strike;
    case game::EnemyPhase::Recover:
      return network::EnemyActionPhase::Recover;
    case game::EnemyPhase::Enraged:
      return network::EnemyActionPhase::Enraged;
  }
  return network::EnemyActionPhase::Signal;
}

/** @brief What a GameEvent tells: its kind, the player it concerns and the
 * entity it concerns, each left out when the kind names none. */
struct GameEventSubject {
  network::GameEventKind kind{};
  std::uint8_t playerNumber{network::NO_PLAYER_NUMBER};
  std::uint32_t entityId{identifiers::NO_ENTITY_ID};
};

/** @returns The guaranteed GameEvent of @p subject, numbered
 * @p nextEventNumber, which moves on to the next number once the message is
 * written. */
EventMessage numberedGameEvent(std::uint16_t& nextEventNumber,
                               const GameEventSubject& subject) {
  const network::GameEvent message{.eventNumber = nextEventNumber,
                                   .kind = subject.kind,
                                   .playerNumber = subject.playerNumber,
                                   .entityId = subject.entityId};
  network::EncodedMessage encoded = network::encodedMessage(message);
  ++nextEventNumber;
  return EventMessage{.message = std::move(encoded), .guaranteed = true};
}

}  // namespace

EventMessage GameEventEncoder::encode(const game::EntitySpawn& event) {
  const network::EntitySpawned message{.entityId = event.networkId,
                                       .entityType = event.typeNumber,
                                       .x = event.x,
                                       .y = event.y};
  return EventMessage{.message = network::encodedMessage(message),
                      .guaranteed = false};
}

EventMessage GameEventEncoder::encode(const game::EntityDestruction& event) {
  const network::EntityDestroyed message{.entityId = event.networkId,
                                         .reason = toWireReason(event.cause)};
  return EventMessage{.message = network::encodedMessage(message),
                      .guaranteed = true};
}

EventMessage GameEventEncoder::encode(const game::EnemyActionStarted& event) {
  const network::EnemyAction message{.entityId = event.networkId,
                                     .attack = event.attack,
                                     .phase = toWirePhase(event.phase)};
  return EventMessage{.message = network::encodedMessage(message),
                      .guaranteed = true};
}

EventMessage GameEventEncoder::encode(
    [[maybe_unused]] const game::BossArrival& event) {
  return EventMessage{.message = network::encodedMessage(network::BossStatus{
                          .stage = network::BossStage::Arriving}),
                      .guaranteed = true};
}

EventMessage GameEventEncoder::encode(const game::BossHealthChanged& event) {
  network::BossStage stage = network::BossStage::Fighting;
  if (event.points == 0) {
    stage = network::BossStage::Defeated;
  } else if (event.enraged) {
    stage = network::BossStage::Enraged;
  }
  return EventMessage{.message = network::encodedMessage(
                          network::BossStatus{.bossEntityId = event.networkId,
                                              .health = event.points,
                                              .fullHealth = event.full,
                                              .stage = stage}),
                      .guaranteed = true};
}

EventMessage GameEventEncoder::encode(const game::EntityDamage& event) {
  return numberedGameEvent(
      nextEventNumber_,
      GameEventSubject{.kind = network::GameEventKind::EntityDamaged,
                       .entityId = event.networkId});
}

EventMessage GameEventEncoder::encode(const game::PlayerHit& event) {
  return numberedGameEvent(
      nextEventNumber_,
      GameEventSubject{.kind = network::GameEventKind::PlayerHit,
                       .playerNumber = event.playerNumber});
}

EventMessage GameEventEncoder::encode(const game::PlayerElimination& event) {
  return numberedGameEvent(
      nextEventNumber_,
      GameEventSubject{.kind = network::GameEventKind::PlayerEliminated,
                       .playerNumber = event.playerNumber});
}

EventMessage GameEventEncoder::encode(const game::PlayerDeparture& event) {
  const network::PlayerLeft message{.playerNumber = event.playerNumber,
                                    .reason = toWireReason(event.cause)};
  return EventMessage{.message = network::encodedMessage(message),
                      .guaranteed = true};
}

EventMessage GameEventEncoder::encode(const game::LevelChange& event) {
  return EventMessage{.message = network::encodedMessage(network::LevelStatus{
                          .biome = event.biome,
                          .scrollPosition = event.scrollPosition,
                          .scrollSpeed = event.scrollSpeed}),
                      .guaranteed = true};
}

}  // namespace rtype::server
