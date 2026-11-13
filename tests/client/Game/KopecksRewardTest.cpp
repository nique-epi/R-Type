#include <gtest/gtest.h>
#include <array>
#include "FinalPilot.hpp"
#include "GameEnded.hpp"
#include "KopecksReward.hpp"
#include "KopecksRewardTestConstants.hpp"
#include "PlayerSettings.hpp"
#include "RecordingSettingsStorage.hpp"
#include "SettingsService.hpp"

using rtype::client::creditKopecks;
using rtype::client::FinalPilot;
using rtype::client::kopecksEarnedBy;
using rtype::client::PlayerSettings;
using rtype::client::SettingsService;
using rtype::client::testing::ANOTHER_PILOT_SCORE;
using rtype::client::testing::BALANCE_AFTER_THE_GAME;
using rtype::client::testing::BALANCE_BEFORE_THE_GAME;
using rtype::client::testing::BALANCE_UNDER_THE_LARGEST;
using rtype::client::testing::OWN_FINAL_SCORE;
using rtype::client::testing::OWN_GAIN_AFTER_A_DEFEAT;
using rtype::client::testing::OWN_GAIN_AFTER_A_VICTORY;
using rtype::client::testing::THE_LARGEST_BALANCE;
using rtype::network::WireGameOutcome;

/**
 * Given two final pilots, the second being this client with 12450 points
 * When the kopecks of a lost game are counted
 * Then they are those of its own score: 1245, whatever the other scored
 */
TEST(KopecksReward, ADefeatPaysTheScoreOfThisClient) {
  const std::array pilots{
      FinalPilot{
          .playerNumber = 1, .nickname = {}, .score = ANOTHER_PILOT_SCORE},
      FinalPilot{.playerNumber = 2,
                 .nickname = {},
                 .score = OWN_FINAL_SCORE,
                 .isLocalPlayer = true}};

  EXPECT_EQ(kopecksEarnedBy(pilots, WireGameOutcome::Defeat),
            OWN_GAIN_AFTER_A_DEFEAT);
}

/**
 * Given the same pilots
 * When the kopecks of a won game are counted
 * Then they are a quarter more: 1556
 */
TEST(KopecksReward, AVictoryPaysAQuarterMore) {
  const std::array pilots{
      FinalPilot{
          .playerNumber = 1, .nickname = {}, .score = ANOTHER_PILOT_SCORE},
      FinalPilot{.playerNumber = 2,
                 .nickname = {},
                 .score = OWN_FINAL_SCORE,
                 .isLocalPlayer = true}};

  EXPECT_EQ(kopecksEarnedBy(pilots, WireGameOutcome::Victory),
            OWN_GAIN_AFTER_A_VICTORY);
}

/**
 * Given final pilots none of whom is this client
 * When the kopecks of the game are counted
 * Then there are none
 */
TEST(KopecksReward, AGameWithoutThisClientPaysNothing) {
  const std::array pilots{FinalPilot{
      .playerNumber = 1, .nickname = {}, .score = ANOTHER_PILOT_SCORE}};

  EXPECT_EQ(kopecksEarnedBy(pilots, WireGameOutcome::Victory), 0U);
}

/**
 * Given a player holding 300 kopecks
 * When a gain of 1245 is credited
 * Then the balance is 1545 and the settings are saved once with it
 */
TEST(KopecksReward, AGainIsAddedToTheBalanceAndSaved) {
  PlayerSettings settings;
  settings.kopecks = BALANCE_BEFORE_THE_GAME;
  RecordingSettingsStorage storage;
  SettingsService service(settings, storage);

  creditKopecks(service, OWN_GAIN_AFTER_A_DEFEAT);

  EXPECT_EQ(settings.kopecks, BALANCE_AFTER_THE_GAME);
  ASSERT_EQ(storage.saves().size(), 1U);
  EXPECT_EQ(storage.saves().back().kopecks, BALANCE_AFTER_THE_GAME);
}

/**
 * Given a player one kopeck under the largest balance
 * When a gain of 1245 is credited
 * Then the balance stops at the largest, 999999
 */
TEST(KopecksReward, TheBalanceNeverPassesTheLargest) {
  PlayerSettings settings;
  settings.kopecks = BALANCE_UNDER_THE_LARGEST;
  RecordingSettingsStorage storage;
  SettingsService service(settings, storage);

  creditKopecks(service, OWN_GAIN_AFTER_A_DEFEAT);

  EXPECT_EQ(settings.kopecks, THE_LARGEST_BALANCE);
}

/**
 * Given a player holding 300 kopecks
 * When no gain is credited
 * Then nothing is saved
 */
TEST(KopecksReward, NoGainSavesNothing) {
  PlayerSettings settings;
  settings.kopecks = BALANCE_BEFORE_THE_GAME;
  RecordingSettingsStorage storage;
  SettingsService service(settings, storage);

  creditKopecks(service, 0);

  EXPECT_TRUE(storage.saves().empty());
}
