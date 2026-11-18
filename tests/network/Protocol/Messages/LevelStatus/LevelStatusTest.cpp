#include <gtest/gtest.h>
#include <string>
#include "ByteSequence.hpp"
#include "LevelStatus.hpp"
#include "LevelStatusTestConstants.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"

namespace rtype::network::testing {

/**
 * Given the biome wreck, begun at 96 units and scrolled 240 units at 120
 * units per second
 * When its status is encoded
 * Then the payload is the length 5 and the letters of the biome, then 240,
 * 120 and 96 as big-endian floats, then 2500 ms of transition, 09 C4
 */
TEST(LevelStatus, EncodesTheBiomeThenTheScrollThenTheTransition) {
  EXPECT_EQ(
      encodeMessage(LevelStatus{.biome = std::string{STATUS_BIOME},
                                .scrollPosition = STATUS_SCROLL_POSITION,
                                .scrollSpeed = STATUS_SCROLL_SPEED,
                                .biomeSince = STATUS_BIOME_SINCE,
                                .transitionLeft = STATUS_TRANSITION_LEFT}),
      byteSequence({0x05, 0x77, 0x72, 0x65, 0x63, 0x6B, 0x43,
                    0x70, 0x00, 0x00, 0x42, 0xF0, 0x00, 0x00,
                    0x42, 0xC0, 0x00, 0x00, 0x09, 0xC4}));
}

/**
 * Given a status with a biome, a moving scroll and a transition
 * When it is encoded then decoded
 * Then the decoded status equals the original
 */
TEST(LevelStatus, DecodesWhatItEncoded) {
  const LevelStatus status{.biome = std::string{STATUS_BIOME},
                           .scrollPosition = STATUS_SCROLL_POSITION,
                           .scrollSpeed = STATUS_SCROLL_SPEED,
                           .biomeSince = STATUS_BIOME_SINCE,
                           .transitionLeft = STATUS_TRANSITION_LEFT};

  EXPECT_EQ(decodeMessage<LevelStatus>(encodeMessage(status)), status);
}

/**
 * Given a status without a biome, its scroll paused at the start
 * When it is encoded then decoded
 * Then the decoded status equals the original
 */
TEST(LevelStatus, CarriesALevelWithoutBiomeWhoseScrollIsPaused) {
  const LevelStatus status{.biome = {}, .scrollSpeed = PAUSED_SCROLL_SPEED};

  EXPECT_EQ(decodeMessage<LevelStatus>(encodeMessage(status)), status);
}

/**
 * Given a payload whose biome is Wreck, with a capital
 * When it is decoded
 * Then it is refused
 */
TEST(LevelStatus, RefusesABiomeThatIsNoBiomeName) {
  EXPECT_THROW(
      static_cast<void>(decodeMessage<LevelStatus>(byteSequence(
          {0x05, 0x57, 0x72, 0x65, 0x63, 0x6B, 0x43, 0x70, 0x00, 0x00,
           0x42, 0xF0, 0x00, 0x00, 0x42, 0xC0, 0x00, 0x00, 0x00, 0x00}))),
      InvalidFieldValueException);
}

/**
 * Given a status whose biome holds a capital
 * When it is encoded
 * Then it is refused
 */
TEST(LevelStatus, RefusesToEncodeABiomeThatIsNoBiomeName) {
  EXPECT_THROW(static_cast<void>(encodeMessage(
                   LevelStatus{.biome = std::string{CAPITALIZED_BIOME}})),
               InvalidFieldValueException);
}

/**
 * Given a status whose scroll position is negative
 * When it is encoded
 * Then it is refused
 */
TEST(LevelStatus, RefusesANegativeScrollPosition) {
  EXPECT_THROW(static_cast<void>(encodeMessage(LevelStatus{
                   .biome = {}, .scrollPosition = NEGATIVE_SCROLL})),
               InvalidFieldValueException);
}

/**
 * Given a status whose scroll speed is not a number
 * When it is encoded
 * Then it is refused
 */
TEST(LevelStatus, RefusesAScrollSpeedThatIsNoNumber) {
  EXPECT_THROW(static_cast<void>(encodeMessage(LevelStatus{
                   .biome = {}, .scrollSpeed = SCROLL_THAT_IS_NO_NUMBER})),
               InvalidFieldValueException);
}

/**
 * Given a payload whose scroll speed is -1, the bytes BF 80 00 00
 * When it is decoded
 * Then it is refused
 */
TEST(LevelStatus, RefusesANegativeScrollSpeedReceived) {
  EXPECT_THROW(
      static_cast<void>(decodeMessage<LevelStatus>(byteSequence(
          {0x05, 0x77, 0x72, 0x65, 0x63, 0x6B, 0x43, 0x70, 0x00, 0x00,
           0xBF, 0x80, 0x00, 0x00, 0x42, 0xC0, 0x00, 0x00, 0x00, 0x00}))),
      InvalidFieldValueException);
}

/**
 * Given a payload that ends one byte before the end of the time left of the
 * transition
 * When it is decoded
 * Then it is refused as too short
 */
TEST(LevelStatus, RefusesAPayloadThatEndsTooSoon) {
  EXPECT_THROW(static_cast<void>(decodeMessage<LevelStatus>(byteSequence(
                   {0x05, 0x77, 0x72, 0x65, 0x63, 0x6B, 0x43, 0x70, 0x00, 0x00,
                    0x42, 0xF0, 0x00, 0x00, 0x42, 0xC0, 0x00, 0x00, 0x09}))),
               BufferUnderflowException);
}

/**
 * Given a payload with one byte after the time left of the transition
 * When it is decoded
 * Then it is refused for its trailing byte
 */
TEST(LevelStatus, RefusesBytesAfterTheTimeLeftOfTheTransition) {
  EXPECT_THROW(
      static_cast<void>(decodeMessage<LevelStatus>(byteSequence(
          {0x05, 0x77, 0x72, 0x65, 0x63, 0x6B, 0x43, 0x70, 0x00, 0x00, 0x42,
           0xF0, 0x00, 0x00, 0x42, 0xC0, 0x00, 0x00, 0x09, 0xC4, 0x00}))),
      TrailingBytesException);
}

/**
 * Given a status whose biome began at a negative position
 * When it is encoded
 * Then it is refused
 */
TEST(LevelStatus, RefusesANegativePositionTheBiomeBeganAt) {
  EXPECT_THROW(static_cast<void>(encodeMessage(
                   LevelStatus{.biome = {}, .biomeSince = NEGATIVE_SCROLL})),
               InvalidFieldValueException);
}

/**
 * Given a payload whose biome began at no number, the bytes 7F C0 00 00
 * When it is decoded
 * Then it is refused
 */
TEST(LevelStatus, RefusesAPositionTheBiomeBeganAtThatIsNoNumber) {
  EXPECT_THROW(
      static_cast<void>(decodeMessage<LevelStatus>(byteSequence(
          {0x05, 0x77, 0x72, 0x65, 0x63, 0x6B, 0x43, 0x70, 0x00, 0x00,
           0x42, 0xF0, 0x00, 0x00, 0x7F, 0xC0, 0x00, 0x00, 0x00, 0x00}))),
      InvalidFieldValueException);
}

/**
 * Given a status playing the longest transition a status carries, 65535 ms
 * When it is encoded then decoded
 * Then the decoded status holds that time left
 */
TEST(LevelStatus, CarriesTheLongestTransition) {
  const LevelStatus status{.biome = {},
                           .transitionLeft = LONGEST_TRANSITION_LEFT};

  EXPECT_EQ(decodeMessage<LevelStatus>(encodeMessage(status)).transitionLeft,
            LONGEST_TRANSITION_LEFT);
}

}  // namespace rtype::network::testing
