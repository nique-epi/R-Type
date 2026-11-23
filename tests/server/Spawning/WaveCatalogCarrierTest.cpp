#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <string_view>
#include "EngineException.hpp"
#include "ServerException.hpp"
#include "ShippedEntitySheets.hpp"
#include "WaveCatalog.hpp"
#include "WaveCatalogCarrierTestConstants.hpp"
#include "WaveCatalogFixtures.hpp"
#include "WaveCatalogTestConstants.hpp"

using rtype::engine::DataFileValueOutOfRangeException;
using rtype::game::testing::shippedEntitySheets;
using rtype::server::UnknownCarrierException;
using rtype::server::WaveCatalog;
using rtype::server::testing::calmFile;
using rtype::server::testing::calmFileWith;
using rtype::server::testing::CARRIER_EVERY_KEY_IN_TESTS;
using rtype::server::testing::CARRIER_KEY_IN_TESTS;
using rtype::server::testing::CRYSTAL_CARRIER_AS_A_VALUE;
using rtype::server::testing::CRYSTAL_CARRIER_NAME_IN_TESTS;
using rtype::server::testing::DRIFTER_AS_A_CARRIER;
using rtype::server::testing::errorReading;
using rtype::server::testing::EVERY_OTHER_WAVE_COUNT;
using rtype::server::testing::EVERY_THIRD_WAVE_FOR_A_CARRIER;
using rtype::server::testing::EVERY_THIRD_WAVE_FOR_A_CARRIER_COUNT;
using rtype::server::testing::NEVER_VALUE;
using rtype::server::testing::PAST_THE_MOST_WAVES_VALUE;
using rtype::server::testing::SHIPPED_DIFFICULTIES;
using rtype::server::testing::shippedCatalog;
using rtype::server::testing::TEST_FILE_NAME;
using rtype::server::testing::UNKNOWN_DRIFTER_CARRIER_TEXT;
using ::testing::HasSubstr;

/**
 * Given a waves file whose difficulty calm names the crystal carrier
 * When it is read
 * Then the waves of calm bring the crystal carrier
 */
TEST(WaveCatalog, ReadsTheCarrierOfADifficulty) {
  const WaveCatalog catalog = WaveCatalog::fromText(
      calmFileWith(CARRIER_KEY_IN_TESTS, CRYSTAL_CARRIER_AS_A_VALUE),
      TEST_FILE_NAME, shippedEntitySheets());

  EXPECT_EQ(catalog.planOf("calm").carrier,
            std::optional<std::string>{CRYSTAL_CARRIER_NAME_IN_TESTS});
}

/**
 * Given a waves file whose difficulty calm names no carrier
 * When it is read
 * Then the waves of calm bring none
 */
TEST(WaveCatalog, ADifficultyWithoutACarrierBringsNone) {
  const WaveCatalog catalog =
      WaveCatalog::fromText(calmFile(), TEST_FILE_NAME, shippedEntitySheets());

  EXPECT_FALSE(catalog.planOf("calm").carrier.has_value());
}

/**
 * Given a waves file whose difficulty calm names the Drifter as its carrier
 * When it is read
 * Then it is refused, naming the Drifter: it drops no crystal
 */
TEST(WaveCatalog, RefusesACarrierThatDropsNoCrystal) {
  const std::string text =
      calmFileWith(CARRIER_KEY_IN_TESTS, DRIFTER_AS_A_CARRIER);

  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(text, TEST_FILE_NAME,
                                                       shippedEntitySheets())),
               UnknownCarrierException);
  EXPECT_THAT(errorReading(text), HasSubstr(UNKNOWN_DRIFTER_CARRIER_TEXT));
}

/**
 * Given a waves file whose calm has no carrier_every
 * When it is read
 * Then its carrier comes every other wave
 */
TEST(WaveCatalog, ACarrierComesEveryOtherWaveByDefault) {
  const WaveCatalog catalog =
      WaveCatalog::fromText(calmFile(), TEST_FILE_NAME, shippedEntitySheets());

  EXPECT_EQ(catalog.planOf("calm").carrierEvery, EVERY_OTHER_WAVE_COUNT);
}

/**
 * Given a waves file whose calm brings its carrier every 3 waves
 * When it is read
 * Then the plan of calm says 3
 */
TEST(WaveCatalog, ReadsHowOftenAWaveBringsTheCarrier) {
  const WaveCatalog catalog = WaveCatalog::fromText(
      calmFileWith(CARRIER_EVERY_KEY_IN_TESTS, EVERY_THIRD_WAVE_FOR_A_CARRIER),
      TEST_FILE_NAME, shippedEntitySheets());

  EXPECT_EQ(catalog.planOf("calm").carrierEvery,
            EVERY_THIRD_WAVE_FOR_A_CARRIER_COUNT);
}

/**
 * Given a waves file whose calm brings its carrier every 0 waves, then every
 * 101
 * When each is read
 * Then both are refused as out of range
 */
TEST(WaveCatalog, RefusesACarrierEveryOutsideOneToAHundred) {
  const std::string never =
      calmFileWith(CARRIER_EVERY_KEY_IN_TESTS, NEVER_VALUE);
  const std::string pastTheMost =
      calmFileWith(CARRIER_EVERY_KEY_IN_TESTS, PAST_THE_MOST_WAVES_VALUE);

  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(never, TEST_FILE_NAME,
                                                       shippedEntitySheets())),
               DataFileValueOutOfRangeException);
  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(
                   pastTheMost, TEST_FILE_NAME, shippedEntitySheets())),
               DataFileValueOutOfRangeException);
}

/**
 * Given the waves file the game ships
 * When it is read
 * Then each of its difficulties brings the crystal carrier every other wave
 */
TEST(WaveCatalog, TheShippedDifficultiesBringTheCarrierEveryOtherWave) {
  const WaveCatalog catalog = shippedCatalog();

  for (const std::string_view difficulty : SHIPPED_DIFFICULTIES) {
    EXPECT_EQ(catalog.planOf(difficulty).carrier,
              std::optional<std::string>{CRYSTAL_CARRIER_NAME_IN_TESTS});
    EXPECT_EQ(catalog.planOf(difficulty).carrierEvery, EVERY_OTHER_WAVE_COUNT);
  }
}
