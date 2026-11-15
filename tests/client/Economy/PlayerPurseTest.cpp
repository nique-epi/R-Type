#include <gtest/gtest.h>
#include "PlayerPurse.hpp"
#include "PlayerPurseTestConstants.hpp"
#include "PlayerSettings.hpp"
#include "Purse.hpp"

using rtype::client::PlayerSettings;
using rtype::client::Purse;
using rtype::client::purseOf;
using rtype::client::testing::CHIPS_OF_THE_SETTINGS;
using rtype::client::testing::CROWN_IN_THE_LOCKER;
using rtype::client::testing::KOPECKS_OF_THE_SETTINGS;

/**
 * Given settings holding kopecks, chips and the crown in the locker
 * When the purse of the player is read from them
 * Then it holds the same kopecks, the same chips and the same locker
 */
TEST(PlayerPurse, ThePurseHoldsWhatTheSettingsHold) {
  PlayerSettings settings;
  settings.kopecks = KOPECKS_OF_THE_SETTINGS;
  settings.chips = CHIPS_OF_THE_SETTINGS;
  settings.shipLocker.items.set(CROWN_IN_THE_LOCKER);

  const Purse purse = purseOf(settings);

  EXPECT_EQ(purse.kopecks, KOPECKS_OF_THE_SETTINGS);
  EXPECT_EQ(purse.chips, CHIPS_OF_THE_SETTINGS);
  EXPECT_EQ(purse.locker, settings.shipLocker);
}
