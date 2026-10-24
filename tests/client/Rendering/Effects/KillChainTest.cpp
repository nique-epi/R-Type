#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "EffectTestConstants.hpp"
#include "KillChainFixtures.hpp"
#include "ScreenTestConstants.hpp"

using ::testing::ElementsAre;

/**
 * Given ten kills 100 ms apart
 * When each is counted
 * Then the fifth and the tenth only are big catches
 */
TEST(KillChain, EveryFifthKillOfAChainIsABigCatch) {
  EXPECT_THAT(bigCatchesOf(2 * KILLS_PER_BIG_CATCH, CLOSE_KILLS),
              ElementsAre(false, false, false, false, true, false, false, false,
                          false, true));
}

/**
 * Given five kills, each one nanosecond less than a second after the previous
 * When each is counted
 * Then the fifth is a big catch: the chain held
 */
TEST(KillChain, AKillLessThanASecondLaterExtendsTheChain) {
  EXPECT_TRUE(
      bigCatchesOf(KILLS_PER_BIG_CATCH, CHAIN_WINDOW_LENGTH - ONE_TICK).back());
}

/**
 * Given five kills, each a second after the previous
 * When each is counted
 * Then the fifth is no big catch: each started a new chain
 */
TEST(KillChain, AKillASecondLaterStartsANewChain) {
  EXPECT_FALSE(bigCatchesOf(KILLS_PER_BIG_CATCH, CHAIN_WINDOW_LENGTH).back());
}
