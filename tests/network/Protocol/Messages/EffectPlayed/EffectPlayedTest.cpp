#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include "ByteSequence.hpp"
#include "ByteWriter.hpp"
#include "EffectPlayed.hpp"
#include "EffectPlayedTestConstants.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"

namespace rtype::network::testing {

/**
 * Given body 9 playing the effect spark
 * When the message is encoded
 * Then the payload is the id, then the name as a short string: its length 5,
 * then its bytes
 */
TEST(EffectPlayed, EncodesTheIdThenTheName) {
  EXPECT_EQ(
      encodeMessage(EffectPlayed{.entityId = EFFECT_BODY_ID,
                                 .effect = std::string{SPARK_EFFECT}}),
      byteSequence({0x00, 0x00, 0x00, 0x09, 0x05, 's', 'p', 'a', 'r', 'k'}));
}

/**
 * Given an effect named with 24 bytes, the longest name of the content
 * When it is encoded then decoded
 * Then the decoded message equals the original
 */
TEST(EffectPlayed, DecodesWhatItEncodedUpToTheLongestName) {
  const EffectPlayed played{.entityId = EFFECT_BODY_ID,
                            .effect = std::string{LONGEST_EFFECT}};

  EXPECT_EQ(decodeMessage<EffectPlayed>(encodeMessage(played)), played);
}

/**
 * Given payloads of the effect spark played by the entity 0, the id no entity
 * has
 * When it is decoded
 * Then it is refused
 */
TEST(EffectPlayed, RefusesTheEntityZero) {
  EXPECT_THROW(static_cast<void>(decodeMessage<EffectPlayed>(byteSequence(
                   {0x00, 0x00, 0x00, 0x00, 0x05, 's', 'p', 'a', 'r', 'k'}))),
               InvalidFieldValueException);
}

/**
 * Given payloads naming an empty effect, an effect of 25 bytes, an effect
 * with a capital letter and an effect with a space
 * When each is decoded
 * Then each is refused as no name of the content
 */
TEST(EffectPlayed, RefusesANameThatIsNoNameOfTheContent) {
  for (const std::string_view name :
       {std::string_view{}, TOO_LONG_EFFECT, EFFECT_WITH_A_CAPITAL,
        EFFECT_WITH_A_SPACE}) {
    SCOPED_TRACE(name);
    ByteWriter writer;
    writer.writeUint32(EFFECT_BODY_ID);
    writer.writeString(name);
    EXPECT_THROW(static_cast<void>(decodeMessage<EffectPlayed>(writer.bytes())),
                 InvalidContentNameException);
  }
}

/**
 * Given an effect named with a capital letter
 * When it is encoded
 * Then it is refused before anything is written
 */
TEST(EffectPlayed, RefusesToEncodeANameThatIsNoNameOfTheContent) {
  EXPECT_THROW(static_cast<void>(encodeMessage(
                   EffectPlayed{.entityId = EFFECT_BODY_ID,
                                .effect = std::string{EFFECT_WITH_A_CAPITAL}})),
               InvalidContentNameException);
}

/**
 * Given a payload whose name announces 5 bytes and holds 4
 * When it is decoded
 * Then it is refused as cut short
 */
TEST(EffectPlayed, RefusesANameCutShort) {
  EXPECT_THROW(static_cast<void>(decodeMessage<EffectPlayed>(byteSequence(
                   {0x00, 0x00, 0x00, 0x09, 0x05, 's', 'p', 'a', 'r'}))),
               BufferUnderflowException);
}

/**
 * Given a payload with one byte after the name
 * When it is decoded
 * Then it is refused
 */
TEST(EffectPlayed, RefusesABytePastTheName) {
  EXPECT_THROW(
      static_cast<void>(decodeMessage<EffectPlayed>(byteSequence(
          {0x00, 0x00, 0x00, 0x09, 0x05, 's', 'p', 'a', 'r', 'k', 0x00}))),
      TrailingBytesException);
}

}  // namespace rtype::network::testing
