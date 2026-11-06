#include <gtest/gtest.h>
#include <string>
#include "ByteSequence.hpp"
#include "ByteWriter.hpp"
#include "ChatLine.hpp"
#include "ChatLineTestConstants.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::network::testing {

/**
 * Given the line 7 "gg"
 * When it is encoded
 * Then the payload is the number on two bytes, then the text after its length:
 * 00 07 02 67 67
 */
TEST(ChatLine, EncodesTheNumberThenTheText) {
  EXPECT_EQ(encodeMessage(ChatLine{.lineNumber = SOME_LINE_NUMBER,
                                   .text = std::string{SOME_TEXT}}),
            byteSequence({0x00, 0x07, 0x02, 0x67, 0x67}));
}

/**
 * Given a line whose text holds MAXIMUM_CHAT_TEXT_SIZE bytes
 * When it is encoded then decoded
 * Then the decoded line equals the original: the longest text goes through
 */
TEST(ChatLine, DecodesWhatItEncodedAtTheLongestText) {
  const ChatLine longest{
      .lineNumber = SOME_LINE_NUMBER,
      .text = std::string(MAXIMUM_CHAT_TEXT_SIZE, TEXT_FILLER)};

  EXPECT_EQ(decodeMessage<ChatLine>(encodeMessage(longest)), longest);
}

/**
 * Given a payload whose text holds one byte more than MAXIMUM_CHAT_TEXT_SIZE
 * When it is decoded
 * Then it is refused
 */
TEST(ChatLine, RefusesATextOneByteTooLong) {
  ByteWriter writer;
  writer.writeUint16(SOME_LINE_NUMBER);
  writer.writeString(std::string(MAXIMUM_CHAT_TEXT_SIZE + 1, TEXT_FILLER));

  EXPECT_THROW(static_cast<void>(decodeMessage<ChatLine>(writer.bytes())),
               InvalidChatTextException);
}

/**
 * Given a payload whose text is empty
 * When it is decoded
 * Then it is refused
 */
TEST(ChatLine, RefusesAnEmptyText) {
  EXPECT_THROW(static_cast<void>(
                   decodeMessage<ChatLine>(byteSequence({0x00, 0x07, 0x00}))),
               InvalidChatTextException);
}

/**
 * Given a payload whose text holds a newline
 * When it is decoded
 * Then it is refused: a line written in a log never starts a line of its own
 */
TEST(ChatLine, RefusesAControlCharacter) {
  EXPECT_THROW(static_cast<void>(decodeMessage<ChatLine>(
                   byteSequence({0x00, 0x07, 0x03, 0x67, 0x0A, 0x67}))),
               InvalidChatTextException);
}

/**
 * Given a payload whose text is the byte 0xFF, never valid in UTF-8
 * When it is decoded
 * Then it is refused
 */
TEST(ChatLine, RefusesATextThatIsNotUtf8) {
  EXPECT_THROW(static_cast<void>(decodeMessage<ChatLine>(
                   byteSequence({0x00, 0x07, 0x01, 0xFF}))),
               InvalidChatTextException);
}

/**
 * Given a line whose text holds a newline
 * When it is encoded
 * Then it is refused and the writer stays empty
 */
TEST(ChatLine, WritesNothingForATextItRefuses) {
  ByteWriter writer;

  EXPECT_THROW(
      encodePayload(writer, ChatLine{.lineNumber = SOME_LINE_NUMBER,
                                     .text = std::string{TEXT_WITH_A_NEWLINE}}),
      InvalidChatTextException);
  EXPECT_TRUE(writer.bytes().empty());
}

/**
 * Given the line 7 "gg" followed by one more byte
 * When it is decoded
 * Then it is refused
 */
TEST(ChatLine, RefusesBytesAfterTheText) {
  EXPECT_THROW(static_cast<void>(decodeMessage<ChatLine>(
                   byteSequence({0x00, 0x07, 0x02, 0x67, 0x67, 0x00}))),
               TrailingBytesException);
}

/**
 * Given a payload whose text announces two bytes and holds one
 * When it is decoded
 * Then it is refused
 */
TEST(ChatLine, RefusesATextCutShort) {
  EXPECT_THROW(static_cast<void>(decodeMessage<ChatLine>(
                   byteSequence({0x00, 0x07, 0x02, 0x67}))),
               BufferUnderflowException);
}

}  // namespace rtype::network::testing
