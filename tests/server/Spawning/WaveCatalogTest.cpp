#include <gtest/gtest.h>
#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>
#include "ServerException.hpp"
#include "WaveCatalog.hpp"
#include "WaveCatalogFixtures.hpp"
#include "WaveCatalogTestConstants.hpp"

using rtype::server::InvalidEnemyListException;
using rtype::server::MissingWaveFileKeyException;
using rtype::server::UnknownDifficultyException;
using rtype::server::UnknownEnemyException;
using rtype::server::UnknownWaveFileKeyException;
using rtype::server::UnreadableWaveFileException;
using rtype::server::WaveCatalog;
using rtype::server::WaveFileSyntaxException;
using rtype::server::WaveFileTableExpectedException;
using rtype::server::WaveFileValueOutOfRangeException;
using rtype::server::testing::CALM_CURVE;
using rtype::server::testing::CALM_ENEMIES;
using rtype::server::testing::calmFile;
using rtype::server::testing::calmFileWith;
using rtype::server::testing::calmFileWithout;
using rtype::server::testing::EMPTY_FILE;
using rtype::server::testing::ENEMY_OUTSIDE_A_LIST;
using rtype::server::testing::errorReading;
using rtype::server::testing::FEWEST_BYDOS;
using rtype::server::testing::FRACTION;
using rtype::server::testing::INVALID_ENEMY_LIST_TEXT;
using rtype::server::testing::LEVEL_THAT_IS_NOT_A_TABLE;
using rtype::server::testing::LONGER_THAN_THE_FIRST_TEXT;
using rtype::server::testing::MISSING_DIFFICULTIES_TEXT;
using rtype::server::testing::MISSING_FILE;
using rtype::server::testing::MISSING_PAUSE_TEXT;
using rtype::server::testing::MISSPELT_KEY;
using rtype::server::testing::MISSPELT_KEY_MESSAGE;
using rtype::server::testing::MOST_BYDOS;
using rtype::server::testing::NO_BYDO;
using rtype::server::testing::NO_BYDO_MESSAGE;
using rtype::server::testing::NO_ENEMY;
using rtype::server::testing::NOT_A_TABLE_TEXT;
using rtype::server::testing::NOT_A_TOML_VALUE;
using rtype::server::testing::NUMBER_AS_AN_ENEMY;
using rtype::server::testing::ONE_BYDO_TOO_MANY;
using rtype::server::testing::SHIPPED_FILE_IN_ASSETS;
using rtype::server::testing::SHORTEST_EQUAL_TO_THE_FIRST;
using rtype::server::testing::SHORTEST_LONGER_THAN_THE_FIRST;
using rtype::server::testing::STORM_CURVE;
using rtype::server::testing::STORM_ENEMIES;
using rtype::server::testing::STORM_TABLE;
using rtype::server::testing::SYNTAX_ERROR_START;
using rtype::server::testing::TEST_FILE_NAME;
using rtype::server::testing::UNKNOWN_DIFFICULTY_MESSAGE;
using rtype::server::testing::UNKNOWN_ENEMY;
using rtype::server::testing::UNKNOWN_ENEMY_MESSAGE;
using rtype::server::testing::UNREADABLE_FILE_START;
using rtype::server::testing::WHOLE_FLOAT;

/**
 * Given a waves file describing storm, then calm
 * When it is read
 * Then each difficulty has the waves its table gives
 */
TEST(WaveCatalog, ReadsTheWavesOfEachDifficulty) {
  const WaveCatalog catalog = WaveCatalog::fromText(
      std::string{STORM_TABLE} + calmFile(), TEST_FILE_NAME);

  EXPECT_EQ(catalog.planOf("calm").curve, CALM_CURVE);
  EXPECT_EQ(catalog.planOf("storm").curve, STORM_CURVE);
}

/**
 * Given a waves file describing storm, then calm
 * When it is read
 * Then the enemies of each difficulty are the kinds its list names, in order
 */
TEST(WaveCatalog, ReadsTheEnemiesOfEachDifficultyInOrder) {
  const WaveCatalog catalog = WaveCatalog::fromText(
      std::string{STORM_TABLE} + calmFile(), TEST_FILE_NAME);

  EXPECT_TRUE(std::ranges::equal(catalog.planOf("calm").enemies, CALM_ENEMIES));
  EXPECT_TRUE(
      std::ranges::equal(catalog.planOf("storm").enemies, STORM_ENEMIES));
}

/**
 * Given a waves file describing storm, then calm
 * When its difficulties are listed
 * Then they come in alphabetical order
 */
TEST(WaveCatalog, ListsTheDifficultiesInAlphabeticalOrder) {
  const WaveCatalog catalog = WaveCatalog::fromText(
      std::string{STORM_TABLE} + calmFile(), TEST_FILE_NAME);

  EXPECT_EQ(catalog.difficulties(),
            (std::vector<std::string>{"calm", "storm"}));
}

/**
 * Given a waves file describing calm and storm
 * When the waves of normal are asked for
 * Then an UnknownDifficultyException names the difficulties the file describes
 */
TEST(WaveCatalog, AnUnknownDifficultyNamesTheOnesDescribed) {
  const WaveCatalog catalog = WaveCatalog::fromText(
      std::string{STORM_TABLE} + calmFile(), TEST_FILE_NAME);

  try {
    static_cast<void>(catalog.planOf("normal"));
    ADD_FAILURE() << "normal was found";
  } catch (const UnknownDifficultyException& error) {
    EXPECT_EQ(std::string{error.what()}, UNKNOWN_DIFFICULTY_MESSAGE);
  }
}

/**
 * Given a waves file whose line 4 is not TOML
 * When it is read
 * Then a WaveFileSyntaxException names the file and line 4
 */
TEST(WaveCatalog, TextThatIsNotTomlIsRefusedAtItsLine) {
  const std::string text =
      calmFileWith("first_spawn_interval_ms", NOT_A_TOML_VALUE);

  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(text, TEST_FILE_NAME)),
               WaveFileSyntaxException);
  EXPECT_TRUE(errorReading(text).starts_with(SYNTAX_ERROR_START));
}

/**
 * Given a waves file whose difficulty calm has no pause
 * When it is read
 * Then a MissingWaveFileKeyException names the missing key
 */
TEST(WaveCatalog, AMissingValueIsNamed) {
  const std::string text = calmFileWithout("wave_pause_ms");

  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(text, TEST_FILE_NAME)),
               MissingWaveFileKeyException);
  EXPECT_NE(errorReading(text).find(MISSING_PAUSE_TEXT), std::string::npos);
}

/**
 * Given a waves file whose difficulty calm also holds a misspelt key, on line 9
 * When it is read
 * Then an UnknownWaveFileKeyException names the key, its line and its column
 */
TEST(WaveCatalog, AMisspeltKeyIsRefused) {
  const std::string text = calmFileWith(MISSPELT_KEY, FEWEST_BYDOS);

  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(text, TEST_FILE_NAME)),
               UnknownWaveFileKeyException);
  EXPECT_EQ(errorReading(text), MISSPELT_KEY_MESSAGE);
}

/**
 * Given an empty waves file
 * When it is read
 * Then a MissingWaveFileKeyException says the table of the difficulties is
 * missing
 */
TEST(WaveCatalog, AFileWithoutDifficultiesIsRefused) {
  EXPECT_THROW(
      static_cast<void>(WaveCatalog::fromText(EMPTY_FILE, TEST_FILE_NAME)),
      MissingWaveFileKeyException);
  EXPECT_NE(errorReading(EMPTY_FILE).find(MISSING_DIFFICULTIES_TEXT),
            std::string::npos);
}

/**
 * Given a waves file whose difficulty calm is a number
 * When it is read
 * Then a WaveFileTableExpectedException names it
 */
TEST(WaveCatalog, ADifficultyThatIsNotATableIsRefused) {
  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(
                   LEVEL_THAT_IS_NOT_A_TABLE, TEST_FILE_NAME)),
               WaveFileTableExpectedException);
  EXPECT_NE(errorReading(LEVEL_THAT_IS_NOT_A_TABLE).find(NOT_A_TABLE_TEXT),
            std::string::npos);
}

/**
 * Given a first wave of 1 Bydo, the fewest allowed
 * When the file is read
 * Then calm starts with 1 Bydo
 */
TEST(WaveCatalog, AcceptsTheFewestBydos) {
  const WaveCatalog catalog = WaveCatalog::fromText(
      calmFileWith("first_wave_bydos", FEWEST_BYDOS), TEST_FILE_NAME);

  EXPECT_EQ(catalog.planOf("calm").curve.firstWave.bydoCount, 1U);
}

/**
 * Given a first wave of 0 Bydos, one under the fewest
 * When the file is read
 * Then a WaveFileValueOutOfRangeException names the file, line 2, the column
 * of the value, the key and its range
 */
TEST(WaveCatalog, RefusesAFirstWaveWithoutBydos) {
  const std::string text = calmFileWith("first_wave_bydos", NO_BYDO);

  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(text, TEST_FILE_NAME)),
               WaveFileValueOutOfRangeException);
  EXPECT_EQ(errorReading(text), NO_BYDO_MESSAGE);
}

/**
 * Given a first wave of 100 Bydos, the most allowed
 * When the file is read
 * Then calm starts with 100 Bydos
 */
TEST(WaveCatalog, AcceptsTheMostBydos) {
  const WaveCatalog catalog = WaveCatalog::fromText(
      calmFileWith("first_wave_bydos", MOST_BYDOS), TEST_FILE_NAME);

  EXPECT_EQ(catalog.planOf("calm").curve.firstWave.bydoCount, 100U);
}

/**
 * Given a first wave of 101 Bydos, one over the most
 * When the file is read
 * Then a WaveFileValueOutOfRangeException is thrown
 */
TEST(WaveCatalog, RefusesOneBydoTooMany) {
  EXPECT_THROW(
      static_cast<void>(WaveCatalog::fromText(
          calmFileWith("first_wave_bydos", ONE_BYDO_TOO_MANY), TEST_FILE_NAME)),
      WaveFileValueOutOfRangeException);
}

/**
 * Given a first interval of 2500.5 milliseconds
 * When the file is read
 * Then a WaveFileValueOutOfRangeException is thrown: durations are whole
 * milliseconds
 */
TEST(WaveCatalog, RefusesAFraction) {
  EXPECT_THROW(
      static_cast<void>(WaveCatalog::fromText(
          calmFileWith("first_spawn_interval_ms", FRACTION), TEST_FILE_NAME)),
      WaveFileValueOutOfRangeException);
}

/**
 * Given a first interval of 2500.0 milliseconds, a float that is whole
 * When the file is read
 * Then a WaveFileValueOutOfRangeException is thrown: the keys take TOML
 * integers, as the file says
 */
TEST(WaveCatalog, RefusesAFloatEvenWhenWhole) {
  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(
                   calmFileWith("first_spawn_interval_ms", WHOLE_FLOAT),
                   TEST_FILE_NAME)),
               WaveFileValueOutOfRangeException);
}

/**
 * Given a shortest interval equal to the first, 2500 milliseconds
 * When the file is read
 * Then calm keeps its Bydos 2.5 seconds apart in every wave
 */
TEST(WaveCatalog, AcceptsAShortestIntervalEqualToTheFirst) {
  const WaveCatalog catalog = WaveCatalog::fromText(
      calmFileWith("shortest_spawn_interval_ms", SHORTEST_EQUAL_TO_THE_FIRST),
      TEST_FILE_NAME);

  EXPECT_EQ(catalog.planOf("calm").curve.shortestSpawnInterval,
            CALM_CURVE.firstWave.spawnInterval);
}

/**
 * Given a shortest interval of 2501 milliseconds, one longer than the first
 * When the file is read
 * Then a WaveFileValueOutOfRangeException gives the range up to the first
 * interval: the waves would slow down
 */
TEST(WaveCatalog, RefusesAShortestIntervalLongerThanTheFirst) {
  const std::string text = calmFileWith("shortest_spawn_interval_ms",
                                        SHORTEST_LONGER_THAN_THE_FIRST);

  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(text, TEST_FILE_NAME)),
               WaveFileValueOutOfRangeException);
  EXPECT_NE(errorReading(text).find(LONGER_THAN_THE_FIRST_TEXT),
            std::string::npos);
}

/**
 * Given a waves file whose calm names an enemy no kind has
 * When it is read
 * Then an UnknownEnemyException names the file, the line and the column of
 * the name, the key, and the enemies the server knows
 */
TEST(WaveCatalog, AnUnknownEnemyIsNamedWithTheKnownOnes) {
  const std::string text = calmFileWith("enemies", UNKNOWN_ENEMY);

  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(text, TEST_FILE_NAME)),
               UnknownEnemyException);
  EXPECT_EQ(errorReading(text), UNKNOWN_ENEMY_MESSAGE);
}

/**
 * Given a waves file whose calm lists no enemy
 * When it is read
 * Then an InvalidEnemyListException gives the length allowed
 */
TEST(WaveCatalog, AnEmptyEnemyListIsRefused) {
  const std::string text = calmFileWith("enemies", NO_ENEMY);

  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(text, TEST_FILE_NAME)),
               InvalidEnemyListException);
  EXPECT_NE(errorReading(text).find(INVALID_ENEMY_LIST_TEXT),
            std::string::npos);
}

/**
 * Given a waves file whose calm gives one name outside a list
 * When it is read
 * Then an InvalidEnemyListException is thrown
 */
TEST(WaveCatalog, EnemiesThatAreNotAListAreRefused) {
  EXPECT_THROW(
      static_cast<void>(WaveCatalog::fromText(
          calmFileWith("enemies", ENEMY_OUTSIDE_A_LIST), TEST_FILE_NAME)),
      InvalidEnemyListException);
}

/**
 * Given a waves file whose calm lists a number as an enemy
 * When it is read
 * Then an InvalidEnemyListException is thrown
 */
TEST(WaveCatalog, AnEnemyThatIsNotANameIsRefused) {
  EXPECT_THROW(
      static_cast<void>(WaveCatalog::fromText(
          calmFileWith("enemies", NUMBER_AS_AN_ENEMY), TEST_FILE_NAME)),
      InvalidEnemyListException);
}

/**
 * Given a waves file that does not exist
 * When it is read
 * Then an UnreadableWaveFileException names it by its absolute path
 */
TEST(WaveCatalog, AMissingFileIsNamedByItsAbsolutePath) {
  const std::filesystem::path file{MISSING_FILE};
  const std::string expected = std::string{UNREADABLE_FILE_START} +
                               std::filesystem::absolute(file).generic_string();

  try {
    static_cast<void>(WaveCatalog::fromFile(file));
    ADD_FAILURE() << "the missing file was read";
  } catch (const UnreadableWaveFileException& error) {
    EXPECT_EQ(std::string{error.what()}, expected);
  }
}

/**
 * Given the waves file the game ships
 * When it is read
 * Then it describes easy, hard and normal, the difficulty every game plays
 */
TEST(WaveCatalog, TheShippedFileDescribesEasyNormalAndHard) {
  const WaveCatalog catalog =
      WaveCatalog::fromFile(std::filesystem::path{RTYPE_SOURCE_ASSETS_FOLDER} /
                            SHIPPED_FILE_IN_ASSETS);

  EXPECT_EQ(catalog.difficulties(),
            (std::vector<std::string>{"easy", "hard", "normal"}));
}
