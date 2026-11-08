#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <array>
#include <string>
#include <vector>
#include "EngineException.hpp"
#include "ServerException.hpp"
#include "WaveCatalog.hpp"
#include "WaveCatalogFixtures.hpp"
#include "WaveCatalogLayersFixtures.hpp"
#include "WaveCatalogLayersTestConstants.hpp"
#include "WaveCatalogTestConstants.hpp"
#include "WaveCurve.hpp"

using rtype::engine::DataFileSyntaxException;
using rtype::engine::DataFileValueOutOfRangeException;
using rtype::engine::MissingDataFileKeyException;
using rtype::engine::UnknownDataFileKeyException;
using rtype::server::TooManyDifficultiesException;
using rtype::server::WaveCatalog;
using rtype::server::testing::A_WEAVER_ALONE;
using rtype::server::testing::A_WEAVER_OVER_CALM;
using rtype::server::testing::CALM_CURVE;
using rtype::server::testing::CALM_ENEMIES;
using rtype::server::testing::CALM_THEN_STORM;
using rtype::server::testing::calmFile;
using rtype::server::testing::errorLaying;
using rtype::server::testing::fileOfDifficulties;
using rtype::server::testing::FIRST_LEVEL_CHANGED;
using rtype::server::testing::FOUR_BYDOS;
using rtype::server::testing::FOUR_BYDOS_OVER_CALM;
using rtype::server::testing::GALE_MISSING_KEY_TEXT;
using rtype::server::testing::laid;
using rtype::server::testing::LINE_1_OF_THE_RESOURCE;
using rtype::server::testing::MISSPELT_KEY_IN_THE_RESOURCE_MESSAGE;
using rtype::server::testing::MISSPELT_KEY_OVER_CALM;
using rtype::server::testing::MOST_DIFFICULTIES_OF_A_FILE;
using rtype::server::testing::NINE_BYDOS;
using rtype::server::testing::NINE_BYDOS_OVER_CALM;
using rtype::server::testing::NO_BYDO_IN_THE_RESOURCE_MESSAGE;
using rtype::server::testing::NO_BYDO_OVER_CALM;
using rtype::server::testing::NOT_TOML_OVER_CALM;
using rtype::server::testing::PART_OF_GALE;
using rtype::server::testing::SEVENTEENTH_LEVEL_ADDED;
using rtype::server::testing::SHORT_FIRST_INTERVAL_OVER_CALM;
using rtype::server::testing::SHORTEST_OF_THE_ASSETS_MESSAGE;
using rtype::server::testing::STORM_CURVE;
using rtype::server::testing::STORM_TABLE;
using rtype::server::testing::SYNTAX_ERROR_IN_THE_RESOURCE_START;
using ::testing::HasSubstr;
using ::testing::StartsWith;

/**
 * Given calm, and a file laid over it that gives its first wave 9 Bydos
 * When both are read
 * Then the first wave of calm holds 9 Bydos
 */
TEST(WaveCatalogLayers, AKeyOfALaterFileReplacesTheSameKey) {
  const std::array files{calmFile(), std::string{NINE_BYDOS_OVER_CALM}};

  EXPECT_EQ(laid(files).planOf("calm").curve.firstWave.entityCount, NINE_BYDOS);
}

/**
 * Given calm, and a file laid over it that gives its first wave alone
 * When both are read
 * Then every other key of calm keeps the value of the first file
 */
TEST(WaveCatalogLayers, AKeyALaterFileLeavesOutKeepsItsEarlierValue) {
  const std::array files{calmFile(), std::string{NINE_BYDOS_OVER_CALM}};
  const WaveCatalog catalog = laid(files);
  rtype::game::WaveCurve expected = CALM_CURVE;
  expected.firstWave.entityCount = NINE_BYDOS;

  EXPECT_EQ(catalog.planOf("calm").curve, expected);
  EXPECT_EQ(catalog.planOf("calm").enemies,
            std::vector<std::string>(CALM_ENEMIES.begin(), CALM_ENEMIES.end()));
}

/**
 * Given calm, a file that gives its first wave 9 Bydos, then a file that
 * gives it 4
 * When the three are read in that order
 * Then the first wave of calm holds 4 Bydos: the last file wins
 */
TEST(WaveCatalogLayers, TheLastFileThatGivesAKeyWins) {
  const std::array files{calmFile(), std::string{NINE_BYDOS_OVER_CALM},
                         std::string{FOUR_BYDOS_OVER_CALM}};

  EXPECT_EQ(laid(files).planOf("calm").curve.firstWave.entityCount, FOUR_BYDOS);
}

/**
 * Given calm, and a file laid over it that gives its enemies, a single Weaver
 * When both are read
 * Then the enemies of calm are the Weaver alone: the list is replaced whole
 */
TEST(WaveCatalogLayers, AListOfALaterFileReplacesTheWholeList) {
  const std::array files{calmFile(), std::string{A_WEAVER_OVER_CALM}};

  EXPECT_EQ(
      laid(files).planOf("calm").enemies,
      std::vector<std::string>(A_WEAVER_ALONE.begin(), A_WEAVER_ALONE.end()));
}

/**
 * Given calm, and a file laid over it that describes storm whole
 * When both are read
 * Then the catalog holds calm and storm, storm as its file describes it
 */
TEST(WaveCatalogLayers, ADifficultyOnlyALaterFileNamesIsAdded) {
  const std::array files{calmFile(), std::string{STORM_TABLE}};
  const WaveCatalog catalog = laid(files);

  EXPECT_EQ(
      catalog.difficulties(),
      std::vector<std::string>(CALM_THEN_STORM.begin(), CALM_THEN_STORM.end()));
  EXPECT_EQ(catalog.planOf("storm").curve, STORM_CURVE);
}

/**
 * Given calm, and a file laid over it that gives its first wave no Bydo, on
 * line 2
 * When both are read
 * Then the error names the second file, the line and the column of the value
 */
TEST(WaveCatalogLayers, AValueOutOfRangeIsNamedInTheFileItComesFrom) {
  const std::array files{calmFile(), std::string{NO_BYDO_OVER_CALM}};

  EXPECT_THROW(static_cast<void>(laid(files)),
               DataFileValueOutOfRangeException);
  EXPECT_EQ(errorLaying(files), NO_BYDO_IN_THE_RESOURCE_MESSAGE);
}

/**
 * Given calm, and a file laid over it that misspells a key, on line 2
 * When both are read
 * Then the error names the second file, the line and the key
 */
TEST(WaveCatalogLayers, AMisspeltKeyIsNamedInTheFileItComesFrom) {
  const std::array files{calmFile(), std::string{MISSPELT_KEY_OVER_CALM}};

  EXPECT_THROW(static_cast<void>(laid(files)), UnknownDataFileKeyException);
  EXPECT_EQ(errorLaying(files), MISSPELT_KEY_IN_THE_RESOURCE_MESSAGE);
}

/**
 * Given calm, whose shortest interval is 1500 ms, and a file laid over it that
 * makes its first interval 1000 ms
 * When both are read
 * Then the shortest interval of the first file is refused against the first
 * interval of the second
 */
TEST(WaveCatalogLayers, AValueIsCheckedAgainstTheValueOfAnotherFile) {
  const std::array files{calmFile(),
                         std::string{SHORT_FIRST_INTERVAL_OVER_CALM}};

  EXPECT_EQ(errorLaying(files), SHORTEST_OF_THE_ASSETS_MESSAGE);
}

/**
 * Given calm, and a file laid over it that is not TOML on line 2
 * When both are read
 * Then a DataFileSyntaxException names the second file and the line
 */
TEST(WaveCatalogLayers, TextThatIsNotTomlIsNamedInItsFile) {
  const std::array files{calmFile(), std::string{NOT_TOML_OVER_CALM}};

  EXPECT_THROW(static_cast<void>(laid(files)), DataFileSyntaxException);
  EXPECT_THAT(errorLaying(files),
              StartsWith(std::string{SYNTAX_ERROR_IN_THE_RESOURCE_START}));
}

/**
 * Given calm, and a file laid over it that names gale with one key of its
 * nine
 * When both are read
 * Then a MissingDataFileKeyException names a missing key of gale, at the table
 * of gale in the second file
 */
TEST(WaveCatalogLayers, AKeyMissingFromAnAddedDifficultyIsNamedInItsFile) {
  const std::array files{calmFile(), std::string{PART_OF_GALE}};

  EXPECT_THROW(static_cast<void>(laid(files)), MissingDataFileKeyException);
  EXPECT_THAT(errorLaying(files),
              StartsWith(std::string{LINE_1_OF_THE_RESOURCE}));
  EXPECT_THAT(errorLaying(files),
              HasSubstr(std::string{GALE_MISSING_KEY_TEXT}));
}

/**
 * Given a file of 16 difficulties, the most a server may hold, and a file
 * laid over it that changes the first of them
 * When both are read
 * Then the catalog holds the 16 difficulties
 */
TEST(WaveCatalogLayers, AFileMayChangeTheMostDifficultiesAServerMayHold) {
  const std::array files{fileOfDifficulties(MOST_DIFFICULTIES_OF_A_FILE),
                         std::string{FIRST_LEVEL_CHANGED}};

  EXPECT_EQ(laid(files).difficulties().size(), MOST_DIFFICULTIES_OF_A_FILE);
}

/**
 * Given a file of 16 difficulties, the most a server may hold, and a file
 * laid over it that adds a seventeenth
 * When both are read
 * Then a TooManyDifficultiesException names the second file
 */
TEST(WaveCatalogLayers, FilesTogetherHoldNoMoreDifficultiesThanAServerMay) {
  const std::array files{fileOfDifficulties(MOST_DIFFICULTIES_OF_A_FILE),
                         std::string{SEVENTEENTH_LEVEL_ADDED}};

  EXPECT_THROW(static_cast<void>(laid(files)), TooManyDifficultiesException);
  EXPECT_THAT(errorLaying(files),
              StartsWith(std::string{LINE_1_OF_THE_RESOURCE}));
}

/**
 * Given calm alone, then calm with a file laid over it
 * When each is read
 * Then their fingerprints differ
 */
TEST(WaveCatalogLayers, ALaterFileChangesTheFingerprint) {
  const std::array alone{calmFile()};
  const std::array files{calmFile(), std::string{NINE_BYDOS_OVER_CALM}};

  EXPECT_NE(laid(files).fingerprint(), laid(alone).fingerprint());
}

/**
 * Given calm with a file laid over it that gives 9 Bydos, then calm with a
 * file that gives 4
 * When each is read
 * Then their fingerprints differ: the text of a later file counts
 */
TEST(WaveCatalogLayers, TheTextOfALaterFileCountsInTheFingerprint) {
  const std::array nine{calmFile(), std::string{NINE_BYDOS_OVER_CALM}};
  const std::array four{calmFile(), std::string{FOUR_BYDOS_OVER_CALM}};

  EXPECT_NE(laid(nine).fingerprint(), laid(four).fingerprint());
}
