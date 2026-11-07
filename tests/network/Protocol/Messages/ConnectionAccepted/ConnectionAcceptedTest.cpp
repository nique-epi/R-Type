#include <gtest/gtest.h>
#include <cstddef>
#include <limits>
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
#include "ShipGun.hpp"

namespace rtype::network::testing {

/**
 * Given the acceptance of player 1 on a server of a Drifter, an enemy of 64 by
 * 32 units, then the ship, a player of 96 by 48, with a gun that predicts
 * nothing
 * When it is encoded
 * Then the payload is the player number, the count of types, then for each
 * type its side, the length of its name, its bytes and its box, then the 14
 * bytes of the gun, as the protocol page writes them
 */
TEST(ConnectionAccepted, EncodesThePlayerNumberThenTheTypes) {
  const ConnectionAccepted acceptance{
      .playerNumber = 1,
      .types = {EntityTypeEntry{.side = EntitySide::Enemy,
                                .name = "drifter",
                                .hitboxWidth = 64,
                                .hitboxHeight = 32},
                EntityTypeEntry{.side = EntitySide::Player,
                                .name = "ship",
                                .hitboxWidth = 96,
                                .hitboxHeight = 48}}};

  EXPECT_EQ(
      encodeMessage(acceptance),
      byteSequence({0x01, 0x02, 0x02, 0x07, 'd',  'r',  'i',  'f',  't',  'e',
                    'r',  0x00, 0x40, 0x00, 0x20, 0x01, 0x04, 's',  'h',  'i',
                    'p',  0x00, 0x60, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}));
}

/**
 * Given the acceptance of a ship and its missile, the gun of the shipped ship:
 * 20 ticks between shots, ready at once, the missile flying right at 960
 * units per second
 * When it is encoded
 * Then its last 14 bytes are the ship type, the missile type, 20 and 0 on two
 * bytes each, then 960 and 0 as big-endian floats
 */
TEST(ConnectionAccepted, EncodesTheGunAfterTheTypes) {
  const std::vector<std::byte> payload = encodeMessage(
      acceptanceWithGun(ShipGun{.shipType = SHIP_TYPE_NUMBER,
                                .projectileType = MISSILE_TYPE_NUMBER,
                                .intervalTicks = SHIPPED_INTERVAL_TICKS,
                                .firstShotTicks = 0,
                                .velocityX = SHIPPED_MISSILE_SPEED,
                                .velocityY = 0.0F}));

  EXPECT_EQ(
      std::vector<std::byte>(payload.end() - NO_GUN_BYTE_COUNT, payload.end()),
      byteSequence({0x01, 0x02, 0x00, 0x14, 0x00, 0x00, 0x44, 0x70, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00}));
}

/**
 * Given the acceptance of the last player, with a type of every side, one of
 * them with a name of every kind of character, and a gun
 * When it is encoded then decoded
 * Then the decoded acceptance equals the original, the types in the same order
 * with their boxes, and the gun
 */
TEST(ConnectionAccepted, DecodesWhatItEncoded) {
  const ConnectionAccepted acceptance{
      .playerNumber = LAST_PLAYER_NUMBER,
      .types = {EntityTypeEntry{.side = EntitySide::Player,
                                .name = "ship",
                                .hitboxWidth = 96,
                                .hitboxHeight = 48},
                EntityTypeEntry{.side = EntitySide::Enemy,
                                .name = std::string{NAME_OF_EVERY_KIND},
                                .hitboxWidth = 64,
                                .hitboxHeight = 32},
                EntityTypeEntry{.side = EntitySide::PlayerMissile,
                                .name = "player_missile",
                                .hitboxWidth = 64,
                                .hitboxHeight = 24},
                EntityTypeEntry{.side = EntitySide::EnemyMissile,
                                .name = "enemy_missile",
                                .hitboxWidth = 32,
                                .hitboxHeight = 32}},
      .shipGun = ShipGun{.shipType = 1,
                         .projectileType = 3,
                         .intervalTicks = SHIPPED_INTERVAL_TICKS,
                         .firstShotTicks = SHIPPED_INTERVAL_TICKS,
                         .velocityX = SHIPPED_MISSILE_SPEED,
                         .velocityY = 0.0F}};

  EXPECT_EQ(decodeMessage<ConnectionAccepted>(encodeMessage(acceptance)),
            acceptance);
}

/**
 * Given the payloads of the acceptance of players 1 and 4, then 0 and 5
 * When each is decoded
 * Then 1 and 4 give their player number, 0 and 5 are refused
 */
TEST(ConnectionAccepted, ReadsThePlayerNumbersOneToFourOnly) {
  EXPECT_EQ(decodeMessage<ConnectionAccepted>(acceptancePayloadOfPlayer(1))
                .playerNumber,
            1);
  EXPECT_EQ(decodeMessage<ConnectionAccepted>(
                acceptancePayloadOfPlayer(LAST_PLAYER_NUMBER))
                .playerNumber,
            LAST_PLAYER_NUMBER);
  EXPECT_THROW(
      (void)decodeMessage<ConnectionAccepted>(acceptancePayloadOfPlayer(0)),
      InvalidFieldValueException);
  EXPECT_THROW((void)decodeMessage<ConnectionAccepted>(
                   acceptancePayloadOfPlayer(PLAYER_NUMBER_ABOVE_THE_LAST)),
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
  EXPECT_THROW((void)decodeMessage<ConnectionAccepted>(byteSequence(
                   {0x01, 0x02, 0x02, 0x01, 'a', 0x00, 0x00, 0x00, 0x00})),
               BufferUnderflowException);
}

/**
 * Given an acceptance of one type, then one type and its box with no gun
 * When each is decoded
 * Then the second is refused as cut short: an acceptance always ends with the
 * gun
 */
TEST(ConnectionAccepted, RefusesAPayloadCutBeforeItsGun) {
  std::vector<std::byte> withoutGun = acceptancePayloadOfPlayer(1);
  withoutGun.resize(withoutGun.size() - NO_GUN_BYTE_COUNT);

  EXPECT_NO_THROW(
      (void)decodeMessage<ConnectionAccepted>(acceptancePayloadOfPlayer(1)));
  EXPECT_THROW((void)decodeMessage<ConnectionAccepted>(withoutGun),
               BufferUnderflowException);
}

/**
 * Given an acceptance of one type and its gun followed by one more byte
 * When it is decoded
 * Then it is refused
 */
TEST(ConnectionAccepted, RefusesBytesAfterTheGun) {
  std::vector<std::byte> payload = acceptancePayloadOfPlayer(1);
  payload.push_back(std::byte{0});

  EXPECT_THROW((void)decodeMessage<ConnectionAccepted>(payload),
               TrailingBytesException);
}

/**
 * Given a gun naming as its ship, then as its missile, the type after the two
 * of the table
 * When each acceptance is encoded
 * Then both are refused
 */
TEST(ConnectionAccepted, RefusesAGunNamingATypePastTheTable) {
  EXPECT_THROW((void)encodeMessage(acceptanceWithGun(
                   ShipGun{.shipType = TYPE_NUMBER_PAST_THE_TABLE})),
               InvalidFieldValueException);
  EXPECT_THROW((void)encodeMessage(acceptanceWithGun(
                   ShipGun{.projectileType = TYPE_NUMBER_PAST_THE_TABLE})),
               InvalidFieldValueException);
}

/**
 * Given a gun whose ship is the missile and whose missile is the ship, then a
 * missile and no ship
 * When each acceptance is encoded
 * Then both are refused: only a ship of a player fires a missile the client
 * predicts
 */
TEST(ConnectionAccepted, PredictsOnlyAMissileOfAPlayerFiredByAShip) {
  EXPECT_THROW((void)encodeMessage(acceptanceWithGun(
                   ShipGun{.shipType = MISSILE_TYPE_NUMBER,
                           .projectileType = SHIP_TYPE_NUMBER,
                           .intervalTicks = SHIPPED_INTERVAL_TICKS})),
               InvalidFieldValueException);
  EXPECT_THROW((void)encodeMessage(acceptanceWithGun(
                   ShipGun{.projectileType = MISSILE_TYPE_NUMBER,
                           .intervalTicks = SHIPPED_INTERVAL_TICKS})),
               InvalidFieldValueException);
}

/**
 * Given the gun of a ship firing its missile every 0 ticks, then every tick
 * When each acceptance is encoded
 * Then the first is refused and the second written
 */
TEST(ConnectionAccepted, RefusesAGunWithoutTicksBetweenShots) {
  EXPECT_THROW((void)encodeMessage(acceptanceWithGun(
                   ShipGun{.shipType = SHIP_TYPE_NUMBER,
                           .projectileType = MISSILE_TYPE_NUMBER,
                           .intervalTicks = 0})),
               InvalidFieldValueException);
  EXPECT_NO_THROW((void)encodeMessage(
      acceptanceWithGun(ShipGun{.shipType = SHIP_TYPE_NUMBER,
                                .projectileType = MISSILE_TYPE_NUMBER,
                                .intervalTicks = 1})));
}

/**
 * Given a gun whose missile flies at an infinite speed along x, then a NaN
 * along y
 * When each acceptance is encoded
 * Then both are refused
 */
TEST(ConnectionAccepted, RefusesAVelocityThatIsNotFinite) {
  EXPECT_THROW((void)encodeMessage(acceptanceWithGun(ShipGun{
                   .velocityX = std::numeric_limits<float>::infinity()})),
               InvalidCoordinateException);
  EXPECT_THROW((void)encodeMessage(acceptanceWithGun(ShipGun{
                   .velocityY = std::numeric_limits<float>::quiet_NaN()})),
               InvalidCoordinateException);
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
