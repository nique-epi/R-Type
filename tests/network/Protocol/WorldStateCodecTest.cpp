#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "ByteSequence.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "WorldState.hpp"
#include "WorldStateCodec.hpp"
#include "WorldStateFixtures.hpp"

using rtype::network::BufferUnderflowException;
using rtype::network::EntityState;
using rtype::network::MalformedMessageException;
using rtype::network::TooManyPlayersException;
using rtype::network::WorldState;
using rtype::network::WorldStateCodec;
using rtype::network::testing::BEYOND_RANGE_X;
using rtype::network::testing::BEYOND_RANGE_Y;
using rtype::network::testing::byteSequence;
using rtype::network::testing::DATAGRAM_PAYLOAD_LIMIT;
using rtype::network::testing::ENTITIES_FITTING_WITH_FOUR_PLAYERS;
using rtype::network::testing::ENTITY_TYPE_ABOVE_THE_LAST;
using rtype::network::testing::ENTITY_TYPE_BELOW_THE_FIRST;
using rtype::network::testing::EXAMPLE_ENTITY_ID;
using rtype::network::testing::FEW_ENTITIES;
using rtype::network::testing::fourPlayers;
using rtype::network::testing::LARGEST_WIRE_COORDINATE;
using rtype::network::testing::LATE_TICK;
using rtype::network::testing::NEAREST_EIGHTH_OF_UNROUNDED_X;
using rtype::network::testing::NEAREST_EIGHTH_OF_UNROUNDED_Y;
using rtype::network::testing::NUMBER_OF_A_FIFTH_PLAYER;
using rtype::network::testing::oneShipState;
using rtype::network::testing::SMALLEST_WIRE_COORDINATE;
using rtype::network::testing::SOME_TICK;
using rtype::network::testing::someEntities;
using rtype::network::testing::somePlayer;
using rtype::network::testing::UNROUNDED_X;
using rtype::network::testing::UNROUNDED_Y;

/**
 * Given a state with one player and one entity
 * When it is encoded
 * Then the payload is the 26 bytes of the protocol page: tick, playerCount,
 * the player, entityCount, the entity with x and y in eighths of a unit
 */
TEST(WorldStateCodec, EncodesTheLayoutOfTheProtocolPage) {
  const auto payloads = WorldStateCodec::encode(oneShipState());

  ASSERT_EQ(payloads.size(), 1U);
  EXPECT_EQ(payloads.front(),
            byteSequence({0x00, 0x00, 0x00, 0x03, 0x01, 0x01, 0x00, 0x00, 0x00,
                          0x07, 0x03, 0x00, 0x00, 0x00, 0x64, 0x00, 0x01, 0x00,
                          0x00, 0x00, 0x09, 0x01, 0x03, 0x20, 0x01, 0x94}));
}

/**
 * Given a state with players and entities
 * When it is encoded then decoded
 * Then the decoded state equals the original
 */
TEST(WorldStateCodec, DecodesWhatItEncoded) {
  const WorldState state{.tick = LATE_TICK,
                         .players = fourPlayers(),
                         .entities = someEntities(FEW_ENTITIES)};

  const auto payloads = WorldStateCodec::encode(state);

  ASSERT_EQ(payloads.size(), 1U);
  EXPECT_EQ(WorldStateCodec::decode(payloads.front()), state);
}

/**
 * Given a state without players and without entities
 * When it is encoded
 * Then one payload of 6 bytes is produced, and it decodes to the same state
 */
TEST(WorldStateCodec, EncodesAnEmptyStateAsOnePayload) {
  const WorldState state{.tick = SOME_TICK, .players = {}, .entities = {}};

  const auto payloads = WorldStateCodec::encode(state);

  ASSERT_EQ(payloads.size(), 1U);
  EXPECT_EQ(payloads.front().size(), rtype::network::WORLD_STATE_FIXED_SIZE);
  EXPECT_EQ(WorldStateCodec::decode(payloads.front()), state);
}

/**
 * Given a position that is not a multiple of an eighth of a unit
 * When it is encoded then decoded
 * Then it comes back as the nearest eighth
 */
TEST(WorldStateCodec, RoundsAPositionToTheNearestEighth) {
  WorldState state = oneShipState();
  state.entities.front().x = UNROUNDED_X;
  state.entities.front().y = UNROUNDED_Y;

  const WorldState decoded =
      WorldStateCodec::decode(WorldStateCodec::encode(state).front());

  EXPECT_EQ(decoded.entities.front().x, NEAREST_EIGHTH_OF_UNROUNDED_X);
  EXPECT_EQ(decoded.entities.front().y, NEAREST_EIGHTH_OF_UNROUNDED_Y);
}

/**
 * Given positions beyond the range of a wire coordinate
 * When they are encoded then decoded
 * Then they come back at the nearest end of the range, -4096 and 4095.875
 */
TEST(WorldStateCodec, ClampsAPositionToTheRangeOfTheWire) {
  WorldState state = oneShipState();
  state.entities.front().x = BEYOND_RANGE_X;
  state.entities.front().y = BEYOND_RANGE_Y;

  const WorldState decoded =
      WorldStateCodec::decode(WorldStateCodec::encode(state).front());

  EXPECT_EQ(decoded.entities.front().x, LARGEST_WIRE_COORDINATE);
  EXPECT_EQ(decoded.entities.front().y, SMALLEST_WIRE_COORDINATE);
}

/**
 * Given four players and 125 entities, the most one datagram holds
 * When the state is encoded
 * Then one payload of 1175 bytes is produced
 */
TEST(WorldStateCodec, KeepsTheLargestStateThatFitsInOnePayload) {
  const WorldState state{
      .tick = SOME_TICK,
      .players = fourPlayers(),
      .entities = someEntities(ENTITIES_FITTING_WITH_FOUR_PLAYERS)};

  const auto payloads = WorldStateCodec::encode(state);

  ASSERT_EQ(payloads.size(), 1U);
  EXPECT_EQ(payloads.front().size(), DATAGRAM_PAYLOAD_LIMIT);
}

/**
 * Given four players and 126 entities, one more than a datagram holds
 * When the state is encoded
 * Then two payloads are produced, each with the same tick and the four
 * players, holding 125 then 1 entities in their order, and none exceeds 1175
 * bytes
 */
TEST(WorldStateCodec, SplitsEntitiesThatDoNotFitIntoSeveralPayloads) {
  const WorldState state{
      .tick = SOME_TICK,
      .players = fourPlayers(),
      .entities = someEntities(ENTITIES_FITTING_WITH_FOUR_PLAYERS + 1)};

  const auto payloads = WorldStateCodec::encode(state);

  ASSERT_EQ(payloads.size(), 2U);
  const WorldState first = WorldStateCodec::decode(payloads[0]);
  const WorldState second = WorldStateCodec::decode(payloads[1]);
  EXPECT_LE(payloads[0].size(), DATAGRAM_PAYLOAD_LIMIT);
  EXPECT_EQ(first.tick, SOME_TICK);
  EXPECT_EQ(second.tick, SOME_TICK);
  EXPECT_EQ(first.players, state.players);
  EXPECT_EQ(second.players, state.players);
  EXPECT_EQ(first.entities.size(), ENTITIES_FITTING_WITH_FOUR_PLAYERS);
  EXPECT_EQ(second.entities.size(), 1U);
  EXPECT_EQ(second.entities.front(), state.entities.back());
}

/**
 * Given five players
 * When the state is encoded
 * Then it is refused, because a game has four players at most
 */
TEST(WorldStateCodec, RefusesMoreThanFourPlayers) {
  WorldState state{.tick = SOME_TICK, .players = fourPlayers(), .entities = {}};
  state.players.push_back(somePlayer(NUMBER_OF_A_FIFTH_PLAYER));

  EXPECT_THROW((void)WorldStateCodec::encode(state), TooManyPlayersException);
}

/**
 * Given a payload cut short at each length from 0 up to one byte less than its
 * size
 * When it is decoded
 * Then each cut is refused
 */
TEST(WorldStateCodec, RefusesATruncatedPayload) {
  const auto payload = WorldStateCodec::encode(oneShipState()).front();

  for (std::size_t length = 0; length < payload.size(); ++length) {
    const std::vector<std::byte> cut(
        payload.begin(), payload.begin() + static_cast<std::ptrdiff_t>(length));
    EXPECT_THROW((void)WorldStateCodec::decode(cut), BufferUnderflowException)
        << "length " << length;
  }
}

/**
 * Given a payload followed by one extra byte
 * When it is decoded
 * Then it is refused
 */
TEST(WorldStateCodec, RefusesBytesAfterTheLastEntity) {
  auto payload = WorldStateCodec::encode(oneShipState()).front();
  payload.push_back(std::byte{0});

  EXPECT_THROW((void)WorldStateCodec::decode(payload),
               MalformedMessageException);
}

/**
 * Given a payload announcing five players
 * When it is decoded
 * Then it is refused before any player is read
 */
TEST(WorldStateCodec, RefusesAnnouncedPlayersBeyondTheLimit) {
  EXPECT_THROW((void)WorldStateCodec::decode(byteSequence({0, 0, 0, 1, 5, 0})),
               MalformedMessageException);
}

/**
 * Given a payload announcing 255 entities in 6 bytes
 * When it is decoded
 * Then it is refused for being short, without reading outside the payload
 */
TEST(WorldStateCodec, RefusesAnnouncedEntitiesBeyondThePayload) {
  EXPECT_THROW(
      (void)WorldStateCodec::decode(byteSequence({0, 0, 0, 1, 0, 255})),
      BufferUnderflowException);
}

/**
 * Given an entity type of 0, and another of 5, which no entity has
 * When each payload is decoded
 * Then each is refused
 */
TEST(WorldStateCodec, RefusesAnUnknownEntityType) {
  const auto withType = [](unsigned int type) {
    return byteSequence(
        {0, 0, 0, 1, 0, 1, 0, 0, 0, EXAMPLE_ENTITY_ID, type, 0, 0, 0, 0});
  };

  EXPECT_THROW(
      (void)WorldStateCodec::decode(withType(ENTITY_TYPE_BELOW_THE_FIRST)),
      MalformedMessageException);
  EXPECT_THROW(
      (void)WorldStateCodec::decode(withType(ENTITY_TYPE_ABOVE_THE_LAST)),
      MalformedMessageException);
}

/**
 * Given the payload of the protocol page whose invulnerable byte is 2
 * When it is decoded
 * Then it is refused, since a bool on the wire is 0 or 1
 */
TEST(WorldStateCodec, RefusesAnInvulnerableFlagOtherThanZeroOrOne) {
  EXPECT_THROW((void)WorldStateCodec::decode(byteSequence(
                   {0x00, 0x00, 0x00, 0x03, 0x01, 0x01, 0x00, 0x00, 0x00,
                    0x07, 0x03, 0x00, 0x00, 0x00, 0x64, 0x02, 0x01, 0x00,
                    0x00, 0x00, 0x09, 0x01, 0x03, 0x20, 0x01, 0x94})),
               MalformedMessageException);
}
