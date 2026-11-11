#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "EntityDamage.hpp"
#include "EventMessage.hpp"
#include "GameEvent.hpp"
#include "GameEventConstants.hpp"
#include "GameEventEncoder.hpp"
#include "GameEventTestConstants.hpp"
#include "MessagePayload.hpp"
#include "MessageType.hpp"
#include "NetworkException.hpp"
#include "PlayerElimination.hpp"
#include "PlayerHit.hpp"

namespace rtype::server::testing {

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
