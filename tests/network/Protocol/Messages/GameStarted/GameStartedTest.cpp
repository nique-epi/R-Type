#include <gtest/gtest.h>
#include <string>
#include "ByteSequence.hpp"
#include "GameStarted.hpp"
#include "GameStartedTestConstants.hpp"
#include "LevelStatusTestConstants.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"

namespace rtype::network::testing {

/**
 * Given the start of a level opening in the biome wreck, its scroll at 240
 * units, the biome begun at -4096 units
 * When it is encoded
 * Then the payload is the length 5 and the letters of the biome, then 240 and
 * -4096 as big-endian floats
 */
TEST(GameStarted, EncodesTheBiomeTheScrollThenWhereTheBiomeBegan) {
  EXPECT_EQ(encodeMessage(GameStarted{.biome = std::string{STATUS_BIOME},
                                      .scrollPosition = STATUS_SCROLL_POSITION,
                                      .biomeSince = STARTED_BIOME_SINCE}),
            byteSequence({0x05, 0x77, 0x72, 0x65, 0x63, 0x6B, 0x43, 0x70, 0x00,
                          0x00, 0xC5, 0x80, 0x00, 0x00}));
}

/**
 * Given the start of a level opening in a biome
 * When it is encoded then decoded
 * Then the decoded start equals the original
 */
TEST(GameStarted, DecodesWhatItEncoded) {
  const GameStarted started{.biome = std::string{STATUS_BIOME},
                            .scrollPosition = STATUS_SCROLL_POSITION,
                            .biomeSince = STARTED_BIOME_SINCE};

  EXPECT_EQ(decodeMessage<GameStarted>(encodeMessage(started)), started);
}

/**
 * Given the start of a game whose level names no biome yet
 * When it is encoded then decoded
 * Then it comes back without a biome
 */
TEST(GameStarted, CarriesAStartWithoutBiome) {
  EXPECT_EQ(decodeMessage<GameStarted>(encodeMessage(GameStarted{})),
            GameStarted{});
}

/**
 * Given a payload whose biome is Wreck, with a capital
 * When it is decoded
 * Then it is refused
 */
TEST(GameStarted, RefusesABiomeThatIsNoBiomeName) {
  EXPECT_THROW(static_cast<void>(decodeMessage<GameStarted>(
                   byteSequence({0x05, 0x57, 0x72, 0x65, 0x63, 0x6B, 0x43, 0x70,
                                 0x00, 0x00, 0xC5, 0x80, 0x00, 0x00}))),
               InvalidFieldValueException);
}

/**
 * Given a start whose biome holds a capital
 * When it is encoded
 * Then it is refused
 */
TEST(GameStarted, RefusesToWriteABiomeThatIsNoBiomeName) {
  EXPECT_THROW(static_cast<void>(encodeMessage(
                   GameStarted{.biome = std::string{CAPITALIZED_BIOME}})),
               InvalidFieldValueException);
}

/**
 * Given a start whose scroll stands behind the start of the level
 * When it is encoded
 * Then it is refused
 */
TEST(GameStarted, RefusesAScrollBehindTheStart) {
  EXPECT_THROW(static_cast<void>(encodeMessage(GameStarted{
                   .biome = {}, .scrollPosition = NEGATIVE_SCROLL})),
               InvalidFieldValueException);
}

/**
 * Given a start whose biome began at a position that is no number
 * When it is encoded
 * Then it is refused
 */
TEST(GameStarted, RefusesABiomeBegunAtNoNumber) {
  EXPECT_THROW(static_cast<void>(encodeMessage(GameStarted{
                   .biome = {}, .biomeSince = SCROLL_THAT_IS_NO_NUMBER})),
               InvalidFieldValueException);
}

/**
 * Given a payload that ends one byte before where the biome began
 * When it is decoded
 * Then a BufferUnderflowException is raised
 */
TEST(GameStarted, RefusesAPayloadCutBeforeItsEnd) {
  EXPECT_THROW(static_cast<void>(decodeMessage<GameStarted>(
                   byteSequence({0x05, 0x77, 0x72, 0x65, 0x63, 0x6B, 0x43, 0x70,
                                 0x00, 0x00, 0xC5, 0x80, 0x00}))),
               BufferUnderflowException);
}

/**
 * Given a payload with one byte after where the biome began
 * When it is decoded
 * Then a TrailingBytesException is raised
 */
TEST(GameStarted, RefusesAByteAfterItsEnd) {
  EXPECT_THROW(static_cast<void>(decodeMessage<GameStarted>(
                   byteSequence({0x05, 0x77, 0x72, 0x65, 0x63, 0x6B, 0x43, 0x70,
                                 0x00, 0x00, 0xC5, 0x80, 0x00, 0x00, 0x00}))),
               TrailingBytesException);
}

}  // namespace rtype::network::testing
