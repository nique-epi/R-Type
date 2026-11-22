#include <gtest/gtest.h>
#include <string>
#include "ByteSequence.hpp"
#include "LevelStatus.hpp"
#include "LevelStatusTestConstants.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::network::testing {

/**
 * Given the biome wreck, begun at 96 units and scrolled 240 units at 120
 * units per second
 * When its status is encoded
 * Then the payload is the length 5 and the letters of the biome, then 240,
 * 120 and 96 as big-endian floats, then 2500 ms of transition, 09 C4, then
 * the band from row 192 to row 848, 00 C0 03 50, then the 2 continues left, 02
 */
TEST(LevelStatus,
     EncodesTheBiomeTheScrollTheTransitionTheBandThenTheContinues) {
  EXPECT_EQ(encodeMessage(LevelStatus{.biome = std::string{STATUS_BIOME},
                                      .scrollPosition = STATUS_SCROLL_POSITION,
                                      .scrollSpeed = STATUS_SCROLL_SPEED,
                                      .biomeSince = STATUS_BIOME_SINCE,
                                      .transitionLeft = STATUS_TRANSITION_LEFT,
                                      .bandTop = STATUS_BAND_TOP,
                                      .bandBottom = STATUS_BAND_BOTTOM,
                                      .continuesLeft = STATUS_CONTINUES_LEFT}),
            byteSequence({0x05, 0x77, 0x72, 0x65, 0x63, 0x6B, 0x43, 0x70, 0x00,
                          0x00, 0x42, 0xF0, 0x00, 0x00, 0x42, 0xC0, 0x00, 0x00,
                          0x09, 0xC4, 0x00, 0xC0, 0x03, 0x50, 0x02}));
}

/**
 * Given a status with a biome, a moving scroll, a transition, a band and
 * continues left
 * When it is encoded then decoded
 * Then the decoded status equals the original
 */
TEST(LevelStatus, DecodesWhatItEncoded) {
  const LevelStatus status{.biome = std::string{STATUS_BIOME},
                           .scrollPosition = STATUS_SCROLL_POSITION,
                           .scrollSpeed = STATUS_SCROLL_SPEED,
                           .biomeSince = STATUS_BIOME_SINCE,
                           .transitionLeft = STATUS_TRANSITION_LEFT,
                           .bandTop = STATUS_BAND_TOP,
                           .bandBottom = STATUS_BAND_BOTTOM,
                           .continuesLeft = STATUS_CONTINUES_LEFT};

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
  EXPECT_THROW(static_cast<void>(decodeMessage<LevelStatus>(byteSequence(
                   {0x05, 0x57, 0x72, 0x65, 0x63, 0x6B, 0x43, 0x70, 0x00,
                    0x00, 0x42, 0xF0, 0x00, 0x00, 0x42, 0xC0, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00}))),
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
  EXPECT_THROW(static_cast<void>(decodeMessage<LevelStatus>(byteSequence(
                   {0x05, 0x77, 0x72, 0x65, 0x63, 0x6B, 0x43, 0x70, 0x00,
                    0x00, 0xBF, 0x80, 0x00, 0x00, 0x42, 0xC0, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00}))),
               InvalidFieldValueException);
}

/**
 * Given a payload that ends after the bottom of the band, before the
 * continues left
 * When it is decoded
 * Then it is refused as too short
 */
TEST(LevelStatus, RefusesAPayloadThatEndsTooSoon) {
  EXPECT_THROW(static_cast<void>(decodeMessage<LevelStatus>(byteSequence(
                   {0x05, 0x77, 0x72, 0x65, 0x63, 0x6B, 0x43, 0x70,
                    0x00, 0x00, 0x42, 0xF0, 0x00, 0x00, 0x42, 0xC0,
                    0x00, 0x00, 0x09, 0xC4, 0x00, 0xC0, 0x03, 0x50}))),
               BufferUnderflowException);
}

/**
 * Given a payload with one byte after the continues left
 * When it is decoded
 * Then it is refused for its trailing byte
 */
TEST(LevelStatus, RefusesBytesAfterTheContinuesLeft) {
  EXPECT_THROW(static_cast<void>(decodeMessage<LevelStatus>(byteSequence(
                   {0x05, 0x77, 0x72, 0x65, 0x63, 0x6B, 0x43, 0x70, 0x00,
                    0x00, 0x42, 0xF0, 0x00, 0x00, 0x42, 0xC0, 0x00, 0x00,
                    0x09, 0xC4, 0x00, 0xC0, 0x03, 0x50, 0x02, 0x00}))),
               TrailingBytesException);
}

/**
 * Given a status whose band has its top on its bottom
 * When it is encoded
 * Then it is refused: a band leaves at least a row between them
 */
TEST(LevelStatus, RefusesABandWhoseTopIsItsBottom) {
  EXPECT_THROW(static_cast<void>(encodeMessage(
                   LevelStatus{.biome = {},
                               .bandTop = STATUS_BAND_BOTTOM,
                               .bandBottom = STATUS_BAND_BOTTOM})),
               InvalidFieldValueException);
}

/**
 * Given a payload whose band has its top on row 848, below its bottom on row
 * 192
 * When it is decoded
 * Then it is refused
 */
TEST(LevelStatus, RefusesABandReceivedUpsideDown) {
  EXPECT_THROW(static_cast<void>(decodeMessage<LevelStatus>(byteSequence(
                   {0x05, 0x77, 0x72, 0x65, 0x63, 0x6B, 0x43, 0x70, 0x00,
                    0x00, 0x42, 0xF0, 0x00, 0x00, 0x42, 0xC0, 0x00, 0x00,
                    0x09, 0xC4, 0x03, 0x50, 0x00, 0xC0, 0x00}))),
               InvalidFieldValueException);
}

/**
 * Given a status without a band of its own
 * When it is encoded then decoded
 * Then its band closes nothing: its bottom is OPEN_BAND_BOTTOM
 */
TEST(LevelStatus, ALevelWithoutABandClosesNothing) {
  const LevelStatus status{.biome = {}};

  const auto decoded = decodeMessage<LevelStatus>(encodeMessage(status));

  EXPECT_EQ(decoded.bandTop, 0);
  EXPECT_EQ(decoded.bandBottom, OPEN_BAND_BOTTOM);
}

/**
 * Given a status whose biome began before the scroll, as the biome a level
 * opens in does
 * When it is encoded then decoded
 * Then the decoded status holds that negative position
 */
TEST(LevelStatus, CarriesABiomeThatBeganBeforeTheScroll) {
  const LevelStatus status{.biome = std::string{STATUS_BIOME},
                           .biomeSince = NEGATIVE_SCROLL};

  EXPECT_FLOAT_EQ(decodeMessage<LevelStatus>(encodeMessage(status)).biomeSince,
                  NEGATIVE_SCROLL);
}

/**
 * Given a payload whose biome began at no number, the bytes 7F C0 00 00
 * When it is decoded
 * Then it is refused
 */
TEST(LevelStatus, RefusesAPositionTheBiomeBeganAtThatIsNoNumber) {
  EXPECT_THROW(static_cast<void>(decodeMessage<LevelStatus>(byteSequence(
                   {0x05, 0x77, 0x72, 0x65, 0x63, 0x6B, 0x43, 0x70, 0x00,
                    0x00, 0x42, 0xF0, 0x00, 0x00, 0x7F, 0xC0, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00}))),
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
