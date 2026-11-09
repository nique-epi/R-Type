#include <gtest/gtest.h>
#include <algorithm>
#include <array>
#include <filesystem>
#include <string>
#include <vector>
#include "EngineException.hpp"
#include "NonAnsiFolderTestConstants.hpp"
#include "ServerConstants.hpp"
#include "ServerException.hpp"
#include "ShippedEntitySheets.hpp"
#include "TemporaryFolder.hpp"
#include "WaveCatalog.hpp"
#include "WaveCatalogFixtures.hpp"
#include "WaveCatalogTestConstants.hpp"

using rtype::engine::DataFileSyntaxException;
using rtype::engine::DataFileTableExpectedException;
using rtype::engine::DataFileValueOutOfRangeException;
using rtype::engine::MissingDataFileKeyException;
using rtype::engine::UnknownDataFileKeyException;
using rtype::engine::UnreadableDataFileException;
using rtype::game::testing::shippedEntitySheets;
using rtype::server::InvalidDifficultyNameException;
using rtype::server::InvalidEnemyListException;
using rtype::server::OFFICIAL_WAVES_FINGERPRINT;
using rtype::server::TooManyDifficultiesException;
using rtype::server::UnknownDifficultyException;
using rtype::server::UnknownEnemyException;
using rtype::server::UnknownLookItemException;
using rtype::server::WaveCatalog;
using rtype::server::testing::A_COMMENT_LINE;
using rtype::server::testing::A_MISSILE_AS_AN_ENEMY;
using rtype::server::testing::AN_ENEMY_AS_A_LOOK_ITEM;
using rtype::server::testing::CALM_CURVE;
using rtype::server::testing::CALM_ENEMIES;
using rtype::server::testing::CALM_LOOK_ITEMS;
using rtype::server::testing::CALM_LOOK_ITEMS_VALUE;
using rtype::server::testing::calmFile;
using rtype::server::testing::calmFileNamed;
using rtype::server::testing::calmFileWith;
using rtype::server::testing::calmFileWithout;
using rtype::server::testing::DIFFICULTIES_PAST_THE_MOST_OF_A_FILE;
using rtype::server::testing::EMPTY_FILE;
using rtype::server::testing::ENEMY_OUTSIDE_A_LIST;
using rtype::server::testing::errorReading;
using rtype::server::testing::FEWEST_BYDOS;
using rtype::server::testing::FEWEST_WAVES;
using rtype::server::testing::fileOfDifficulties;
using rtype::server::testing::FRACTION;
using rtype::server::testing::INVALID_ENEMY_LIST_TEXT;
using rtype::server::testing::LEVEL_THAT_IS_NOT_A_TABLE;
using rtype::server::testing::LONGER_THAN_THE_FIRST_TEXT;
using rtype::server::testing::MISSING_DIFFICULTIES_TEXT;
using rtype::server::testing::MISSING_FILE;
using rtype::server::testing::MISSING_LEVEL_WAVES_TEXT;
using rtype::server::testing::MISSING_PAUSE_TEXT;
using rtype::server::testing::MISSPELT_KEY;
using rtype::server::testing::MISSPELT_KEY_MESSAGE;
using rtype::server::testing::MOST_BYDOS;
using rtype::server::testing::MOST_DIFFICULTIES_OF_A_FILE;
using rtype::server::testing::MOST_WAVES;
using rtype::server::testing::NO_BYDO;
using rtype::server::testing::NO_BYDO_MESSAGE;
using rtype::server::testing::NO_ENEMY;
using rtype::server::testing::NO_WAVE;
using rtype::server::testing::NO_WAVE_MESSAGE;
using rtype::server::testing::NOT_A_TABLE_TEXT;
using rtype::server::testing::NOT_A_TOML_VALUE;
using rtype::server::testing::NUMBER_AS_AN_ENEMY;
using rtype::server::testing::ONE_BYDO_TOO_MANY;
using rtype::server::testing::ONE_WAVE_TOO_MANY;
using rtype::server::testing::SEVENTEEN_BYTE_DIFFICULTY;
using rtype::server::testing::shippedCatalog;
using rtype::server::testing::SHORTEST_EQUAL_TO_THE_FIRST;
using rtype::server::testing::SHORTEST_LONGER_THAN_THE_FIRST;
using rtype::server::testing::SIXTEEN_BYTE_DIFFICULTY;
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
using rtype::server::testing::withCrlfLineEndings;

/**
 * Given a waves file describing storm, then calm
 * When it is read
 * Then each difficulty has the waves its table gives
 */
TEST(WaveCatalog, ReadsTheWavesOfEachDifficulty) {
  const WaveCatalog catalog =
      WaveCatalog::fromText(std::string{STORM_TABLE} + calmFile(),
                            TEST_FILE_NAME, shippedEntitySheets());

  EXPECT_EQ(catalog.planOf("calm").curve, CALM_CURVE);
  EXPECT_EQ(catalog.planOf("storm").curve, STORM_CURVE);
}

/**
 * Given a waves file describing storm, then calm
 * When it is read
 * Then the enemies of each difficulty are the kinds its list names, in order
 */
TEST(WaveCatalog, ReadsTheEnemiesOfEachDifficultyInOrder) {
  const WaveCatalog catalog =
      WaveCatalog::fromText(std::string{STORM_TABLE} + calmFile(),
                            TEST_FILE_NAME, shippedEntitySheets());

  EXPECT_TRUE(std::ranges::equal(catalog.planOf("calm").enemies, CALM_ENEMIES));
  EXPECT_TRUE(
      std::ranges::equal(catalog.planOf("storm").enemies, STORM_ENEMIES));
}

/**
 * Given a waves file whose calm lists the crown and the party hat as look
 * items
 * When it is read
 * Then the look items of calm are those two, in order
 */
TEST(WaveCatalog, ReadsTheLookItemsOfADifficulty) {
  const WaveCatalog catalog =
      WaveCatalog::fromText(calmFileWith("look_items", CALM_LOOK_ITEMS_VALUE),
                            TEST_FILE_NAME, shippedEntitySheets());

  EXPECT_TRUE(
      std::ranges::equal(catalog.planOf("calm").lookItems, CALM_LOOK_ITEMS));
}

/**
 * Given a waves file whose calm has no look_items
 * When it is read
 * Then calm has no look item
 */
TEST(WaveCatalog, ADifficultyWithoutLookItemsHasNone) {
  const WaveCatalog catalog =
      WaveCatalog::fromText(calmFile(), TEST_FILE_NAME, shippedEntitySheets());

  EXPECT_TRUE(catalog.planOf("calm").lookItems.empty());
}

/**
 * Given a waves file whose look items of calm name the weaver, an enemy
 * When it is read
 * Then an UnknownLookItemException is thrown: only a look item sheet counts
 */
TEST(WaveCatalog, RefusesAnEnemyAmongTheLookItems) {
  const std::string text = calmFileWith("look_items", AN_ENEMY_AS_A_LOOK_ITEM);

  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(text, TEST_FILE_NAME,
                                                       shippedEntitySheets())),
               UnknownLookItemException);
}

/**
 * Given a waves file describing calm and easy
 * When its difficulties are listed
 * Then easy comes first, before calm, which alphabetical order would put
 * ahead: a difficulty every server ships leads any other
 */
TEST(WaveCatalog, ListsTheShippedDifficultiesBeforeTheOthers) {
  const WaveCatalog catalog =
      WaveCatalog::fromText(calmFile() + calmFileNamed("easy"), TEST_FILE_NAME,
                            shippedEntitySheets());

  EXPECT_EQ(catalog.difficulties(), (std::vector<std::string>{"easy", "calm"}));
}

/**
 * Given a waves file describing storm, then calm
 * When its difficulties are listed
 * Then they come in alphabetical order, since neither is a difficulty every
 * server ships
 */
TEST(WaveCatalog, ListsOtherDifficultiesInAlphabeticalOrder) {
  const WaveCatalog catalog =
      WaveCatalog::fromText(std::string{STORM_TABLE} + calmFile(),
                            TEST_FILE_NAME, shippedEntitySheets());

  EXPECT_EQ(catalog.difficulties(),
            (std::vector<std::string>{"calm", "storm"}));
}

/**
 * Given a waves file describing calm and storm
 * When the waves of normal are asked for
 * Then an UnknownDifficultyException names the difficulties the file describes
 */
TEST(WaveCatalog, AnUnknownDifficultyNamesTheOnesDescribed) {
  const WaveCatalog catalog =
      WaveCatalog::fromText(std::string{STORM_TABLE} + calmFile(),
                            TEST_FILE_NAME, shippedEntitySheets());

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
 * Then a DataFileSyntaxException names the file and line 4
 */
TEST(WaveCatalog, TextThatIsNotTomlIsRefusedAtItsLine) {
  const std::string text =
      calmFileWith("first_spawn_interval_ms", NOT_A_TOML_VALUE);

  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(text, TEST_FILE_NAME,
                                                       shippedEntitySheets())),
               DataFileSyntaxException);
  EXPECT_TRUE(errorReading(text).starts_with(SYNTAX_ERROR_START));
}

/**
 * Given a waves file whose difficulty calm has no pause
 * When it is read
 * Then a MissingDataFileKeyException names the missing key
 */
TEST(WaveCatalog, AMissingValueIsNamed) {
  const std::string text = calmFileWithout("wave_pause_ms");

  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(text, TEST_FILE_NAME,
                                                       shippedEntitySheets())),
               MissingDataFileKeyException);
  EXPECT_NE(errorReading(text).find(MISSING_PAUSE_TEXT), std::string::npos);
}

/**
 * Given a waves file whose difficulty calm has no level_waves
 * When it is read
 * Then a MissingDataFileKeyException names the missing key
 */
TEST(WaveCatalog, AMissingLevelLengthIsNamed) {
  const std::string text = calmFileWithout("level_waves");

  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(text, TEST_FILE_NAME,
                                                       shippedEntitySheets())),
               MissingDataFileKeyException);
  EXPECT_NE(errorReading(text).find(MISSING_LEVEL_WAVES_TEXT),
            std::string::npos);
}

/**
 * Given a waves file whose difficulty calm has level_waves 1, then 100
 * When it is read
 * Then each level holds that many waves
 */
TEST(WaveCatalog, AcceptsTheFewestAndTheMostWavesInALevel) {
  const WaveCatalog shortest =
      WaveCatalog::fromText(calmFileWith("level_waves", FEWEST_WAVES),
                            TEST_FILE_NAME, shippedEntitySheets());
  const WaveCatalog longest =
      WaveCatalog::fromText(calmFileWith("level_waves", MOST_WAVES),
                            TEST_FILE_NAME, shippedEntitySheets());

  EXPECT_EQ(shortest.planOf("calm").curve.levelWaves, 1U);
  EXPECT_EQ(longest.planOf("calm").curve.levelWaves, 100U);
}

/**
 * Given a waves file whose difficulty calm has level_waves 0, then 101
 * When it is read
 * Then a DataFileValueOutOfRangeException names the key, its line and its
 * column
 */
TEST(WaveCatalog, RefusesALevelWithoutWavesOrWithOneTooMany) {
  const std::string noWave = calmFileWith("level_waves", NO_WAVE);
  const std::string oneTooMany = calmFileWith("level_waves", ONE_WAVE_TOO_MANY);

  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(noWave, TEST_FILE_NAME,
                                                       shippedEntitySheets())),
               DataFileValueOutOfRangeException);
  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(
                   oneTooMany, TEST_FILE_NAME, shippedEntitySheets())),
               DataFileValueOutOfRangeException);
  EXPECT_EQ(errorReading(noWave), NO_WAVE_MESSAGE);
}

/**
 * Given waves files whose one difficulty is named by 16 bytes, then by 17
 * When each is read
 * Then the first is read and the second is refused: the protocol carries a
 * difficulty named by 16 bytes at most
 */
TEST(WaveCatalog, RefusesADifficultyNameTheProtocolCannotCarry) {
  EXPECT_NO_THROW(static_cast<void>(
      WaveCatalog::fromText(calmFileNamed(SIXTEEN_BYTE_DIFFICULTY),
                            TEST_FILE_NAME, shippedEntitySheets())));
  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(
                   calmFileNamed(SEVENTEEN_BYTE_DIFFICULTY), TEST_FILE_NAME,
                   shippedEntitySheets())),
               InvalidDifficultyNameException);
}

/**
 * Given waves files of 16 difficulties, the most a server may hold, then 17
 * When each is read
 * Then the first is read and the second is refused
 */
TEST(WaveCatalog, RefusesMoreDifficultiesThanAServerMayHold) {
  EXPECT_NO_THROW(static_cast<void>(
      WaveCatalog::fromText(fileOfDifficulties(MOST_DIFFICULTIES_OF_A_FILE),
                            TEST_FILE_NAME, shippedEntitySheets())));
  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(
                   fileOfDifficulties(DIFFICULTIES_PAST_THE_MOST_OF_A_FILE),
                   TEST_FILE_NAME, shippedEntitySheets())),
               TooManyDifficultiesException);
}

/**
 * Given a waves file whose difficulty calm also holds a misspelt key, on line
 * 10
 * When it is read
 * Then an UnknownDataFileKeyException names the key, its line and its column
 */
TEST(WaveCatalog, AMisspeltKeyIsRefused) {
  const std::string text = calmFileWith(MISSPELT_KEY, FEWEST_BYDOS);

  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(text, TEST_FILE_NAME,
                                                       shippedEntitySheets())),
               UnknownDataFileKeyException);
  EXPECT_EQ(errorReading(text), MISSPELT_KEY_MESSAGE);
}

/**
 * Given an empty waves file
 * When it is read
 * Then a MissingDataFileKeyException says the table of the difficulties is
 * missing
 */
TEST(WaveCatalog, AFileWithoutDifficultiesIsRefused) {
  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(
                   EMPTY_FILE, TEST_FILE_NAME, shippedEntitySheets())),
               MissingDataFileKeyException);
  EXPECT_NE(errorReading(EMPTY_FILE).find(MISSING_DIFFICULTIES_TEXT),
            std::string::npos);
}

/**
 * Given a waves file whose difficulty calm is a number
 * When it is read
 * Then a DataFileTableExpectedException names it
 */
TEST(WaveCatalog, ADifficultyThatIsNotATableIsRefused) {
  EXPECT_THROW(
      static_cast<void>(WaveCatalog::fromText(
          LEVEL_THAT_IS_NOT_A_TABLE, TEST_FILE_NAME, shippedEntitySheets())),
      DataFileTableExpectedException);
  EXPECT_NE(errorReading(LEVEL_THAT_IS_NOT_A_TABLE).find(NOT_A_TABLE_TEXT),
            std::string::npos);
}

/**
 * Given a first wave of 1 Bydo, the fewest allowed
 * When the file is read
 * Then calm starts with 1 Bydo
 */
TEST(WaveCatalog, AcceptsTheFewestBydos) {
  const WaveCatalog catalog =
      WaveCatalog::fromText(calmFileWith("first_wave_bydos", FEWEST_BYDOS),
                            TEST_FILE_NAME, shippedEntitySheets());

  EXPECT_EQ(catalog.planOf("calm").curve.firstWave.entityCount, 1U);
}

/**
 * Given a first wave of 0 Bydos, one under the fewest
 * When the file is read
 * Then a DataFileValueOutOfRangeException names the file, line 2, the column
 * of the value, the key and its range
 */
TEST(WaveCatalog, RefusesAFirstWaveWithoutBydos) {
  const std::string text = calmFileWith("first_wave_bydos", NO_BYDO);

  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(text, TEST_FILE_NAME,
                                                       shippedEntitySheets())),
               DataFileValueOutOfRangeException);
  EXPECT_EQ(errorReading(text), NO_BYDO_MESSAGE);
}

/**
 * Given a first wave of 100 Bydos, the most allowed
 * When the file is read
 * Then calm starts with 100 Bydos
 */
TEST(WaveCatalog, AcceptsTheMostBydos) {
  const WaveCatalog catalog =
      WaveCatalog::fromText(calmFileWith("first_wave_bydos", MOST_BYDOS),
                            TEST_FILE_NAME, shippedEntitySheets());

  EXPECT_EQ(catalog.planOf("calm").curve.firstWave.entityCount, 100U);
}

/**
 * Given a first wave of 101 Bydos, one over the most
 * When the file is read
 * Then a DataFileValueOutOfRangeException is thrown
 */
TEST(WaveCatalog, RefusesOneBydoTooMany) {
  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(
                   calmFileWith("first_wave_bydos", ONE_BYDO_TOO_MANY),
                   TEST_FILE_NAME, shippedEntitySheets())),
               DataFileValueOutOfRangeException);
}

/**
 * Given a first interval of 2500.5 milliseconds
 * When the file is read
 * Then a DataFileValueOutOfRangeException is thrown: durations are whole
 * milliseconds
 */
TEST(WaveCatalog, RefusesAFraction) {
  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(
                   calmFileWith("first_spawn_interval_ms", FRACTION),
                   TEST_FILE_NAME, shippedEntitySheets())),
               DataFileValueOutOfRangeException);
}

/**
 * Given a first interval of 2500.0 milliseconds, a float that is whole
 * When the file is read
 * Then a DataFileValueOutOfRangeException is thrown: the keys take TOML
 * integers, as the file says
 */
TEST(WaveCatalog, RefusesAFloatEvenWhenWhole) {
  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(
                   calmFileWith("first_spawn_interval_ms", WHOLE_FLOAT),
                   TEST_FILE_NAME, shippedEntitySheets())),
               DataFileValueOutOfRangeException);
}

/**
 * Given a shortest interval equal to the first, 2500 milliseconds
 * When the file is read
 * Then calm keeps its Bydos 2.5 seconds apart in every wave
 */
TEST(WaveCatalog, AcceptsAShortestIntervalEqualToTheFirst) {
  const WaveCatalog catalog = WaveCatalog::fromText(
      calmFileWith("shortest_spawn_interval_ms", SHORTEST_EQUAL_TO_THE_FIRST),
      TEST_FILE_NAME, shippedEntitySheets());

  EXPECT_EQ(catalog.planOf("calm").curve.shortestSpawnInterval,
            CALM_CURVE.firstWave.spawnInterval);
}

/**
 * Given a shortest interval of 2501 milliseconds, one longer than the first
 * When the file is read
 * Then a DataFileValueOutOfRangeException gives the range up to the first
 * interval: the waves would slow down
 */
TEST(WaveCatalog, RefusesAShortestIntervalLongerThanTheFirst) {
  const std::string text = calmFileWith("shortest_spawn_interval_ms",
                                        SHORTEST_LONGER_THAN_THE_FIRST);

  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(text, TEST_FILE_NAME,
                                                       shippedEntitySheets())),
               DataFileValueOutOfRangeException);
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

  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(text, TEST_FILE_NAME,
                                                       shippedEntitySheets())),
               UnknownEnemyException);
  EXPECT_EQ(errorReading(text), UNKNOWN_ENEMY_MESSAGE);
}

/**
 * Given a waves file whose calm lists the sheet of a missile of a player
 * When it is read
 * Then an UnknownEnemyException is thrown: a wave makes enemies only
 */
TEST(WaveCatalog, ASheetThatIsNoEnemyIsRefused) {
  const std::string text = calmFileWith("enemies", A_MISSILE_AS_AN_ENEMY);

  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(text, TEST_FILE_NAME,
                                                       shippedEntitySheets())),
               UnknownEnemyException);
}

/**
 * Given a waves file whose calm lists no enemy
 * When it is read
 * Then an InvalidEnemyListException gives the length allowed
 */
TEST(WaveCatalog, AnEmptyEnemyListIsRefused) {
  const std::string text = calmFileWith("enemies", NO_ENEMY);

  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(text, TEST_FILE_NAME,
                                                       shippedEntitySheets())),
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
  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(
                   calmFileWith("enemies", ENEMY_OUTSIDE_A_LIST),
                   TEST_FILE_NAME, shippedEntitySheets())),
               InvalidEnemyListException);
}

/**
 * Given a waves file whose calm lists a number as an enemy
 * When it is read
 * Then an InvalidEnemyListException is thrown
 */
TEST(WaveCatalog, AnEnemyThatIsNotANameIsRefused) {
  EXPECT_THROW(static_cast<void>(WaveCatalog::fromText(
                   calmFileWith("enemies", NUMBER_AS_AN_ENEMY), TEST_FILE_NAME,
                   shippedEntitySheets())),
               InvalidEnemyListException);
}

/**
 * Given a waves file that does not exist
 * When it is read
 * Then an UnreadableDataFileException names it by its absolute path
 */
TEST(WaveCatalog, AMissingFileIsNamedByItsAbsolutePath) {
  const std::filesystem::path file{MISSING_FILE};
  const std::string expected = std::string{UNREADABLE_FILE_START} +
                               std::filesystem::absolute(file).generic_string();

  try {
    static_cast<void>(
        WaveCatalog::fromFiles(std::array{file}, shippedEntitySheets()));
    ADD_FAILURE() << "the missing file was read";
  } catch (const UnreadableDataFileException& error) {
    EXPECT_EQ(std::string{error.what()}, expected);
  }
}

/**
 * Given a missing waves file in a folder whose name is outside the code page
 * When it is read
 * Then the UnreadableDataFileException names it in UTF-8
 */
TEST(WaveCatalog, AMissingFileInANonAnsiFolderIsNamedInUtf8) {
  const TemporaryFolder folder;
  const std::filesystem::path file =
      folder.path() / rtype::testing::NON_ANSI_FOLDER_NAME / "waves.toml";

  try {
    static_cast<void>(
        WaveCatalog::fromFiles(std::array{file}, shippedEntitySheets()));
    ADD_FAILURE() << "the missing file was read";
  } catch (const UnreadableDataFileException& error) {
    EXPECT_NE(
        std::string{error.what()}.find(
            std::string{rtype::testing::NON_ANSI_FOLDER_BYTES} + "/waves.toml"),
        std::string::npos);
  }
}

/**
 * Given the waves file the game ships in its assets folder
 * When the file the server reads in that folder is read
 * Then it describes easy, normal and hard, from the easiest, as the new game
 * dialog lists them
 */
TEST(WaveCatalog, TheShippedFileDescribesEasyNormalAndHard) {
  const WaveCatalog catalog = shippedCatalog();

  EXPECT_EQ(catalog.difficulties(),
            (std::vector<std::string>{"easy", "normal", "hard"}));
}

/**
 * Given the waves file the game ships
 * When the file the server reads in its assets folder is read
 * Then its fingerprint is OFFICIAL_WAVES_FINGERPRINT, so a change of the file
 * fails here until the constant follows it
 */
TEST(WaveCatalog, TheShippedFileHasTheOfficialFingerprint) {
  const WaveCatalog catalog = shippedCatalog();

  EXPECT_EQ(catalog.fingerprint(), OFFICIAL_WAVES_FINGERPRINT);
}

/**
 * Given a waves file, and the same file with a comment added
 * When both are read
 * Then they describe the same difficulties, under two fingerprints
 */
TEST(WaveCatalog, AnAddedCommentChangesTheFingerprint) {
  const WaveCatalog plain =
      WaveCatalog::fromText(calmFile(), TEST_FILE_NAME, shippedEntitySheets());
  const WaveCatalog commented =
      WaveCatalog::fromText(calmFile() + std::string{A_COMMENT_LINE},
                            TEST_FILE_NAME, shippedEntitySheets());

  EXPECT_EQ(commented.difficulties(), plain.difficulties());
  EXPECT_NE(commented.fingerprint(), plain.fingerprint());
}

/**
 * Given a waves file, and the same file with CRLF line endings
 * When both are read
 * Then they describe the same difficulties, under two fingerprints, which is
 * why .gitattributes keeps the shipped file in LF
 */
TEST(WaveCatalog, CrlfLineEndingsChangeTheFingerprint) {
  const WaveCatalog plain =
      WaveCatalog::fromText(calmFile(), TEST_FILE_NAME, shippedEntitySheets());
  const WaveCatalog crlf = WaveCatalog::fromText(
      withCrlfLineEndings(calmFile()), TEST_FILE_NAME, shippedEntitySheets());

  EXPECT_EQ(crlf.difficulties(), plain.difficulties());
  EXPECT_NE(crlf.fingerprint(), plain.fingerprint());
}
