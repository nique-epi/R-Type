#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "EntityDamage.hpp"
#include "EntityDestroyed.hpp"
#include "EntityDestruction.hpp"
#include "EntitySpawn.hpp"
#include "EntitySpawned.hpp"
#include "EntityType.hpp"
#include "EventMessage.hpp"
#include "GameEvent.hpp"
#include "GameEventConstants.hpp"
#include "GameEventEncoder.hpp"
#include "GameEventTestConstants.hpp"
#include "MessagePayload.hpp"
#include "MessageType.hpp"
#include "NetworkException.hpp"
#include "PlayerDeparture.hpp"
#include "PlayerElimination.hpp"
#include "PlayerHit.hpp"
#include "PlayerLeft.hpp"
#include "WorldState.hpp"

namespace rtype::server::testing {

/**
 * Given an encoder
 * When a monster spawn is encoded
 * Then the message is an EntitySpawned with the monster's id, type and
 * position, sent once
 */
TEST(GameEventEncoderTest, EncodesASpawnSentOnce) {
  const EventMessage message = GameEventEncoder::encode(
      game::EntitySpawn{.networkId = MONSTER_NETWORK_ID,
                        .type = game::EntityType::Enemy,
                        .x = MONSTER_X,
                        .y = MONSTER_Y});

  EXPECT_EQ(message.message.type, network::MessageType::EntitySpawned);
  EXPECT_FALSE(message.guaranteed);
  EXPECT_EQ(
      network::decodeMessage<network::EntitySpawned>(message.message.payload),
      (network::EntitySpawned{.entityId = MONSTER_NETWORK_ID,
                              .entityType = network::WireEntityType::Bydo,
                              .x = MONSTER_X,
                              .y = MONSTER_Y}));
}

/**
 * Given an encoder
 * When a kill and a removal are encoded
 * Then the messages are EntityDestroyed with the reason Killed and the reason
 * Removed, each guaranteed
 */
TEST(GameEventEncoderTest, EncodesAKillAndARemovalWithTheirReason) {
  const EventMessage kill = GameEventEncoder::encode(
      game::EntityDestruction{.networkId = MONSTER_NETWORK_ID,
                              .cause = game::DestructionCause::Killed});
  const EventMessage removal = GameEventEncoder::encode(
      game::EntityDestruction{.networkId = MONSTER_NETWORK_ID,
                              .cause = game::DestructionCause::Removed});

  EXPECT_EQ(kill.message.type, network::MessageType::EntityDestroyed);
  EXPECT_TRUE(kill.guaranteed);
  EXPECT_TRUE(removal.guaranteed);
  EXPECT_EQ(
      network::decodeMessage<network::EntityDestroyed>(kill.message.payload),
      (network::EntityDestroyed{.entityId = MONSTER_NETWORK_ID,
                                .reason = network::DestructionReason::Killed}));
  EXPECT_EQ(
      network::decodeMessage<network::EntityDestroyed>(removal.message.payload),
      (network::EntityDestroyed{
          .entityId = MONSTER_NETWORK_ID,
          .reason = network::DestructionReason::Removed}));
}

/**
 * Given an encoder
 * When an entity damage, a player hit and a player elimination are encoded
 * Then each is a GameEvent of its kind, numbered 1, 2 and 3, with the player
 * number or the entity id the protocol page gives that kind and 0 in the
 * other field, each guaranteed
 */
TEST(GameEventEncoderTest, EncodesEachKindOfGameEventWithItsFields) {
  GameEventEncoder encoder;

  const EventMessage damaged =
      encoder.encode(game::EntityDamage{.networkId = MONSTER_NETWORK_ID});
  const EventMessage hit =
      encoder.encode(game::PlayerHit{.playerNumber = SECOND_PLAYER_NUMBER});
  const EventMessage eliminated = encoder.encode(
      game::PlayerElimination{.playerNumber = SECOND_PLAYER_NUMBER});

  EXPECT_EQ(damaged.message.type, network::MessageType::GameEvent);
  EXPECT_TRUE(damaged.guaranteed);
  EXPECT_EQ(network::decodeMessage<network::GameEvent>(damaged.message.payload),
            (network::GameEvent{.eventNumber = FIRST_EVENT_NUMBER,
                                .kind = network::GameEventKind::EntityDamaged,
                                .playerNumber = 0,
                                .entityId = MONSTER_NETWORK_ID}));
  EXPECT_EQ(network::decodeMessage<network::GameEvent>(hit.message.payload),
            (network::GameEvent{.eventNumber = FIRST_EVENT_NUMBER + 1,
                                .kind = network::GameEventKind::PlayerHit,
                                .playerNumber = SECOND_PLAYER_NUMBER,
                                .entityId = 0}));
  EXPECT_EQ(
      network::decodeMessage<network::GameEvent>(eliminated.message.payload),
      (network::GameEvent{.eventNumber = FIRST_EVENT_NUMBER + 2,
                          .kind = network::GameEventKind::PlayerEliminated,
                          .playerNumber = SECOND_PLAYER_NUMBER,
                          .entityId = 0}));
}

/**
 * Given an encoder
 * When a player that left and a player that timed out are encoded
 * Then the messages are PlayerLeft with the number of the player and the
 * reason Left and the reason TimedOut, each guaranteed
 */
TEST(GameEventEncoderTest, EncodesADepartureWithItsReason) {
  const EventMessage left = GameEventEncoder::encode(
      game::PlayerDeparture{.playerNumber = SECOND_PLAYER_NUMBER,
                            .cause = game::DepartureCause::Left});
  const EventMessage timedOut = GameEventEncoder::encode(
      game::PlayerDeparture{.playerNumber = SECOND_PLAYER_NUMBER,
                            .cause = game::DepartureCause::TimedOut});

  EXPECT_EQ(left.message.type, network::MessageType::PlayerLeft);
  EXPECT_TRUE(left.guaranteed);
  EXPECT_TRUE(timedOut.guaranteed);
  EXPECT_EQ(network::decodeMessage<network::PlayerLeft>(left.message.payload),
            (network::PlayerLeft{.playerNumber = SECOND_PLAYER_NUMBER,
                                 .reason = network::DepartureReason::Left}));
  EXPECT_EQ(
      network::decodeMessage<network::PlayerLeft>(timedOut.message.payload),
      (network::PlayerLeft{.playerNumber = SECOND_PLAYER_NUMBER,
                           .reason = network::DepartureReason::TimedOut}));
}

/**
 * Given an encoder
 * When 65537 game events are encoded
 * Then the numbers run 1 to 65535, then 0, then 1
 */
TEST(GameEventEncoderTest, WrapsTheEventNumberFromTheMaximumToZero) {
  GameEventEncoder encoder;
  std::vector<std::uint16_t> numbers;

  for (std::size_t event = 0; event < EVENTS_PAST_THE_WRAP; ++event) {
    const EventMessage message =
        encoder.encode(game::PlayerHit{.playerNumber = SECOND_PLAYER_NUMBER});
    numbers.push_back(
        network::decodeMessage<network::GameEvent>(message.message.payload)
            .eventNumber);
  }

  EXPECT_EQ(numbers.at(0), FIRST_EVENT_NUMBER);
  EXPECT_EQ(numbers.at(std::size_t{LARGEST_EVENT_NUMBER} - 1),
            LARGEST_EVENT_NUMBER);
  EXPECT_EQ(numbers.at(std::size_t{LARGEST_EVENT_NUMBER}), std::uint16_t{0});
  EXPECT_EQ(numbers.at(std::size_t{LARGEST_EVENT_NUMBER} + 1),
            FIRST_EVENT_NUMBER);
}

/**
 * Given an encoder
 * When an event naming the entity id 0 is encoded
 * Then it throws, and the next event still gets the first event number
 */
TEST(GameEventEncoderTest, KeepsTheEventNumberWhenAnEventIsRefused) {
  GameEventEncoder encoder;

  EXPECT_THROW(
      static_cast<void>(encoder.encode(game::EntityDamage{.networkId = 0})),
      network::InvalidFieldValueException);

  const EventMessage next =
      encoder.encode(game::EntityDamage{.networkId = MONSTER_NETWORK_ID});
  EXPECT_EQ(network::decodeMessage<network::GameEvent>(next.message.payload)
                .eventNumber,
            FIRST_EVENT_NUMBER);
}

}  // namespace rtype::server::testing
