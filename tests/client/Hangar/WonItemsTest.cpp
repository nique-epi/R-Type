#include <gtest/gtest.h>
#include <cstddef>
#include "PlayerSettings.hpp"
#include "RecordingSettingsStorage.hpp"
#include "RecordingShipLookExchange.hpp"
#include "SettingsService.hpp"
#include "WonItems.hpp"

namespace rtype::client {

namespace {

/** @brief The number of the crown among the items: its place, 2, plus one. */
constexpr unsigned char CROWN_NUMBER = 3;
constexpr std::size_t CROWN_PLACE = 2;

}  // namespace

/**
 * Given a player with the starter kit and a crown the server said it won
 * When the won items are stored
 * Then the crown is in the locker and the settings were saved once
 */
TEST(WonItems, AWonItemGoesInTheLockerAndIsSaved) {
  PlayerSettings settings;
  RecordingSettingsStorage storage;
  SettingsService service(settings, storage);
  RecordingShipLookExchange looks;
  looks.setWonItems({CROWN_NUMBER});

  storeWonItems(looks, service);

  EXPECT_TRUE(settings.shipLocker.items.test(CROWN_PLACE));
  ASSERT_EQ(storage.saves().size(), 1U);
  EXPECT_TRUE(storage.saves().back().shipLocker.items.test(CROWN_PLACE));
}

/**
 * Given a player and nothing won
 * When the won items are stored
 * Then nothing is saved
 */
TEST(WonItems, NothingWonSavesNothing) {
  PlayerSettings settings;
  RecordingSettingsStorage storage;
  SettingsService service(settings, storage);
  RecordingShipLookExchange looks;

  storeWonItems(looks, service);

  EXPECT_TRUE(storage.saves().empty());
}

}  // namespace rtype::client
