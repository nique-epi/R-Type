#include <gtest/gtest.h>
#include "ChipSettlement.hpp"
#include "ChipSettlementTestConstants.hpp"
#include "PlayerSettings.hpp"
#include "RecordingCasinoChannel.hpp"
#include "RecordingSettingsStorage.hpp"
#include "SettingsConstants.hpp"
#include "SettingsService.hpp"

using rtype::client::chipsAfter;
using rtype::client::MAXIMUM_CHIPS;
using rtype::client::PlayerSettings;
using rtype::client::SettingsService;
using rtype::client::settleChipMoves;
using rtype::client::testing::A_HUNDRED_WON;
using rtype::client::testing::A_ROUND_WON;
using rtype::client::testing::A_STAKE_OF_A_HUNDRED_LOST;
using rtype::client::testing::A_THOUSAND_CHIPS;
using rtype::client::testing::CHIPS_AFTER_THE_ROUND_WON;
using rtype::client::testing::FIFTY_CHIPS;
using rtype::client::testing::TEN_CHIPS_UNDER_THE_LARGEST;
using rtype::client::testing::THE_LARGEST_PAYOUT;

/**
 * Given a player with a thousand chips and a round that took 100 and paid 350
 * When the moves of the table are settled
 * Then the player holds 1250 chips, saved once
 */
TEST(ChipSettlement, ARoundTakesItsStakeAndPaysItsPayoutAndIsSaved) {
  PlayerSettings settings;
  settings.chips = A_THOUSAND_CHIPS;
  RecordingSettingsStorage storage;
  SettingsService service(settings, storage);
  RecordingCasinoChannel casino;
  casino.setChipMoves(A_ROUND_WON);

  settleChipMoves(casino, service);

  EXPECT_EQ(settings.chips, CHIPS_AFTER_THE_ROUND_WON);
  ASSERT_EQ(storage.saves().size(), 1U);
  EXPECT_EQ(storage.saves().back().chips, CHIPS_AFTER_THE_ROUND_WON);
}

/**
 * Given a round settled once
 * When the moves of the table are settled again, the table moving nothing new
 * Then the chips stay where the round left them, and nothing more is saved
 */
TEST(ChipSettlement, ARoundIsSettledOnce) {
  PlayerSettings settings;
  settings.chips = A_THOUSAND_CHIPS;
  RecordingSettingsStorage storage;
  SettingsService service(settings, storage);
  RecordingCasinoChannel casino;
  casino.setChipMoves(A_ROUND_WON);
  settleChipMoves(casino, service);

  settleChipMoves(casino, service);

  EXPECT_EQ(settings.chips, CHIPS_AFTER_THE_ROUND_WON);
  EXPECT_EQ(storage.saves().size(), 1U);
}

/**
 * Given a player and a table that moved nothing
 * When the moves of the table are settled
 * Then nothing is saved
 */
TEST(ChipSettlement, NothingMovedSavesNothing) {
  PlayerSettings settings;
  RecordingSettingsStorage storage;
  SettingsService service(settings, storage);
  RecordingCasinoChannel casino;

  settleChipMoves(casino, service);

  EXPECT_TRUE(storage.saves().empty());
}

/**
 * Given a player with fifty chips
 * When a stake of a hundred the server settled is taken off
 * Then the player holds no chip, never fewer
 */
TEST(ChipSettlement, AStakeNeverTakesTheChipsBelowNone) {
  EXPECT_EQ(chipsAfter(FIFTY_CHIPS, A_STAKE_OF_A_HUNDRED_LOST), 0U);
}

/**
 * Given a player ten chips under the largest balance
 * When a round pays a hundred
 * Then the player holds the largest balance, never more
 */
TEST(ChipSettlement, APayoutStopsAtTheLargestBalance) {
  EXPECT_EQ(chipsAfter(TEN_CHIPS_UNDER_THE_LARGEST, A_HUNDRED_WON),
            MAXIMUM_CHIPS);
}

/**
 * Given a player ten chips under the largest balance
 * When a round pays as much as the count of the server can hold
 * Then the player holds the largest balance: the sum does not wrap around
 */
TEST(ChipSettlement, TheLargestPayoutDoesNotWrapAround) {
  EXPECT_EQ(chipsAfter(TEN_CHIPS_UNDER_THE_LARGEST, THE_LARGEST_PAYOUT),
            MAXIMUM_CHIPS);
}
