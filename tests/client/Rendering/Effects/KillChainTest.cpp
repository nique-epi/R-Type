#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <chrono>
#include <vector>
#include "EffectLengths.hpp"
#include "KillChain.hpp"
#include "ScreenTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::KillChain;
using rtype::engine::Duration;
using ::testing::ElementsAre;

namespace {

constexpr Duration CLOSE_KILLS = std::chrono::milliseconds(100);

/** @returns Whether each of @p count kills, @p gap apart, is a big catch. */
std::vector<bool> bigCatchesOf(int count, Duration gap) {
  KillChain chain;
  std::vector<bool> bigCatches;
  for (int kill = 0; kill < count; ++kill) {
    bigCatches.push_back(chain.noteKill());
    chain.advance(gap);
  }
  return bigCatches;
}

}  // namespace

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
