#include <gtest/gtest.h>
#include <string>
#include "ByteSequence.hpp"
#include "ChatLine.hpp"
#include "ChatLineTestConstants.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"
#include "PlayerChatLine.hpp"
#include "PlayerChatLineTestConstants.hpp"

namespace rtype::network::testing {

/**
 * Given the line 7 "gg" of the second player
 * When it is encoded
 * Then the payload is the line, then the player number: 00 07 02 67 67 02
 */
TEST(PlayerChatLine, EncodesTheLineThenThePlayerNumber) {
  EXPECT_EQ(
      encodeMessage(PlayerChatLine{.line = {.lineNumber = SOME_LINE_NUMBER,
                                            .text = std::string{SOME_TEXT}},
                                   .playerNumber = SECOND_PLAYER_NUMBER}),
      byteSequence({0x00, 0x07, 0x02, 0x67, 0x67, 0x02}));
}

/**
 * Given the line of the fourth player, the last a game holds
 * When it is encoded then decoded
 * Then the decoded line equals the original
 */
TEST(PlayerChatLine, DecodesWhatItEncoded) {
  const PlayerChatLine relayed{
      .line = {.lineNumber = SOME_LINE_NUMBER, .text = std::string{SOME_TEXT}},
      .playerNumber = LAST_PLAYER_NUMBER};

  EXPECT_EQ(decodeMessage<PlayerChatLine>(encodeMessage(relayed)), relayed);
}

/**
 * Given a payload whose player number is 0, which no player has
 * When it is decoded
 * Then it is refused
 */
TEST(PlayerChatLine, RefusesThePlayerNumberZero) {
  EXPECT_THROW(static_cast<void>(decodeMessage<PlayerChatLine>(
                   byteSequence({0x00, 0x07, 0x02, 0x67, 0x67, 0x00}))),
               InvalidFieldValueException);
}

/**
 * Given a payload whose player number is 5, one more than a game holds
 * When it is decoded
 * Then it is refused
 */
TEST(PlayerChatLine, RefusesAPlayerNumberAboveTheLast) {
  EXPECT_THROW(static_cast<void>(decodeMessage<PlayerChatLine>(
                   byteSequence({0x00, 0x07, 0x02, 0x67, 0x67, 0x05}))),
               InvalidFieldValueException);
}

/**
 * Given a payload whose text holds a newline
 * When it is decoded
 * Then it is refused
 */
TEST(PlayerChatLine, RefusesAControlCharacter) {
  EXPECT_THROW(static_cast<void>(decodeMessage<PlayerChatLine>(
                   byteSequence({0x00, 0x07, 0x03, 0x67, 0x0A, 0x67, 0x02}))),
               InvalidChatTextException);
}

/**
 * Given a payload that stops after the text
 * When it is decoded
 * Then it is refused
 */
TEST(PlayerChatLine, RefusesAPayloadWithoutThePlayerNumber) {
  EXPECT_THROW(static_cast<void>(decodeMessage<PlayerChatLine>(
                   byteSequence({0x00, 0x07, 0x02, 0x67, 0x67}))),
               BufferUnderflowException);
}

/**
 * Given a whole payload followed by one more byte
 * When it is decoded
 * Then it is refused
 */
TEST(PlayerChatLine, RefusesBytesAfterThePlayerNumber) {
  EXPECT_THROW(static_cast<void>(decodeMessage<PlayerChatLine>(
                   byteSequence({0x00, 0x07, 0x02, 0x67, 0x67, 0x02, 0x00}))),
               TrailingBytesException);
}

}  // namespace rtype::network::testing
