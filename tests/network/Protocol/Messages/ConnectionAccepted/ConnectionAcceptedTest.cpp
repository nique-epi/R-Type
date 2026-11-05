#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <vector>
#include "ByteSequence.hpp"
#include "ConnectionAccepted.hpp"
#include "ConnectionAcceptedFixtures.hpp"
#include "ConnectionAcceptedTestConstants.hpp"
#include "DatagramCodec.hpp"
#include "EntitySide.hpp"
#include "EntityTypeEntry.hpp"
#include "MessagePayload.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "PacketHeader.hpp"
#include "ProtocolTestConstants.hpp"

namespace rtype::network::testing {

/**
 * Given the acceptance of player 1 on a server of a Drifter, an enemy, then
 * the ship, a player
 * When it is encoded
 * Then the payload is the player number, the count of types, then for each
 * type its side, the length of its name and its bytes, as the protocol page
 * writes them
 */
TEST(ConnectionAccepted, EncodesThePlayerNumberThenTheTypes) {
  const ConnectionAccepted acceptance{
      .playerNumber = 1,
      .types = {EntityTypeEntry{.side = EntitySide::Enemy, .name = "drifter"},
                EntityTypeEntry{.side = EntitySide::Player, .name = "ship"}}};

  EXPECT_EQ(encodeMessage(acceptance),
            byteSequence({0x01, 0x02, 0x02, 0x07, 'd', 'r', 'i', 'f', 't', 'e',
                          'r', 0x01, 0x04, 's', 'h', 'i', 'p'}));
}

/**
 * Given the acceptance of the last player, with a type of every side, one of
 * them with a name of every kind of character
 * When it is encoded then decoded
 * Then the decoded acceptance equals the original, the types in the same order
 */
TEST(ConnectionAccepted, DecodesWhatItEncoded) {
  const ConnectionAccepted acceptance{
      .playerNumber = LAST_PLAYER_NUMBER,
      .types = {EntityTypeEntry{.side = EntitySide::Player, .name = "ship"},
                EntityTypeEntry{.side = EntitySide::Enemy,
                                .name = std::string{NAME_OF_EVERY_KIND}},
                EntityTypeEntry{.side = EntitySide::PlayerMissile,
                                .name = "player_missile"},
                EntityTypeEntry{.side = EntitySide::EnemyMissile,
                                .name = "enemy_missile"}}};

  EXPECT_EQ(decodeMessage<ConnectionAccepted>(encodeMessage(acceptance)),
            acceptance);
}

/**
 * Given the payloads of the acceptance of players 1 and 4, then 0 and 5
 * When each is decoded
 * Then 1 and 4 give their player number, 0 and 5 are refused
 */
TEST(ConnectionAccepted, ReadsThePlayerNumbersOneToFourOnly) {
  EXPECT_EQ(decodeMessage<ConnectionAccepted>(
                byteSequence({0x01, 0x01, 0x02, 0x01, 'a'}))
                .playerNumber,
            1);
  EXPECT_EQ(decodeMessage<ConnectionAccepted>(
                byteSequence({0x04, 0x01, 0x02, 0x01, 'a'}))
                .playerNumber,
            LAST_PLAYER_NUMBER);
  EXPECT_THROW((void)decodeMessage<ConnectionAccepted>(
                   byteSequence({0x00, 0x01, 0x02, 0x01, 'a'})),
               InvalidFieldValueException);
  EXPECT_THROW((void)decodeMessage<ConnectionAccepted>(
                   byteSequence({0x05, 0x01, 0x02, 0x01, 'a'})),
               InvalidFieldValueException);
}

/**
 * Given the acceptance of a fifth player
 * When it is encoded
 * Then it is refused
 */
TEST(ConnectionAccepted, RefusesToWriteAPlayerNumberOutsideOneToFour) {
  ConnectionAccepted acceptance =
      acceptanceWithEnemyTypes(ONE_TYPE, SHORTEST_TYPE_NAME);
  acceptance.playerNumber = PLAYER_NUMBER_ABOVE_THE_LAST;

  EXPECT_THROW((void)encodeMessage(acceptance), InvalidFieldValueException);
}

/**
 * Given an empty payload, then a payload holding a player number only
 * When each is decoded as an acceptance
 * Then each is refused as cut short: an acceptance always carries its types
 */
TEST(ConnectionAccepted, RefusesAPayloadCutBeforeItsTypes) {
  EXPECT_THROW((void)decodeMessage<ConnectionAccepted>(byteSequence({})),
               BufferUnderflowException);
  EXPECT_THROW((void)decodeMessage<ConnectionAccepted>(byteSequence({0x01})),
               BufferUnderflowException);
}

/**
 * Given payloads announcing 0, 1, 32 and 33 types
 * When they are decoded
 * Then 0 and 33 are refused before any type is read, 1 and 32 are read
 */
TEST(ConnectionAccepted, ReadsOneToThirtyTwoTypesOnly) {
  EXPECT_THROW(
      (void)decodeMessage<ConnectionAccepted>(byteSequence({0x01, 0x00})),
      InvalidFieldValueException);
  EXPECT_EQ(
      decodeMessage<ConnectionAccepted>(
          encodeMessage(acceptanceWithEnemyTypes(ONE_TYPE, SHORTEST_TYPE_NAME)))
          .types.size(),
      ONE_TYPE);
  EXPECT_EQ(decodeMessage<ConnectionAccepted>(
                encodeMessage(acceptanceWithEnemyTypes(
                    MOST_TYPES_ON_THE_PROTOCOL_PAGE, SHORTEST_TYPE_NAME)))
                .types.size(),
            MOST_TYPES_ON_THE_PROTOCOL_PAGE);
  EXPECT_THROW(
      (void)decodeMessage<ConnectionAccepted>(byteSequence({0x01, 0x21})),
      InvalidFieldValueException);
}

/**
 * Given acceptances of 0 and 33 types
 * When they are encoded
 * Then both are refused
 */
TEST(ConnectionAccepted, WritesOneToThirtyTwoTypesOnly) {
  EXPECT_THROW((void)encodeMessage(
                   acceptanceWithEnemyTypes(NO_TYPE, SHORTEST_TYPE_NAME)),
               InvalidFieldValueException);
  EXPECT_THROW((void)encodeMessage(acceptanceWithEnemyTypes(
                   ONE_TYPE_TOO_MANY, SHORTEST_TYPE_NAME)),
               InvalidFieldValueException);
}

/**
 * Given a type whose name has 0, 1, 24 and 25 bytes
 * When its acceptance is decoded
 * Then 1 and 24 bytes are read, 0 and 25 are refused
 */
TEST(ConnectionAccepted, ReadsANameOfOneToTwentyFourBytes) {
  EXPECT_THROW((void)decodeMessage<ConnectionAccepted>(
                   oneTypeAcceptancePayload(ENEMY_SIDE_BYTE, EMPTY_TYPE_NAME)),
               InvalidTypeNameException);
  EXPECT_EQ(decodeMessage<ConnectionAccepted>(
                oneTypeAcceptancePayload(ENEMY_SIDE_BYTE, SHORTEST_TYPE_NAME))
                .types.front()
                .name,
            SHORTEST_TYPE_NAME);
  EXPECT_EQ(decodeMessage<ConnectionAccepted>(
                oneTypeAcceptancePayload(ENEMY_SIDE_BYTE, LONGEST_TYPE_NAME))
                .types.front()
                .name,
            LONGEST_TYPE_NAME);
  EXPECT_THROW((void)decodeMessage<ConnectionAccepted>(oneTypeAcceptancePayload(
                   ENEMY_SIDE_BYTE, TOO_LONG_TYPE_NAME)),
               InvalidTypeNameException);
}

/**
 * Given a type whose name has 0 and 25 bytes
 * When its acceptance is encoded
 * Then both are refused
 */
TEST(ConnectionAccepted, WritesANameOfOneToTwentyFourBytesOnly) {
  EXPECT_THROW(
      (void)encodeMessage(acceptanceWithEnemyTypes(ONE_TYPE, EMPTY_TYPE_NAME)),
      InvalidTypeNameException);
  EXPECT_THROW((void)encodeMessage(
                   acceptanceWithEnemyTypes(ONE_TYPE, TOO_LONG_TYPE_NAME)),
               InvalidTypeNameException);
}

/**
 * Given a name with an uppercase letter, one with a dash and one with a space
 * When an acceptance holding each is decoded
 * Then each is refused: a name is lowercase letters, digits and underscores
 */
TEST(ConnectionAccepted, RefusesANameOfOtherCharacters) {
  for (const std::string_view name :
       {NAME_WITH_AN_UPPERCASE_LETTER, NAME_WITH_A_DASH, NAME_WITH_A_SPACE}) {
    EXPECT_THROW((void)decodeMessage<ConnectionAccepted>(
                     oneTypeAcceptancePayload(ENEMY_SIDE_BYTE, name)),
                 InvalidTypeNameException)
        << name;
  }
}

/**
 * Given a type of side 0, 1, 4 and 5
 * When its acceptance is decoded
 * Then sides 1 and 4 are read, 0 and 5 are refused
 */
TEST(ConnectionAccepted, ReadsTheFourSidesOnly) {
  EXPECT_THROW((void)decodeMessage<ConnectionAccepted>(oneTypeAcceptancePayload(
                   SIDE_BELOW_THE_FIRST, SHORTEST_TYPE_NAME)),
               InvalidFieldValueException);
  EXPECT_EQ(decodeMessage<ConnectionAccepted>(
                oneTypeAcceptancePayload(PLAYER_SIDE_BYTE, SHORTEST_TYPE_NAME))
                .types.front()
                .side,
            EntitySide::Player);
  EXPECT_EQ(
      decodeMessage<ConnectionAccepted>(
          oneTypeAcceptancePayload(ENEMY_MISSILE_SIDE_BYTE, SHORTEST_TYPE_NAME))
          .types.front()
          .side,
      EntitySide::EnemyMissile);
  EXPECT_THROW((void)decodeMessage<ConnectionAccepted>(oneTypeAcceptancePayload(
                   SIDE_ABOVE_THE_LAST, SHORTEST_TYPE_NAME)),
               InvalidFieldValueException);
}

/**
 * Given an acceptance announcing two types that holds one
 * When it is decoded
 * Then it is refused as cut short
 */
TEST(ConnectionAccepted, RefusesTypesCutShort) {
  EXPECT_THROW((void)decodeMessage<ConnectionAccepted>(
                   byteSequence({0x01, 0x02, 0x02, 0x01, 'a'})),
               BufferUnderflowException);
}

/**
 * Given an acceptance of one type followed by one more byte
 * When it is decoded
 * Then it is refused
 */
TEST(ConnectionAccepted, RefusesBytesAfterTheLastType) {
  EXPECT_THROW((void)decodeMessage<ConnectionAccepted>(
                   byteSequence({0x01, 0x01, 0x02, 0x01, 'a', 0x00})),
               TrailingBytesException);
}

/**
 * Given 32 types, each with a name of 24 bytes
 * When their acceptance is encoded in a datagram
 * Then the datagram holds the bytes the protocol page adds up, and fits the
 * largest datagram of the protocol
 */
TEST(ConnectionAccepted, TheLargestAcceptanceFitsOneDatagram) {
  const auto datagram = DatagramCodec::encode(
      PacketHeader{}, acceptanceWithEnemyTypes(MOST_TYPES_ON_THE_PROTOCOL_PAGE,
                                               LONGEST_TYPE_NAME));

  EXPECT_EQ(datagram.size(), LARGEST_ACCEPTANCE_ON_THE_PROTOCOL_PAGE);
  EXPECT_LE(datagram.size(), MAX_DATAGRAM_SIZE);
}

}  // namespace rtype::network::testing
