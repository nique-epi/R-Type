#include <gtest/gtest.h>
#include <filesystem>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "CommandInterpreter.hpp"
#include "CommandResult.hpp"
#include "EngineException.hpp"
#include "GamePlayers.hpp"
#include "GameState.hpp"
#include "GamesSettings.hpp"
#include "MessageType.hpp"
#include "RecordingPlayerAdmission.hpp"
#include "SentDatagrams.hpp"
#include "ServerConstants.hpp"
#include "ServerGamesFixture.hpp"
#include "ServerModuleTestConstants.hpp"

using rtype::engine::ModuleStartFailedException;
using rtype::network::MessageType;
using rtype::server::IMPOSED_SEED_BREACH;
using rtype::server::testing::playerEndpoint;

/**
 * Given a waves file that does not exist
 * When the application starts
 * Then start() fails with an error that names the games module and the file
 */
TEST_F(ServerGamesFixture, FailsToStartNamingTheWavesFileWhenItCannotBeRead) {
  try {
    startWith(rtype::server::GamesSettings{
        .seed = SOME_SEED,
        .wavesFiles = {MISSING_WAVES_FILE},
        .contentFolders = shippedWaves().contentFolders});
    FAIL() << "the application started without a waves file";
  } catch (const ModuleStartFailedException& error) {
    const std::string text = error.what();
    EXPECT_NE(text.find("games"), std::string::npos);
    EXPECT_NE(text.find(MISSING_WAVES_FILE), std::string::npos);
  }
}

/**
 * Given a content folder that does not exist, and the shipped waves
 * file
 * When the application starts
 * Then start() fails with an error that names the games module and the folder
 */
TEST_F(ServerGamesFixture, FailsToStartNamingTheSheetsFolderWhenItIsMissing) {
  try {
    startWith(rtype::server::GamesSettings{
        .seed = SOME_SEED,
        .wavesFiles = shippedWaves().wavesFiles,
        .contentFolders = {MISSING_CONTENT_FOLDER}});
    FAIL() << "the application started without entity sheets";
  } catch (const ModuleStartFailedException& error) {
    const std::string text = error.what();
    EXPECT_NE(text.find("games"), std::string::npos);
    EXPECT_NE(text.find(MISSING_CONTENT_FOLDER), std::string::npos);
  }
}

/**
 * Given the waves file shipped with the game
 * When the application starts
 * Then the game exists and waits for players
 */
TEST_F(ServerGamesFixture, CreatesTheGameFromTheShippedWavesFile) {
  startWith(shippedWaves());

  EXPECT_EQ(probe().runningGame()->game().state(),
            rtype::server::GameState::WaitingForPlayers);
}

/**
 * Given the shipped waves file and no seed
 * When the application starts
 * Then the journal says the game is vanilla
 */
TEST_F(ServerGamesFixture, SaysTheGameIsVanillaWithoutASeed) {
  rtype::server::GamesSettings settings = shippedWaves();
  settings.seed = std::nullopt;

  startWith(std::move(settings));

  EXPECT_EQ(journal().linesHolding(VANILLA_GAMES_LINE).size(), 1U);
}

/**
 * Given the shipped waves file, no seed, and a configuration that sets the
 * difficulty "hard", five lives and friendly fire on
 * When the application starts
 * Then the journal says the game plays those rules, and is not vanilla
 */
TEST_F(ServerGamesFixture, PlaysTheRulesTheConfigurationSets) {
  rtype::server::GamesSettings settings = shippedWaves();
  settings.seed = std::nullopt;

  startWith(std::move(settings),
            [](rtype::engine::CommandInterpreter& commands) {
              static_cast<void>(commands.execute(SET_HARD_GAME_LINE));
              static_cast<void>(commands.execute(SET_FIVE_LIVES_GAME_LINE));
              static_cast<void>(commands.execute(SET_FRIENDLY_FIRE_GAME_LINE));
            });

  EXPECT_EQ(journal().linesHolding(RULED_GAMES_LINE).size(), 1U);
}

/**
 * Given the shipped waves file and a configuration that sets a difficulty it
 * has no table for
 * When the application starts
 * Then start() fails: the game has no waves to play
 */
TEST_F(ServerGamesFixture, FailsToStartOnADifficultyTheWavesFileHasNot) {
  EXPECT_THROW(startWith(shippedWaves(),
                         [](rtype::engine::CommandInterpreter& commands) {
                           static_cast<void>(
                               commands.execute(SET_UNKNOWN_DIFFICULTY_LINE));
                         }),
               ModuleStartFailedException);
}

/**
 * Given the shipped waves file and a seed
 * When the application starts
 * Then the journal says the seed makes the game not vanilla
 */
TEST_F(ServerGamesFixture, SaysASeedMakesTheGameNotVanilla) {
  startWith(shippedWaves());

  EXPECT_EQ(journal().linesHolding(IMPOSED_SEED_BREACH).size(), 1U);
}

/**
 * Given a player in the game
 * When its Disconnection reaches a tick
 * Then the network is told to forget that player, with its address and token
 */
TEST_F(ServerGamesFixture, MakesTheNetworkForgetAPlayerWhoseSessionClosed) {
  startWith(shippedWaves());
  join(0);

  runTick({disconnectionFrom(0)});

  EXPECT_EQ(network().admission().forgotten(),
            (std::vector<AdmissionCall>{AdmissionCall{
                .player = playerEndpoint(0), .sessionToken = tokenOf(0)}}));
}

/**
 * Given two players in the game
 * When one of them leaves
 * Then the other one is told it left by the tick that closed the session: the
 * bus delivers the departure and the PlayerLeft is guaranteed at once
 */
TEST_F(ServerGamesFixture, TellsTheOthersThatAPlayerLeftInTheSameTick) {
  startWith(shippedWaves());
  join(0);
  join(1);

  runTick({disconnectionFrom(0)});

  EXPECT_EQ(datagramsTo(1, MessageType::PlayerLeft), 1U);
}

/**
 * Given a player in the game
 * When the application stops
 * Then the player got the three copies of a Disconnection
 */
TEST_F(ServerGamesFixture, SaysGoodbyeToThePlayersWhenTheApplicationStops) {
  startWith(shippedWaves());
  join(0);

  application().stop();

  EXPECT_EQ(datagramsTo(0, MessageType::Disconnection), 3U);
}

/**
 * Given the games module installed
 * When the configuration gives the state rate a rate above the highest
 * Then the line is refused, so the module declared the setting with its bounds
 */
TEST_F(ServerGamesFixture, DeclaresTheStateRateSettingWithItsBounds) {
  rtype::engine::CommandResult result;

  startWith(shippedWaves(),
            [&result](rtype::engine::CommandInterpreter& commands) {
              result = commands.execute(STATE_RATE_ABOVE_THE_HIGHEST_LINE);
            });

  EXPECT_FALSE(result.accepted);
}

/**
 * Given the games module installed
 * When the configuration clears the seed
 * Then the line is accepted, so the module declared the seed setting
 */
TEST_F(ServerGamesFixture, DeclaresTheSeedSettingAndLetsItBeCleared) {
  rtype::engine::CommandResult result;

  startWith(shippedWaves(),
            [&result](rtype::engine::CommandInterpreter& commands) {
              result = commands.execute(CLEAR_THE_SEED_LINE);
            });

  EXPECT_TRUE(result.accepted);
}

/**
 * Given the games module installed
 * When the configuration gives the state encoding a value that names no
 * encoding
 * Then the line is refused, so the module declared the setting with its values
 */
TEST_F(ServerGamesFixture, RefusesAStateEncodingThatNamesNoEncoding) {
  rtype::engine::CommandResult result;

  startWith(shippedWaves(),
            [&result](rtype::engine::CommandInterpreter& commands) {
              result = commands.execute(UNKNOWN_STATE_ENCODING_LINE);
            });

  EXPECT_FALSE(result.accepted);
}

/**
 * Given the games module installed
 * When the configuration asks for every state in full
 * Then the line is accepted, so the module declared the setting
 */
TEST_F(ServerGamesFixture, AcceptsFullAsTheStateEncoding) {
  rtype::engine::CommandResult result;

  startWith(shippedWaves(),
            [&result](rtype::engine::CommandInterpreter& commands) {
              result = commands.execute(FULL_STATE_ENCODING_LINE);
            });

  EXPECT_TRUE(result.accepted);
}

/**
 * Given the shipped assets and the example resource, whose waves send its
 * crimson Drifter
 * When the application starts with the waves of the resource laid over those
 * of the assets
 * Then the games start: the waves name a sheet the resource brings
 */
TEST_F(ServerGamesFixture, StartsWithTheSheetsAndTheWavesOfTheExampleResource) {
  const std::filesystem::path resource =
      std::filesystem::path{RTYPE_SOURCE_ASSETS_FOLDER} /
      EXAMPLE_RESOURCE_FROM_THE_ASSETS;

  EXPECT_NO_THROW(startWith(rtype::server::GamesSettings{
      .seed = SOME_SEED,
      .wavesFiles = {std::filesystem::path{RTYPE_SOURCE_ASSETS_FOLDER} /
                         WAVES_OF_THE_EXAMPLE_RESOURCE,
                     resource / WAVES_OF_THE_EXAMPLE_RESOURCE},
      .contentFolders = {std::filesystem::path{RTYPE_SOURCE_ASSETS_FOLDER},
                         resource}}));
}
