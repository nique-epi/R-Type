#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <array>
#include <filesystem>
#include <string>
#include <vector>
#include "BehaviourScriptFiles.hpp"
#include "BehaviourScripts.hpp"
#include "BehaviourScriptsFixtures.hpp"
#include "EngineException.hpp"
#include "LuauLevelConstants.hpp"
#include "LuauLevelEntry.hpp"
#include "LuauLevelsTestConstants.hpp"

using rtype::engine::ScriptException;
using rtype::game::BehaviourScripts;
using rtype::game::LuauLevelEntry;
using ::testing::ElementsAre;
using ::testing::HasSubstr;
using ::testing::IsEmpty;
using ::testing::ThrowsMessage;

/**
 * Given a script that registers a level of two stages
 * When the stages of that level are asked for
 * Then both are kept
 */
TEST(LuauLevels, KeepsTheStagesOfARegisteredLevel) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  scripts.run(scriptsOf(REGISTERS_TWO_LEVELS));

  const LuauLevelEntry* const entry = scripts.levels().entryOf(CAVES_LEVEL);
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(entry->stages.size(), STAGES_OF_CAVES);
}

/**
 * Given the scripts of the resource training-run the repository ships
 * When they run
 * Then the level training_run is registered, with its two stages
 */
TEST(LuauLevels, ReadsTheLevelTheRepositoryShips) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  const std::array folders{
      std::filesystem::path{RTYPE_SOURCE_RESOURCES_FOLDER} /
      SHIPPED_LEVEL_RESOURCE};

  scripts.run(rtype::game::scriptSourcesIn(folders));

  const LuauLevelEntry* const entry = scripts.levels().entryOf(SHIPPED_LEVEL);
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(entry->stages.size(), STAGES_OF_THE_SHIPPED_LEVEL);
}

/**
 * Given the scripts of the resource fosse the repository ships
 * When they run
 * Then the level fosse is registered, with its three stages
 */
TEST(LuauLevels, ReadsTheFosseLevelTheRepositoryShips) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  const std::array folders{
      std::filesystem::path{RTYPE_SOURCE_RESOURCES_FOLDER} / FOSSE_RESOURCE};

  scripts.run(rtype::game::scriptSourcesIn(folders));

  const LuauLevelEntry* const entry = scripts.levels().entryOf(FOSSE_LEVEL);
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(entry->stages.size(), STAGES_OF_THE_FOSSE_LEVEL);
}

/**
 * Given the scripts of the resource fosse the repository ships
 * When they run
 * Then the level fosse is the map THE PIT
 */
TEST(LuauLevels, ReadsTheMapOfTheFosseLevel) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  const std::array folders{
      std::filesystem::path{RTYPE_SOURCE_RESOURCES_FOLDER} / FOSSE_RESOURCE};

  scripts.run(rtype::game::scriptSourcesIn(folders));

  const LuauLevelEntry* const entry = scripts.levels().entryOf(FOSSE_LEVEL);
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(entry->mapName.value_or(""), FOSSE_MAP);
}

/**
 * Given the scripts of the resource fosse the repository ships
 * When they run
 * Then the level space is registered, with its two stages
 */
TEST(LuauLevels, ReadsTheSpaceLevelTheRepositoryShips) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  const std::array folders{
      std::filesystem::path{RTYPE_SOURCE_RESOURCES_FOLDER} / FOSSE_RESOURCE};

  scripts.run(rtype::game::scriptSourcesIn(folders));

  const LuauLevelEntry* const entry = scripts.levels().entryOf(SPACE_LEVEL);
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(entry->stages.size(), STAGES_OF_THE_SPACE_LEVEL);
}

/**
 * Given the scripts of the resource fosse the repository ships
 * When they run
 * Then the level space is the map OPEN SPACE
 */
TEST(LuauLevels, ReadsTheMapOfTheSpaceLevel) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  const std::array folders{
      std::filesystem::path{RTYPE_SOURCE_RESOURCES_FOLDER} / FOSSE_RESOURCE};

  scripts.run(rtype::game::scriptSourcesIn(folders));

  const LuauLevelEntry* const entry = scripts.levels().entryOf(SPACE_LEVEL);
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(entry->mapName.value_or(""), SPACE_MAP);
}

/**
 * Given a script that registers a level as the map THE CAVES
 * When it runs
 * Then the level is that map
 */
TEST(LuauLevels, ReadsTheMapOfALevel) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  scripts.run(scriptsOf(REGISTERS_A_MAP));

  const LuauLevelEntry* const entry = scripts.levels().entryOf(CAVES_LEVEL);
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(entry->mapName.value_or(""), CAVES_MAP);
}

/**
 * Given a script that registers levels without the option map
 * When it runs
 * Then a level is no map
 */
TEST(LuauLevels, ALevelWithoutTheOptionIsNoMap) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  scripts.run(scriptsOf(REGISTERS_TWO_LEVELS));

  const LuauLevelEntry* const entry = scripts.levels().entryOf(CAVES_LEVEL);
  ASSERT_NE(entry, nullptr);
  EXPECT_FALSE(entry->mapName.has_value());
}

/**
 * Given a script whose map name is written in small letters
 * When it runs
 * Then it fails, saying what the option is
 */
TEST(LuauLevels, RefusesAMapNameInSmallLetters) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  EXPECT_THAT([&] { scripts.run(scriptsOf(REGISTERS_A_MAP_IN_SMALL_LETTERS)); },
              ThrowsMessage<ScriptException>(
                  HasSubstr(rtype::game::WRONG_LEVEL_MAP_MESSAGE)));
}

/**
 * Given a script whose map name is seventeen characters long
 * When it runs
 * Then it fails, saying what the option is
 */
TEST(LuauLevels, RefusesAMapNameTooLong) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  EXPECT_THAT([&] { scripts.run(scriptsOf(REGISTERS_A_MAP_NAME_TOO_LONG)); },
              ThrowsMessage<ScriptException>(
                  HasSubstr(rtype::game::WRONG_LEVEL_MAP_MESSAGE)));
}

/**
 * Given a script whose map name is a number
 * When it runs
 * Then it fails, saying what the option is
 */
TEST(LuauLevels, RefusesAMapThatIsNoText) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  EXPECT_THAT([&] { scripts.run(scriptsOf(REGISTERS_A_MAP_THAT_IS_NO_TEXT)); },
              ThrowsMessage<ScriptException>(
                  HasSubstr(rtype::game::WRONG_LEVEL_MAP_MESSAGE)));
}

/**
 * Given a script that opens a level in the biome caves
 * When it runs
 * Then the level opens in caves
 */
TEST(LuauLevels, ReadsTheBiomeALevelOpensIn) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  scripts.run(scriptsOf(REGISTERS_AN_OPENING_BIOME));

  const LuauLevelEntry* const entry = scripts.levels().entryOf(CAVES_LEVEL);
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(entry->openingBiome.value_or(""), CAVES_BIOME_NAME);
}

/**
 * Given a script that opens a level in a biome whose name holds capitals and
 * a space
 * When it runs
 * Then it fails, saying what a biome name is
 */
TEST(LuauLevels, RefusesAnOpeningBiomeThatIsNoBiomeName) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  EXPECT_THAT([&] { scripts.run(scriptsOf(REGISTERS_A_WRONG_OPENING_BIOME)); },
              ThrowsMessage<ScriptException>(
                  HasSubstr(rtype::game::WRONG_BIOME_NAME_MESSAGE)));
}

/**
 * Given a script that registers two levels
 * When the names of the levels are asked for
 * Then both are listed, in alphabetical order
 */
TEST(LuauLevels, ListsTheNamesOfTheLevels) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  scripts.run(scriptsOf(REGISTERS_TWO_LEVELS));

  EXPECT_THAT(scripts.levels().names(), ElementsAre(CAVES_LEVEL, WRECK_LEVEL));
}

/**
 * Given scripts that register two levels
 * When the stages of another level are asked for
 * Then there are none
 */
TEST(LuauLevels, KnowsNoLevelNoScriptRegistered) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  scripts.run(scriptsOf(REGISTERS_TWO_LEVELS));

  EXPECT_EQ(scripts.levels().entryOf(UNKNOWN_LEVEL), nullptr);
}

/**
 * Given a script that registers the same level twice
 * When it runs
 * Then it fails, saying the level is already registered
 */
TEST(LuauLevels, RefusesALevelRegisteredTwice) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  EXPECT_THAT([&] { scripts.run(scriptsOf(REGISTERS_A_LEVEL_TWICE)); },
              ThrowsMessage<ScriptException>(
                  HasSubstr(rtype::game::DUPLICATE_LEVEL_MESSAGE)));
}

/**
 * Given a script that registers a level without a name
 * When it runs
 * Then it fails, saying a level needs a name
 */
TEST(LuauLevels, RefusesALevelWithoutName) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  EXPECT_THAT([&] { scripts.run(scriptsOf(REGISTERS_A_LEVEL_WITHOUT_NAME)); },
              ThrowsMessage<ScriptException>(
                  HasSubstr(rtype::game::EMPTY_LEVEL_NAME_MESSAGE)));
}

/**
 * Given a script that registers a level without a stage
 * When it runs
 * Then it fails, saying what the stages of a level are
 */
TEST(LuauLevels, RefusesALevelWithoutStage) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  EXPECT_THAT([&] { scripts.run(scriptsOf(REGISTERS_A_LEVEL_WITHOUT_STAGE)); },
              ThrowsMessage<ScriptException>(
                  HasSubstr(rtype::game::WRONG_LEVEL_STAGES_MESSAGE)));
}

/**
 * Given a script that registers a level whose second stage is a number
 * When it runs
 * Then it fails, saying what the stages of a level are
 */
TEST(LuauLevels, RefusesAStageThatIsNoFunction) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  EXPECT_THAT(
      [&] { scripts.run(scriptsOf(REGISTERS_A_STAGE_THAT_IS_NO_FUNCTION)); },
      ThrowsMessage<ScriptException>(
          HasSubstr(rtype::game::WRONG_LEVEL_STAGES_MESSAGE)));
}

/**
 * Given a script that waits while it is first read
 * When it runs
 * Then it fails, saying a wait only runs in a stage
 */
TEST(LuauLevels, RefusesAWaitOutsideAStage) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  EXPECT_THAT([&] { scripts.run(scriptsOf(WAITS_WHILE_READ)); },
              ThrowsMessage<ScriptException>(
                  HasSubstr(rtype::game::LEVEL_WAIT_OUTSIDE_STAGE_MESSAGE)));
}

/**
 * Given a script that reads the scroll while it is first read
 * When it runs
 * Then it fails, saying the level is only reached during a game
 */
TEST(LuauLevels, RefusesToReadTheScrollOutsideAGame) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  EXPECT_THAT([&] { scripts.run(scriptsOf(READS_THE_SCROLL_WHILE_READ)); },
              ThrowsMessage<ScriptException>(
                  HasSubstr(rtype::game::NO_LEVEL_PLAYED_MESSAGE)));
}

/**
 * Given a script that counts the enemies while it is first read
 * When it runs
 * Then it fails, saying the level is only reached during a game
 */
TEST(LuauLevels, RefusesToCountTheEnemiesOutsideAGame) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  EXPECT_THAT([&] { scripts.run(scriptsOf(COUNTS_THE_ENEMIES_WHILE_READ)); },
              ThrowsMessage<ScriptException>(
                  HasSubstr(rtype::game::NO_LEVEL_PLAYED_MESSAGE)));
}

/**
 * Given a script that registers a level with the option ldtk
 * When the entry of that level is asked for
 * Then it names that file
 */
TEST(LuauLevels, KeepsTheLdtkFileOfALevel) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  scripts.run(scriptsOf(REGISTERS_A_LEVEL_WITH_AN_LDTK_FILE));

  const LuauLevelEntry* const entry = scripts.levels().entryOf(CAVES_LEVEL);
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(entry->ldtkFile.value_or(std::string{}), CAVES_LDTK_FILE);
}

/**
 * Given a script that registers a level without options
 * When the entry of that level is asked for
 * Then it names no LDtk file and places no enemy
 */
TEST(LuauLevels, ALevelWithoutOptionsHasNoLdtkFile) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  scripts.run(scriptsOf(REGISTERS_TWO_LEVELS));

  const LuauLevelEntry* const entry = scripts.levels().entryOf(CAVES_LEVEL);
  ASSERT_NE(entry, nullptr);
  EXPECT_FALSE(entry->ldtkFile.has_value());
  EXPECT_THAT(entry->placements, IsEmpty());
}

/**
 * Given the scripts of the resource training-run the repository ships
 * When they run
 * Then the level training_run names its LDtk file
 */
TEST(LuauLevels, TheShippedLevelNamesItsLdtkFile) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  const std::array folders{
      std::filesystem::path{RTYPE_SOURCE_RESOURCES_FOLDER} /
      SHIPPED_LEVEL_RESOURCE};

  scripts.run(rtype::game::scriptSourcesIn(folders));

  const LuauLevelEntry* const entry = scripts.levels().entryOf(SHIPPED_LEVEL);
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(entry->ldtkFile.value_or(std::string{}), SHIPPED_LEVEL_LDTK_FILE);
}

/**
 * Given a script whose LDtk file leaves its content folder with `..`
 * When it runs
 * Then it fails, saying what the option is
 */
TEST(LuauLevels, RefusesAnLdtkFileThatLeavesTheContent) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  EXPECT_THAT(
      [&] { scripts.run(scriptsOf(REGISTERS_AN_LDTK_FILE_ABOVE_THE_CONTENT)); },
      ThrowsMessage<ScriptException>(
          HasSubstr(rtype::game::WRONG_LDTK_FILE_MESSAGE)));
}

/**
 * Given a script whose LDtk file is an absolute path
 * When it runs
 * Then it fails, saying what the option is
 */
TEST(LuauLevels, RefusesAnAbsoluteLdtkFile) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  EXPECT_THAT([&] { scripts.run(scriptsOf(REGISTERS_AN_ABSOLUTE_LDTK_FILE)); },
              ThrowsMessage<ScriptException>(
                  HasSubstr(rtype::game::WRONG_LDTK_FILE_MESSAGE)));
}

/**
 * Given a script whose LDtk option is a number
 * When it runs
 * Then it fails, saying what the option is
 */
TEST(LuauLevels, RefusesAnLdtkFileThatIsNoText) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  EXPECT_THAT(
      [&] { scripts.run(scriptsOf(REGISTERS_AN_LDTK_FILE_THAT_IS_NO_TEXT)); },
      ThrowsMessage<ScriptException>(
          HasSubstr(rtype::game::WRONG_LDTK_FILE_MESSAGE)));
}

/**
 * Given a script that registers a level with an option no level has
 * When it runs
 * Then it fails, saying which options a level has
 */
TEST(LuauLevels, RefusesAnUnknownOption) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  EXPECT_THAT([&] { scripts.run(scriptsOf(REGISTERS_AN_UNKNOWN_OPTION)); },
              ThrowsMessage<ScriptException>(
                  HasSubstr(rtype::game::WRONG_LEVEL_OPTIONS_MESSAGE)));
}

/**
 * Given a script that reads the placements while it is first read
 * When it runs
 * Then it fails, saying the level is only reached during a game
 */
TEST(LuauLevels, RefusesToReadThePlacementsOutsideAGame) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  EXPECT_THAT([&] { scripts.run(scriptsOf(READS_THE_PLACEMENTS_WHILE_READ)); },
              ThrowsMessage<ScriptException>(
                  HasSubstr(rtype::game::NO_LEVEL_PLAYED_MESSAGE)));
}

/**
 * Given a script that asks for the placed enemies while it is first read
 * When it runs
 * Then it fails, saying the level is only reached during a game
 */
TEST(LuauLevels, RefusesToSpawnThePlacedOutsideAGame) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  EXPECT_THAT([&] { scripts.run(scriptsOf(SPAWNS_THE_PLACED_WHILE_READ)); },
              ThrowsMessage<ScriptException>(
                  HasSubstr(rtype::game::NO_LEVEL_PLAYED_MESSAGE)));
}

/**
 * Given a script that declares the band of the biome pit
 * When the entry of its level is asked for
 * Then it holds that band for that biome
 */
TEST(LuauLevels, KeepsTheBandOfABiome) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  scripts.run(scriptsOf(REGISTERS_A_BAND));

  const LuauLevelEntry* const entry = scripts.levels().entryOf(CAVES_LEVEL);
  ASSERT_NE(entry, nullptr);
  ASSERT_TRUE(entry->bands.contains(BANDED_BIOME));
  EXPECT_EQ(entry->bands.find(BANDED_BIOME)->second, DECLARED_BAND);
}

/**
 * Given the scripts of the resource fosse
 * When they run
 * Then the level fosse closes its biome with the ceiling and the floor of the
 * Pit
 */
TEST(LuauLevels, ClosesThePitWithItsCeilingAndItsFloor) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};
  const std::array folders{
      std::filesystem::path{RTYPE_SOURCE_RESOURCES_FOLDER} / FOSSE_RESOURCE};

  scripts.run(rtype::game::scriptSourcesIn(folders));

  const LuauLevelEntry* const entry = scripts.levels().entryOf(FOSSE_LEVEL);
  ASSERT_NE(entry, nullptr);
  ASSERT_TRUE(entry->bands.contains(FOSSE_BIOME));
  EXPECT_EQ(entry->bands.find(FOSSE_BIOME)->second, FOSSE_BAND);
}

/**
 * Given a script whose band is narrower than a ship
 * When it runs
 * Then it fails, saying what the bands of a level are
 */
TEST(LuauLevels, RefusesABandNarrowerThanAShip) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  EXPECT_THAT(
      [&] { scripts.run(scriptsOf(REGISTERS_A_BAND_NARROWER_THAN_A_SHIP)); },
      ThrowsMessage<ScriptException>(
          HasSubstr(rtype::game::WRONG_LEVEL_BANDS_MESSAGE)));
}

/**
 * Given a script whose band has its floor below the play area
 * When it runs
 * Then it fails, saying what the bands of a level are
 */
TEST(LuauLevels, RefusesABandBelowThePlayArea) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  EXPECT_THAT(
      [&] { scripts.run(scriptsOf(REGISTERS_A_BAND_BELOW_THE_PLAY_AREA)); },
      ThrowsMessage<ScriptException>(
          HasSubstr(rtype::game::WRONG_LEVEL_BANDS_MESSAGE)));
}

/**
 * Given a script whose band names no biome
 * When it runs
 * Then it fails, saying what the bands of a level are
 */
TEST(LuauLevels, RefusesABandOfNoBiomeName) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  EXPECT_THAT(
      [&] { scripts.run(scriptsOf(REGISTERS_A_BAND_OF_NO_BIOME_NAME)); },
      ThrowsMessage<ScriptException>(
          HasSubstr(rtype::game::WRONG_LEVEL_BANDS_MESSAGE)));
}

/**
 * Given a script that declares a start for the first stage of its level
 * When it runs
 * Then it fails, saying what the starts of a level are
 */
TEST(LuauLevels, RefusesAStartForTheFirstStage) {
  FailureLog log;
  BehaviourScripts scripts{reportInto(log)};

  EXPECT_THAT(
      [&] { scripts.run(scriptsOf(REGISTERS_A_START_FOR_THE_FIRST_STAGE)); },
      ThrowsMessage<ScriptException>(
          HasSubstr(rtype::game::WRONG_LEVEL_STARTS_MESSAGE)));
}
