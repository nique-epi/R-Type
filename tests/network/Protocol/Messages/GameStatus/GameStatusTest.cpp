#include <gtest/gtest.h>
#include <cstddef>
#include "ByteSequence.hpp"
#include "GameStatus.hpp"
#include "GameStatusTestConstants.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"

namespace rtype::network::testing {

/**
 * Given a game in progress with two players
 * When its status is encoded
 * Then the payload is the state 2, then the number of players 2
 */
TEST(GameStatus, EncodesTheStateThenThePlayerCount) {
  EXPECT_EQ(encodeMessage(GameStatus{.state = GameStatusState::InProgress,
                                     .playerCount = TWO_PLAYERS}),
            byteSequence({0x02, 0x02}));
}

/**
 * Given a status of each state the protocol lists
 * When each is encoded then decoded
 * Then the decoded status equals the original
 */
TEST(GameStatus, DecodesWhatItEncodedForEveryState) {
  for (const GameStatusState state :
       {GameStatusState::WaitingForPlayers, GameStatusState::InProgress,
        GameStatusState::Finished}) {
    const GameStatus status{.state = state, .playerCount = TWO_PLAYERS};

    EXPECT_EQ(decodeMessage<GameStatus>(encodeMessage(status)), status);
  }
}

/**
 * Given a payload whose state byte is 0, which no state uses
 * When it is decoded
 * Then it is refused
 */
TEST(GameStatus, RefusesAStateByteThatNoStateUses) {
  EXPECT_THROW(
      static_cast<void>(decodeMessage<GameStatus>(byteSequence({0x00, 0x01}))),
      InvalidFieldValueException);
}

/**
 * Given a payload whose state byte is 4, the first one above the last state
 * When it is decoded
 * Then it is refused
 */
TEST(GameStatus, RefusesTheFirstStateByteAboveTheLastState) {
  EXPECT_THROW(
      static_cast<void>(decodeMessage<GameStatus>(byteSequence({0x04, 0x01}))),
      InvalidFieldValueException);
}

/**
 * Given a payload that announces five players, one more than a game holds
 * When it is decoded
 * Then it is refused
 */
TEST(GameStatus, RefusesMorePlayersThanAGameHolds) {
  EXPECT_THROW(static_cast<void>(decodeMessage<GameStatus>(
                   byteSequence({0x01, FIVE_PLAYERS}))),
               InvalidFieldValueException);
}

/**
 * Given a status with five players
 * When it is encoded
 * Then it is refused
 */
TEST(GameStatus, RefusesToEncodeMorePlayersThanAGameHolds) {
  EXPECT_THROW(static_cast<void>(encodeMessage(
                   GameStatus{.state = GameStatusState::WaitingForPlayers,
                              .playerCount = FIVE_PLAYERS})),
               InvalidFieldValueException);
}

/**
 * Given a payload of one byte
 * When it is decoded
 * Then it is refused as too short
 */
TEST(GameStatus, RefusesAPayloadOfOneByte) {
  EXPECT_THROW(
      static_cast<void>(decodeMessage<GameStatus>(byteSequence({0x01}))),
      BufferUnderflowException);
}

/**
 * Given a payload of three bytes
 * When it is decoded
 * Then it is refused, since a byte follows the player count
 */
TEST(GameStatus, RefusesAByteAfterThePlayerCount) {
  EXPECT_THROW(static_cast<void>(
                   decodeMessage<GameStatus>(byteSequence({0x01, 0x01, 0x00}))),
               TrailingBytesException);
}

}  // namespace rtype::network::testing
