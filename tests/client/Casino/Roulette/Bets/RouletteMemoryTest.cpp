#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <optional>
#include "BetsReceipt.hpp"
#include "RouletteMemory.hpp"
#include "RouletteMemoryFixtures.hpp"
#include "RouletteMemoryTestConstants.hpp"

using rtype::client::RouletteMemory;
using rtype::client::testing::A_CHIP_ON_RED;
using rtype::client::testing::A_CHIP_ON_SEVENTEEN;
using rtype::client::testing::CHIPS_LEFT;
using rtype::client::testing::CHIPS_OF_THE_PLAYER;
using rtype::client::testing::FIRST_ANSWER;
using rtype::client::testing::FIRST_ROUND;
using rtype::client::testing::NEXT_ANSWER;
using rtype::client::testing::NEXT_ROUND;
using rtype::client::testing::refusedBets;
using rtype::client::testing::roomAtRound;
using rtype::client::testing::takenBets;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given a chip on 17 in a round
 * When the next round opens
 * Then the slip is empty, and the chip is among the bets of the round before
 */
TEST(RouletteMemory, ANewRoundPutsTheBetsAside) {
  RouletteMemory memory;
  memory.follow(roomAtRound(FIRST_ROUND), std::nullopt);
  ASSERT_TRUE(memory.slip().lay(A_CHIP_ON_SEVENTEEN, CHIPS_OF_THE_PLAYER));

  memory.follow(roomAtRound(NEXT_ROUND), std::nullopt);

  EXPECT_THAT(memory.slip().bets(), IsEmpty());
  EXPECT_THAT(memory.previousBets(), ElementsAre(A_CHIP_ON_SEVENTEEN));
}

/**
 * Given a round that laid a chip, then a round that laid none
 * When the round after opens
 * Then the bets of the round before are still the chip: REBET lays the
 * last bets the player laid
 */
TEST(RouletteMemory, ARoundWithoutBetsKeepsTheBetsBefore) {
  RouletteMemory memory;
  memory.follow(roomAtRound(FIRST_ROUND), std::nullopt);
  ASSERT_TRUE(memory.slip().lay(A_CHIP_ON_SEVENTEEN, CHIPS_OF_THE_PLAYER));
  memory.follow(roomAtRound(NEXT_ROUND), std::nullopt);

  memory.follow(roomAtRound(NEXT_ROUND + 1), std::nullopt);

  EXPECT_THAT(memory.previousBets(), ElementsAre(A_CHIP_ON_SEVENTEEN));
}

/**
 * Given a chip on the slip
 * When the room is left
 * Then the slip is empty
 */
TEST(RouletteMemory, NoRoomDropsTheBets) {
  RouletteMemory memory;
  memory.follow(roomAtRound(FIRST_ROUND), std::nullopt);
  ASSERT_TRUE(memory.slip().lay(A_CHIP_ON_SEVENTEEN, CHIPS_OF_THE_PLAYER));

  memory.follow(std::nullopt, std::nullopt);

  EXPECT_THAT(memory.slip().bets(), IsEmpty());
}

/**
 * Given a round in progress
 * When the master answers it took a chip on 17
 * Then the memory keeps its receipt, the chips left included
 */
TEST(RouletteMemory, KeepsTheReceiptOfTheBetsTaken) {
  RouletteMemory memory;

  memory.follow(roomAtRound(FIRST_ROUND),
                takenBets(FIRST_ANSWER, FIRST_ROUND, {A_CHIP_ON_SEVENTEEN}));

  ASSERT_TRUE(memory.accepted().has_value());
  EXPECT_EQ(memory.accepted().value_or(rtype::network::BetsReceipt{}).chips,
            CHIPS_LEFT);
}

/**
 * Given a chip on 17 the master took, then a chip on red laid
 * When the master refuses the bets with the chip on red
 * Then the slip holds the chip on 17 alone, as the master does
 */
TEST(RouletteMemory, RefusedBetsLeaveTheBetsTakenOnTheCloth) {
  RouletteMemory memory;
  memory.follow(roomAtRound(FIRST_ROUND), std::nullopt);
  ASSERT_TRUE(memory.slip().lay(A_CHIP_ON_SEVENTEEN, CHIPS_OF_THE_PLAYER));
  memory.follow(roomAtRound(FIRST_ROUND),
                takenBets(FIRST_ANSWER, FIRST_ROUND, {A_CHIP_ON_SEVENTEEN}));
  ASSERT_TRUE(memory.slip().lay(A_CHIP_ON_RED, CHIPS_OF_THE_PLAYER));

  memory.follow(roomAtRound(FIRST_ROUND),
                refusedBets(NEXT_ANSWER, FIRST_ROUND));

  EXPECT_THAT(memory.slip().bets(), ElementsAre(A_CHIP_ON_SEVENTEEN));
}

/**
 * Given a refusal already read, then a chip laid again
 * When the same refusal is read once more
 * Then the chip stays: an answer is read once
 */
TEST(RouletteMemory, AnAnswerIsReadOnce) {
  RouletteMemory memory;
  memory.follow(roomAtRound(FIRST_ROUND),
                refusedBets(FIRST_ANSWER, FIRST_ROUND));
  ASSERT_TRUE(memory.slip().lay(A_CHIP_ON_SEVENTEEN, CHIPS_OF_THE_PLAYER));

  memory.follow(roomAtRound(FIRST_ROUND),
                refusedBets(FIRST_ANSWER, FIRST_ROUND));

  EXPECT_THAT(memory.slip().bets(), ElementsAre(A_CHIP_ON_SEVENTEEN));
}

/**
 * Given a chip laid on the next round
 * When a refusal of the round before arrives late
 * Then the chip stays
 */
TEST(RouletteMemory, AnAnswerOfAnotherRoundIsIgnored) {
  RouletteMemory memory;
  memory.follow(roomAtRound(NEXT_ROUND), std::nullopt);
  ASSERT_TRUE(memory.slip().lay(A_CHIP_ON_SEVENTEEN, CHIPS_OF_THE_PLAYER));

  memory.follow(roomAtRound(NEXT_ROUND),
                refusedBets(FIRST_ANSWER, FIRST_ROUND));

  EXPECT_THAT(memory.slip().bets(), ElementsAre(A_CHIP_ON_SEVENTEEN));
}
