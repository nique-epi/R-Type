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
