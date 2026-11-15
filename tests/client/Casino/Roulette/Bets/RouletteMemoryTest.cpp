#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include "RouletteMemory.hpp"
#include "RouletteMemoryFixtures.hpp"
#include "RouletteMemoryTestConstants.hpp"
#include "RouletteRound.hpp"

using rtype::client::RouletteMemory;
using rtype::client::testing::A_CHIP_ON_SEVENTEEN;
using rtype::client::testing::CHIPS_OF_THE_PLAYER;
using rtype::client::testing::CHIPS_ONCE_LAID;
using rtype::client::testing::COPIES_OF_A_RESULT;
using rtype::client::testing::FIRST_ROUND;
using rtype::client::testing::LAST_SEVEN_NEWEST_FIRST;
using rtype::client::testing::NEXT_ROUND;
using rtype::client::testing::NUMBERS_DRAWN;
using rtype::client::testing::roundAt;
using rtype::client::testing::SEVENTEEN_DRAWN;
using rtype::network::RoulettePhase;
using ::testing::ElementsAre;
using ::testing::ElementsAreArray;
using ::testing::IsEmpty;

/**
 * Given a chip on 17 in a round, and the spin asked
 * When the next round opens
 * Then the slip is empty, the spin no longer asked, and the chip is among
 * the bets of the round before
 */
TEST(RouletteMemory, ANewRoundPutsTheBetsAside) {
  RouletteMemory memory;
  memory.follow(roundAt(FIRST_ROUND, RoulettePhase::Betting, 0),
                CHIPS_OF_THE_PLAYER);
  ASSERT_TRUE(memory.slip().lay(A_CHIP_ON_SEVENTEEN, CHIPS_OF_THE_PLAYER));
  memory.askSpin();

  memory.follow(roundAt(NEXT_ROUND, RoulettePhase::Betting, 0),
                CHIPS_OF_THE_PLAYER);

  EXPECT_THAT(memory.slip().bets(), IsEmpty());
  EXPECT_FALSE(memory.spinAsked());
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
  memory.follow(roundAt(FIRST_ROUND, RoulettePhase::Betting, 0),
                CHIPS_OF_THE_PLAYER);
  ASSERT_TRUE(memory.slip().lay(A_CHIP_ON_SEVENTEEN, CHIPS_OF_THE_PLAYER));
  memory.follow(roundAt(NEXT_ROUND, RoulettePhase::Betting, 0),
                CHIPS_OF_THE_PLAYER);

  memory.follow(roundAt(NEXT_ROUND + 1, RoulettePhase::Betting, 0),
                CHIPS_OF_THE_PLAYER);

  EXPECT_THAT(memory.previousBets(), ElementsAre(A_CHIP_ON_SEVENTEEN));
}

/**
 * Given a round at its result, seen three times
 * When the numbers drawn are read
 * Then its number is there once
 */
TEST(RouletteMemory, AResultCountsOnce) {
  RouletteMemory memory;

  for (int copy = 0; copy < COPIES_OF_A_RESULT; ++copy) {
    memory.follow(roundAt(FIRST_ROUND, RoulettePhase::Result, SEVENTEEN_DRAWN),
                  CHIPS_OF_THE_PLAYER);
  }

  EXPECT_THAT(memory.lastNumbers(), ElementsAre(SEVENTEEN_DRAWN));
}

/**
 * Given eight rounds drawn one after another
 * When the numbers drawn are read
 * Then the last seven show, the newest first
 */
TEST(RouletteMemory, KeepsTheLastSevenNumbersNewestFirst) {
  RouletteMemory memory;

  for (std::size_t round = 0; round < NUMBERS_DRAWN.size(); ++round) {
    memory.follow(roundAt(static_cast<std::uint16_t>(FIRST_ROUND + round),
                          RoulettePhase::Result, NUMBERS_DRAWN.at(round)),
                  CHIPS_OF_THE_PLAYER);
  }

  EXPECT_THAT(memory.lastNumbers(), ElementsAreArray(LAST_SEVEN_NEWEST_FIRST));
}

/**
 * Given a round whose number is drawn while the wheel turns
 * When it is followed while the wheel turns
 * Then its number is not among the numbers drawn yet
 */
TEST(RouletteMemory, TheWheelTurningKeepsTheNumberSecret) {
  RouletteMemory memory;

  memory.follow(roundAt(FIRST_ROUND, RoulettePhase::Spinning, SEVENTEEN_DRAWN),
                CHIPS_OF_THE_PLAYER);

  EXPECT_THAT(memory.lastNumbers(), IsEmpty());
}

/**
 * Given a chip of 25 laid while the bets are open
 * When the wheel turns, the settings showing other chips
 * Then the chips held before the draw stay those once the chip was laid
 */
TEST(RouletteMemory, KeepsTheChipsHeldWhenTheBetsClosed) {
  RouletteMemory memory;
  memory.follow(roundAt(FIRST_ROUND, RoulettePhase::Betting, 0),
                CHIPS_OF_THE_PLAYER);
  ASSERT_TRUE(memory.slip().lay(A_CHIP_ON_SEVENTEEN, CHIPS_OF_THE_PLAYER));
  memory.follow(roundAt(FIRST_ROUND, RoulettePhase::Betting, 0),
                CHIPS_OF_THE_PLAYER);

  memory.follow(roundAt(FIRST_ROUND, RoulettePhase::Spinning, SEVENTEEN_DRAWN),
                0);

  EXPECT_EQ(memory.chipsBeforeDraw(), CHIPS_ONCE_LAID);
}

/**
 * Given a chip laid and the spin asked
 * When the table goes away, the session closed
 * Then the slip is empty and the spin no longer asked
 */
TEST(RouletteMemory, NoTableDropsTheBetsOfTheRound) {
  RouletteMemory memory;
  memory.follow(roundAt(FIRST_ROUND, RoulettePhase::Betting, 0),
                CHIPS_OF_THE_PLAYER);
  ASSERT_TRUE(memory.slip().lay(A_CHIP_ON_SEVENTEEN, CHIPS_OF_THE_PLAYER));
  memory.askSpin();

  memory.follow(std::nullopt, CHIPS_OF_THE_PLAYER);

  EXPECT_THAT(memory.slip().bets(), IsEmpty());
  EXPECT_FALSE(memory.spinAsked());
}
