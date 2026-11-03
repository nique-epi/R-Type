#include <gtest/gtest.h>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>
#include "ByteSequence.hpp"
#include "LeaderboardAnswer.hpp"
#include "LeaderboardMessagesTestConstants.hpp"
#include "LeaderboardRequest.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"
#include "ProtocolTestConstants.hpp"

namespace rtype::network::testing {

/**
 * Given a request for two players at the normal difficulty
 * When it is encoded
 * Then the payload is the count, then the length and the bytes of the name, as
 * the protocol page writes them
 */
TEST(LeaderboardRequest, EncodesItsFieldsInTheOrderOfTheProtocolPage) {
  const auto expected =
      byteSequence({0x02, 0x06, 'n', 'o', 'r', 'm', 'a', 'l'});

  EXPECT_EQ(encodeMessage(
                LeaderboardRequest{.playerCount = 2, .difficulty = "normal"}),
            expected);
}

/**
 * Given the counts on both sides of each bound
 * When a request is decoded
 * Then 0 and 5 are refused, 1 and 4 are read
 */
TEST(LeaderboardRequest, AcceptsOneToFourPlayersOnly) {
  EXPECT_THROW(
      (void)decodeMessage<LeaderboardRequest>(byteSequence({0x00, 0x01, 'n'})),
      InvalidFieldValueException);
  EXPECT_EQ(decodeMessage<LeaderboardRequest>(byteSequence({0x01, 0x01, 'n'}))
                .playerCount,
            1);
  EXPECT_EQ(decodeMessage<LeaderboardRequest>(byteSequence({0x04, 0x01, 'n'}))
                .playerCount,
            4);
  EXPECT_THROW(
      (void)decodeMessage<LeaderboardRequest>(byteSequence({0x05, 0x01, 'n'})),
      InvalidFieldValueException);
}

/**
 * Given a difficulty of 0, 16 and 17 bytes
 * When a request is encoded
 * Then the longest allowed passes, the two others are refused
 */
TEST(LeaderboardRequest, BoundsTheSizeOfTheDifficulty) {
  EXPECT_THROW((void)encodeMessage(
                   LeaderboardRequest{.playerCount = 1, .difficulty = ""}),
               InvalidDifficultyException);
  EXPECT_NO_THROW((void)encodeMessage(LeaderboardRequest{
      .playerCount = 1, .difficulty = std::string{SIXTEEN_BYTE_NICKNAME}}));
  EXPECT_THROW((void)encodeMessage(LeaderboardRequest{
                   .playerCount = 1,
                   .difficulty = std::string{SEVENTEEN_BYTE_NICKNAME}}),
               InvalidDifficultyException);
}

/**
 * Given a payload cut in the name, and one with a byte after it
 * When each is decoded
 * Then the first is refused as short, the second for its trailing byte
 */
TEST(LeaderboardRequest, RefusesAPayloadOfTheWrongSize) {
  EXPECT_THROW(
      (void)decodeMessage<LeaderboardRequest>(byteSequence({0x01, 0x02, 'n'})),
      BufferUnderflowException);
  EXPECT_THROW((void)decodeMessage<LeaderboardRequest>(
                   byteSequence({0x01, 0x01, 'n', 0x00})),
               TrailingBytesException);
}

/**
 * Given an answer with one row and no own row
 * When it is encoded
 * Then the bytes are those of the protocol page
 */
TEST(LeaderboardAnswer, EncodesARowInTheOrderOfTheProtocolPage) {
  const LeaderboardAnswer answer{
      .asked = {.playerCount = 1, .difficulty = "n"},
      .rows = {LeaderboardRow{.rank = SOME_RANK,
                              .score = SOME_LEADERBOARD_SCORE,
                              .playedAt = SOME_PLAYED_AT,
                              .nickname = "A"}},
      .ownRow = std::nullopt};
  const auto expected = byteSequence(
      {0x01, 0x01, 'n',  0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x01, 0xF4,
       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 'A',  0x00});

  EXPECT_EQ(encodeMessage(answer), expected);
}

/**
 * Given the largest answer the protocol allows
 * When it is encoded, then decoded
 * Then it has the size the protocol page adds up to, and comes back the same
 */
TEST(LeaderboardAnswer, TheLargestOneFitsAndComesBackTheSame) {
  const LeaderboardRow row{.rank = SOME_RANK,
                           .score = SOME_LEADERBOARD_SCORE,
                           .playedAt = SOME_PLAYED_AT,
                           .nickname = std::string{SIXTEEN_BYTE_NICKNAME}};
  const LeaderboardAnswer answer{
      .asked = {.playerCount = 4,
                .difficulty = std::string{SIXTEEN_BYTE_NICKNAME}},
      .rows = std::vector<LeaderboardRow>(MAXIMUM_LEADERBOARD_ROWS, row),
      .ownRow = row};

  const std::vector<std::byte> payload = encodeMessage(answer);

  EXPECT_EQ(payload.size(), LARGEST_ANSWER_ON_THE_PROTOCOL_PAGE);
  EXPECT_EQ(decodeMessage<LeaderboardAnswer>(payload), answer);
}

/**
 * Given an answer that announces eleven rows and holds none
 * When it is decoded
 * Then the count is refused before any row is read
 */
TEST(LeaderboardAnswer, RefusesTooManyRowsBeforeReadingThem) {
  EXPECT_THROW((void)decodeMessage<LeaderboardAnswer>(
                   byteSequence({0x01, 0x01, 'n', TOO_MANY_ROWS})),
               TooManyLeaderboardRowsException);
}

/**
 * Given an own row of rank 0
 * When the answer is encoded
 * Then it is refused
 */
TEST(LeaderboardAnswer, RefusesARankOfZeroInTheOwnRow) {
  const LeaderboardAnswer answer{
      .asked = {.playerCount = 1, .difficulty = "n"},
      .rows = {},
      .ownRow = LeaderboardRow{
          .rank = 0, .score = 0, .playedAt = 0, .nickname = "A"}};

  EXPECT_THROW((void)encodeMessage(answer), InvalidFieldValueException);
}

/**
 * Given a stored row with an empty nickname
 * When the answer is encoded
 * Then it is refused, so it never reaches a client
 */
TEST(LeaderboardAnswer, RefusesARowWithAnInvalidNickname) {
  const LeaderboardAnswer answer{
      .asked = {.playerCount = 1, .difficulty = "n"},
      .rows = {LeaderboardRow{
          .rank = 1, .score = 0, .playedAt = 0, .nickname = ""}},
      .ownRow = std::nullopt};

  EXPECT_THROW((void)encodeMessage(answer), InvalidNicknameException);
}

/**
 * Given an answer with no row and a flag that is not a bool
 * When it is decoded
 * Then it is refused
 */
TEST(LeaderboardAnswer, RefusesAnOwnRowFlagThatIsNotABool) {
  EXPECT_THROW((void)decodeMessage<LeaderboardAnswer>(
                   byteSequence({0x01, 0x01, 'n', 0x00, 0x02})),
               InvalidBoolException);
}

/**
 * Given an answer with no row, a byte after its flag, and one without a flag
 * When each is decoded
 * Then the first is refused for its trailing byte, the second as short
 */
TEST(LeaderboardAnswer, RefusesAPayloadOfTheWrongSize) {
  EXPECT_THROW((void)decodeMessage<LeaderboardAnswer>(
                   byteSequence({0x01, 0x01, 'n', 0x00, 0x00, 0x00})),
               TrailingBytesException);
  EXPECT_THROW((void)decodeMessage<LeaderboardAnswer>(
                   byteSequence({0x01, 0x01, 'n', 0x00})),
               BufferUnderflowException);
}

}  // namespace rtype::network::testing
