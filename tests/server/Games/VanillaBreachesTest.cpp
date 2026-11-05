#include <gtest/gtest.h>
#include <optional>
#include <string_view>
#include <vector>
#include "ServerConstants.hpp"
#include "VanillaBreaches.hpp"
#include "VanillaBreachesTestConstants.hpp"

namespace rtype::server::testing {

/**
 * Given the shipped waves file, no seed and the default input delay
 * When the games are judged
 * Then they are vanilla: no reason is given
 */
TEST(VanillaBreaches, TheShippedRulesAreVanilla) {
  EXPECT_TRUE(vanillaBreaches(OFFICIAL_WAVES_FINGERPRINT, std::nullopt,
                              DEFAULT_INPUT_DELAY_TICKS)
                  .empty());
}

/**
 * Given another waves file
 * When the games are judged
 * Then the waves file is the only reason they are not vanilla
 */
TEST(VanillaBreaches, AnotherWavesFileIsNotVanilla) {
  EXPECT_EQ(vanillaBreaches(ANOTHER_WAVES_FINGERPRINT, std::nullopt,
                            DEFAULT_INPUT_DELAY_TICKS),
            std::vector<std::string_view>{MODIFIED_WAVES_BREACH});
}

/**
 * Given a seed of 0
 * When the games are judged
 * Then the seed is the only reason they are not vanilla: 0 is a seed too
 */
TEST(VanillaBreaches, ASeedOfZeroIsNotVanilla) {
  EXPECT_EQ(vanillaBreaches(OFFICIAL_WAVES_FINGERPRINT, ZERO_SEED,
                            DEFAULT_INPUT_DELAY_TICKS),
            std::vector<std::string_view>{IMPOSED_SEED_BREACH});
}

/**
 * Given an input delay one tick shorter than the default one
 * When the games are judged
 * Then the delay is the only reason they are not vanilla
 */
TEST(VanillaBreaches, AShorterInputDelayIsNotVanilla) {
  EXPECT_EQ(vanillaBreaches(OFFICIAL_WAVES_FINGERPRINT, std::nullopt,
                            ONE_TICK_SHORTER_DELAY),
            std::vector<std::string_view>{CHANGED_INPUT_DELAY_BREACH});
}

/**
 * Given an input delay one tick longer than the default one
 * When the games are judged
 * Then the delay is the only reason they are not vanilla
 */
TEST(VanillaBreaches, ALongerInputDelayIsNotVanilla) {
  EXPECT_EQ(vanillaBreaches(OFFICIAL_WAVES_FINGERPRINT, std::nullopt,
                            ONE_TICK_LONGER_DELAY),
            std::vector<std::string_view>{CHANGED_INPUT_DELAY_BREACH});
}

/**
 * Given another waves file, a seed and another input delay
 * When the games are judged
 * Then every reason is given, in the order of the rules
 */
TEST(VanillaBreaches, GivesEveryReasonInOrder) {
  EXPECT_EQ(
      vanillaBreaches(ANOTHER_WAVES_FINGERPRINT, ZERO_SEED,
                      ONE_TICK_LONGER_DELAY),
      (std::vector<std::string_view>{MODIFIED_WAVES_BREACH, IMPOSED_SEED_BREACH,
                                     CHANGED_INPUT_DELAY_BREACH}));
}

}  // namespace rtype::server::testing
