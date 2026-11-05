#include <gtest/gtest.h>
#include <cstddef>
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
    startWith(rtype::server::GamesSettings{.seed = SOME_SEED,
                                           .wavesFile = MISSING_WAVES_FILE});
    FAIL() << "the application started without a waves file";
  } catch (const ModuleStartFailedException& error) {
    const std::string text = error.what();
    EXPECT_NE(text.find("games"), std::string::npos);
    EXPECT_NE(text.find(MISSING_WAVES_FILE), std::string::npos);
  }
}

/**
 * Given the waves file shipped with the game
 * When the application starts
 * Then the first game exists and waits for players
 */
TEST_F(ServerGamesFixture, CreatesTheFirstGameFromTheShippedWavesFile) {
  startWith(shippedWaves());

  EXPECT_EQ(probe().games()->joinable().state(),
            rtype::server::GameState::WaitingForPlayers);
}

/**
 * Given the shipped waves file and no seed
 * When the application starts
 * Then the journal says the games are vanilla
 */
TEST_F(ServerGamesFixture, SaysTheGamesAreVanillaWithoutASeed) {
  rtype::server::GamesSettings settings = shippedWaves();
  settings.seed = std::nullopt;

  startWith(std::move(settings));

  EXPECT_EQ(journal().linesHolding(VANILLA_GAMES_LINE).size(), 1U);
}

/**
 * Given the shipped waves file and a seed
 * When the application starts
 * Then the journal says the seed makes the games not vanilla
 */
TEST_F(ServerGamesFixture, SaysASeedMakesTheGamesNotVanilla) {
  startWith(shippedWaves());

  EXPECT_EQ(journal().linesHolding(IMPOSED_SEED_BREACH).size(), 1U);
}

/**
 * Given settings that ask for three games
 * When the application starts
 * Then there are three games, all waiting for players
 */
TEST_F(ServerGamesFixture, CreatesAsManyGamesAsTheSettingsAsk) {
  rtype::server::GamesSettings settings = shippedWaves();
  settings.gameCount = THREE_GAMES;

  startWith(std::move(settings));

  ASSERT_EQ(probe().games()->all().size(), THREE_GAMES);
  for (const auto& game : probe().games()->all()) {
    EXPECT_EQ(game->state(), rtype::server::GameState::WaitingForPlayers);
  }
}

/**
 * Given a player in the first game
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
 * Given two players in the first game
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
 * Given two players in the second game
 * When one of them leaves
 * Then the other one is told it left: the second game reads the datagrams of
 * its players, and its own bus delivers its events
 */
TEST_F(ServerGamesFixture, RunsTheSecondGameWithItsOwnPlayersAndBus) {
  startWith(shippedWaves());
  seatPlayerIn(*probe().games()->all()[SECOND_GAME], 0);
  seatPlayerIn(*probe().games()->all()[SECOND_GAME], 1);

  runTick({disconnectionFrom(0)});

  EXPECT_EQ(datagramsTo(1, MessageType::PlayerLeft), 1U);
}

/**
 * Given one player in the first game and one in the second
 * When the player of the first game leaves
 * Then the player of the second game is told nothing: the games do not share
 * a bus
 */
TEST_F(ServerGamesFixture, KeepsTheEventsOfAGameAwayFromTheOtherGames) {
  startWith(shippedWaves());
  seatPlayerIn(*probe().games()->all()[FIRST_GAME], 0);
  seatPlayerIn(*probe().games()->all()[SECOND_GAME], 1);

  runTick({disconnectionFrom(0)});

  EXPECT_EQ(datagramsTo(1, MessageType::PlayerLeft), 0U);
}

/**
 * Given settings that ask for no game
 * When the application starts
 * Then start() fails with an error that names the games module
 */
TEST_F(ServerGamesFixture, FailsToStartWhenTheSettingsAskForNoGame) {
  rtype::server::GamesSettings settings = shippedWaves();
  settings.gameCount = 0;

  EXPECT_THROW(startWith(std::move(settings)), ModuleStartFailedException);
}

/**
 * Given a player in the first game
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
 * Given settings that ask for the most games a heartbeat lists
 * When the application starts
 * Then every game is created
 */
TEST_F(ServerGamesFixture, StartsTheMostGamesAHeartbeatLists) {
  rtype::server::GamesSettings settings = shippedWaves();
  settings.gameCount = MOST_GAMES_A_HEARTBEAT_LISTS;

  startWith(std::move(settings));

  EXPECT_EQ(probe().games()->all().size(), MOST_GAMES_A_HEARTBEAT_LISTS);
}

/**
 * Given settings that ask for one game more than a heartbeat lists
 * When the application starts
 * Then start() fails, since the master would refuse that heartbeat
 */
TEST_F(ServerGamesFixture, FailsToStartWhenTheSettingsAskForTooManyGames) {
  rtype::server::GamesSettings settings = shippedWaves();
  settings.gameCount = MOST_GAMES_A_HEARTBEAT_LISTS + 1;

  EXPECT_THROW(startWith(std::move(settings)), ModuleStartFailedException);
}
