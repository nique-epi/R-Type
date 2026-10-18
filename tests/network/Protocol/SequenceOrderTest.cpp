#include <gtest/gtest.h>
#include <cstdint>
#include "SequenceOrder.hpp"

namespace rtype::network {
namespace {

constexpr std::uint16_t SOME_SEQUENCE = 1000;
constexpr std::uint16_t LARGEST_SEQUENCE = 65535;
constexpr std::uint16_t ONE_AHEAD = 1001;
constexpr std::uint16_t ONE_BEHIND = 999;
constexpr std::uint16_t LARGEST_DISTANCE_AHEAD = 33767;
constexpr std::uint16_t HALF_RANGE_AHEAD = 33768;
constexpr std::uint16_t JUST_PAST_HALF_RANGE_AHEAD = 33769;

/**
 * Given the newest sequence accepted
 * When the same sequence arrives again
 * Then it is not newer
 */
TEST(SequenceOrder, ADuplicateIsNotNewer) {
  EXPECT_FALSE(isNewerSequence(SOME_SEQUENCE, SOME_SEQUENCE));
}

/**
 * Given the newest sequence accepted
 * When a sequence one ahead arrives
 * Then it is newer
 */
TEST(SequenceOrder, OneAheadIsNewer) {
  EXPECT_TRUE(isNewerSequence(ONE_AHEAD, SOME_SEQUENCE));
}

/**
 * Given the newest sequence accepted
 * When a sequence 32767 ahead arrives
 * Then it is newer
 */
TEST(SequenceOrder, ThirtyTwoThousandSevenHundredSixtySevenAheadIsNewer) {
  EXPECT_TRUE(isNewerSequence(LARGEST_DISTANCE_AHEAD, SOME_SEQUENCE));
}

/**
 * Given the newest sequence accepted
 * When a sequence exactly 32768 ahead arrives
 * Then it is not newer
 */
TEST(SequenceOrder, ExactlyHalfTheRangeAheadIsNotNewer) {
  EXPECT_FALSE(isNewerSequence(HALF_RANGE_AHEAD, SOME_SEQUENCE));
}

/**
 * Given the newest sequence accepted
 * When a sequence 32769 ahead, which is 32767 behind, arrives
 * Then it is not newer
 */
TEST(SequenceOrder, MoreThanHalfTheRangeAheadIsNotNewer) {
  EXPECT_FALSE(isNewerSequence(JUST_PAST_HALF_RANGE_AHEAD, SOME_SEQUENCE));
}

/**
 * Given the newest sequence accepted
 * When the sequence just before it arrives late
 * Then it is not newer
 */
TEST(SequenceOrder, OneBehindIsNotNewer) {
  EXPECT_FALSE(isNewerSequence(ONE_BEHIND, SOME_SEQUENCE));
}

/**
 * Given the newest sequence accepted is 65535
 * When sequence 0 arrives
 * Then it is newer, across the wrap
 */
TEST(SequenceOrder, ZeroIsNewerThanTheLargestSequence) {
  EXPECT_TRUE(isNewerSequence(0, LARGEST_SEQUENCE));
}

/**
 * Given the newest sequence accepted is 0
 * When sequence 65535 arrives
 * Then it is not newer
 */
TEST(SequenceOrder, TheLargestSequenceIsNotNewerThanZero) {
  EXPECT_FALSE(isNewerSequence(LARGEST_SEQUENCE, 0));
}

}  // namespace
}  // namespace rtype::network
