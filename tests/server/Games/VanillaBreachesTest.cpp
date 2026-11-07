#include <gtest/gtest.h>
#include <optional>
#include <string_view>
#include <vector>
#include "GameRuleConstants.hpp"
#include "HostedRules.hpp"
#include "ServerConstants.hpp"
#include "VanillaBreaches.hpp"
#include "VanillaBreachesTestConstants.hpp"

namespace rtype::server::testing {

/**
 * Given the shipped waves file and entity sheets, no seed and the default
 * input delay
 * When the games are judged
 * Then they are vanilla: no reason is given
 */
TEST(VanillaBreaches, TheShippedRulesAreVanilla) {
  EXPECT_TRUE(
      vanillaBreaches(
          HostedRules{.wavesFingerprint = OFFICIAL_WAVES_FINGERPRINT,
                      .sheetsFingerprint = OFFICIAL_ENTITY_SHEETS_FINGERPRINT,
                      .seed = std::nullopt,
                      .inputDelayTicks = DEFAULT_INPUT_DELAY_TICKS})
          .empty());
}

/**
 * Given another waves file
 * When the games are judged
 * Then the waves file is the only reason they are not vanilla
 */
TEST(VanillaBreaches, AnotherWavesFileIsNotVanilla) {
  EXPECT_EQ(vanillaBreaches(HostedRules{
                .wavesFingerprint = ANOTHER_WAVES_FINGERPRINT,
                .sheetsFingerprint = OFFICIAL_ENTITY_SHEETS_FINGERPRINT,
                .seed = std::nullopt,
                .inputDelayTicks = DEFAULT_INPUT_DELAY_TICKS}),
            std::vector<std::string_view>{MODIFIED_WAVES_BREACH});
}

/**
 * Given other entity sheets
 * When the games are judged
 * Then the sheets are the only reason they are not vanilla
 */
TEST(VanillaBreaches, OtherEntitySheetsAreNotVanilla) {
  EXPECT_EQ(vanillaBreaches(
                HostedRules{.wavesFingerprint = OFFICIAL_WAVES_FINGERPRINT,
                            .sheetsFingerprint = ANOTHER_SHEETS_FINGERPRINT,
                            .seed = std::nullopt,
                            .inputDelayTicks = DEFAULT_INPUT_DELAY_TICKS}),
            std::vector<std::string_view>{MODIFIED_ENTITY_SHEETS_BREACH});
}

/**
 * Given a seed of 0
 * When the games are judged
 * Then the seed is the only reason they are not vanilla: 0 is a seed too
 */
TEST(VanillaBreaches, ASeedOfZeroIsNotVanilla) {
  EXPECT_EQ(vanillaBreaches(HostedRules{
                .wavesFingerprint = OFFICIAL_WAVES_FINGERPRINT,
                .sheetsFingerprint = OFFICIAL_ENTITY_SHEETS_FINGERPRINT,
                .seed = ZERO_SEED,
                .inputDelayTicks = DEFAULT_INPUT_DELAY_TICKS}),
            std::vector<std::string_view>{IMPOSED_SEED_BREACH});
}

/**
 * Given an input delay one tick shorter than the default one
 * When the games are judged
 * Then the delay is the only reason they are not vanilla
 */
TEST(VanillaBreaches, AShorterInputDelayIsNotVanilla) {
  EXPECT_EQ(vanillaBreaches(HostedRules{
                .wavesFingerprint = OFFICIAL_WAVES_FINGERPRINT,
                .sheetsFingerprint = OFFICIAL_ENTITY_SHEETS_FINGERPRINT,
                .seed = std::nullopt,
                .inputDelayTicks = ONE_TICK_SHORTER_DELAY}),
            std::vector<std::string_view>{CHANGED_INPUT_DELAY_BREACH});
}

/**
 * Given an input delay one tick longer than the default one
 * When the games are judged
 * Then the delay is the only reason they are not vanilla
 */
TEST(VanillaBreaches, ALongerInputDelayIsNotVanilla) {
  EXPECT_EQ(vanillaBreaches(HostedRules{
                .wavesFingerprint = OFFICIAL_WAVES_FINGERPRINT,
                .sheetsFingerprint = OFFICIAL_ENTITY_SHEETS_FINGERPRINT,
                .seed = std::nullopt,
                .inputDelayTicks = ONE_TICK_LONGER_DELAY}),
            std::vector<std::string_view>{CHANGED_INPUT_DELAY_BREACH});
}

/**
 * Given one life fewer than the default lives
 * When the game is judged
 * Then the lives are the only reason it is not vanilla
 */
TEST(VanillaBreaches, FewerLivesAreNotVanilla) {
  EXPECT_EQ(vanillaBreaches(HostedRules{
                .wavesFingerprint = OFFICIAL_WAVES_FINGERPRINT,
                .sheetsFingerprint = OFFICIAL_ENTITY_SHEETS_FINGERPRINT,
                .seed = std::nullopt,
                .inputDelayTicks = DEFAULT_INPUT_DELAY_TICKS,
                .lives = ONE_LIFE_FEWER,
                .friendlyFire = false}),
            std::vector<std::string_view>{CHANGED_LIVES_BREACH});
}

/**
 * Given one life more than the default lives
 * When the game is judged
 * Then the lives are the only reason it is not vanilla
 */
TEST(VanillaBreaches, MoreLivesAreNotVanilla) {
  EXPECT_EQ(vanillaBreaches(HostedRules{
                .wavesFingerprint = OFFICIAL_WAVES_FINGERPRINT,
                .sheetsFingerprint = OFFICIAL_ENTITY_SHEETS_FINGERPRINT,
                .seed = std::nullopt,
                .inputDelayTicks = DEFAULT_INPUT_DELAY_TICKS,
                .lives = ONE_LIFE_MORE,
                .friendlyFire = false}),
            std::vector<std::string_view>{CHANGED_LIVES_BREACH});
}

/**
 * Given friendly fire on, and every other rule the shipped one
 * When the game is judged
 * Then friendly fire is the only reason it is not vanilla
 */
TEST(VanillaBreaches, FriendlyFireIsNotVanilla) {
  EXPECT_EQ(vanillaBreaches(HostedRules{
                .wavesFingerprint = OFFICIAL_WAVES_FINGERPRINT,
                .sheetsFingerprint = OFFICIAL_ENTITY_SHEETS_FINGERPRINT,
                .seed = std::nullopt,
                .inputDelayTicks = DEFAULT_INPUT_DELAY_TICKS,
                .lives = identifiers::DEFAULT_LIVES,
                .friendlyFire = true}),
            std::vector<std::string_view>{FRIENDLY_FIRE_BREACH});
}

/**
 * Given another waves file, other entity sheets, a seed, another input delay,
 * other lives and friendly fire on
 * When the game is judged
 * Then every reason is given, in the order of the rules
 */
TEST(VanillaBreaches, GivesEveryReasonInOrder) {
  EXPECT_EQ(vanillaBreaches(
                HostedRules{.wavesFingerprint = ANOTHER_WAVES_FINGERPRINT,
                            .sheetsFingerprint = ANOTHER_SHEETS_FINGERPRINT,
                            .seed = ZERO_SEED,
                            .inputDelayTicks = ONE_TICK_LONGER_DELAY,
                            .lives = ONE_LIFE_MORE,
                            .friendlyFire = true}),
            (std::vector<std::string_view>{
                MODIFIED_WAVES_BREACH, MODIFIED_ENTITY_SHEETS_BREACH,
                IMPOSED_SEED_BREACH, CHANGED_INPUT_DELAY_BREACH,
                CHANGED_LIVES_BREACH, FRIENDLY_FIRE_BREACH}));
}

}  // namespace rtype::server::testing
