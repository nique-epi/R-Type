#include <gtest/gtest.h>
#include "ByteSequence.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"
#include "PlayerLooks.hpp"

namespace rtype::network::testing {

/**
 * Given the four players of a full game, each with a look
 * When it is encoded
 * Then the payload holds 21 bytes: the count, then four times a player number
 * and four look bytes (1 + 4 x 5)
 */
TEST(PlayerLooks, FourPlayersTakeTwentyOneBytes) {
  const PlayerLooks looks{
      .players = {{.playerNumber = 1, .look = {.head = 1}},
                  {.playerNumber = 2, .look = {.eyes = 9}},
                  {.playerNumber = 3, .look = {.face = 10}},
                  {.playerNumber = 4, .look = {.hull = 26}}}};

  EXPECT_EQ(encodeMessage(looks),
            byteSequence({0x04, 0x01, 0x01, 0x00, 0x00, 0x00, 0x02,
                          0x00, 0x09, 0x00, 0x00, 0x03, 0x00, 0x00,
                          0x0A, 0x00, 0x04, 0x00, 0x00, 0x00, 0x1A}));
}

/**
 * Given two players with looks
 * When they are encoded then decoded
 * Then the decoded looks equal the original
 */
TEST(PlayerLooks, DecodesWhatItEncoded) {
  const PlayerLooks looks{
      .players = {{.playerNumber = 1, .look = {.head = 3, .eyes = 9}},
                  {.playerNumber = 4, .look = {.face = 10}}}};

  EXPECT_EQ(decodeMessage<PlayerLooks>(encodeMessage(looks)), looks);
}

/**
 * Given a payload that lists the first player twice
 * When it is decoded
 * Then it is refused
 */
TEST(PlayerLooks, RefusesAPlayerNumberTwice) {
  EXPECT_THROW(
      static_cast<void>(decodeMessage<PlayerLooks>(byteSequence(
          {0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00}))),
      DuplicatePlayerNumberException);
}

/**
 * Given a payload that announces five players, one more than a game holds
 * When it is decoded
 * Then it is refused before any player is read
 */
TEST(PlayerLooks, RefusesFivePlayers) {
  EXPECT_THROW(
      static_cast<void>(decodeMessage<PlayerLooks>(byteSequence({0x05}))),
      TooManyPlayersException);
}

/**
 * Given a payload whose only player has the face 9, which is a pair of shades
 * When it is decoded
 * Then it is refused
 */
TEST(PlayerLooks, RefusesAnInvalidLook) {
  EXPECT_THROW(static_cast<void>(decodeMessage<PlayerLooks>(
                   byteSequence({0x01, 0x01, 0x00, 0x00, 0x09, 0x00}))),
               InvalidFieldValueException);
}

}  // namespace rtype::network::testing
