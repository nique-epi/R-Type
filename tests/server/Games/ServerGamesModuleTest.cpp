#include <gtest/gtest.h>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>
#include "EngineException.hpp"
#include "GamePlayers.hpp"
#include "GameState.hpp"
#include "GamesSettings.hpp"
#include "MessageType.hpp"
#include "RecordingPlayerAdmission.hpp"
#include "SentDatagrams.hpp"
#include "ServerGamesFixture.hpp"
#include "ServerModuleTestConstants.hpp"

using rtype::engine::ModuleStartFailedException;
using rtype::network::MessageType;
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
 * When one of them leaves, and the loop goes on for a few ticks
 * Then the other one is told it left once by the tick that closed the session,
 * and the later copies follow as the timers advance and the bus delivers
 */
TEST_F(ServerGamesFixture, TellsTheOthersThatAPlayerLeftAsTimersAndBusRun) {
  startWith(shippedWaves());
  join(0);
  join(1);

  runTick({disconnectionFrom(0)});
  const std::size_t afterTheLeavingTick =
      datagramsTo(1, MessageType::PlayerLeft);
  for (int tick = 0; tick < TICKS_FOR_ONE_MORE_COPY; ++tick) {
    runTick({});
  }
  const std::size_t afterOneMoreInterval =
      datagramsTo(1, MessageType::PlayerLeft);
  for (int tick = 0; tick < TICKS_FOR_THE_LAST_COPY; ++tick) {
    runTick({});
  }

  EXPECT_EQ(afterTheLeavingTick, 1U);
  EXPECT_EQ(afterOneMoreInterval, 2U);
  EXPECT_EQ(datagramsTo(1, MessageType::PlayerLeft), 3U);
}

/**
 * Given two players in the second game
 * When one of them leaves, and the loop goes on for a few ticks
 * Then the other one gets every copy of PlayerLeft: the second game reads the
 * datagrams of its players, and its own bus delivers its events
 */
TEST_F(ServerGamesFixture, RunsTheSecondGameWithItsOwnPlayersAndBus) {
  startWith(shippedWaves());
  seatPlayerIn(*probe().games()->all()[SECOND_GAME], 0);
  seatPlayerIn(*probe().games()->all()[SECOND_GAME], 1);

  runTick({disconnectionFrom(0)});
  for (int tick = 0; tick < TICKS_FOR_THE_LAST_COPY; ++tick) {
    runTick({});
  }

  EXPECT_EQ(datagramsTo(1, MessageType::PlayerLeft), 3U);
}

/**
 * Given one player in the first game and one in the second
 * When the player of the first game leaves, and the loop goes on for a few
 * ticks
 * Then the player of the second game is told nothing: the games do not share
 * a bus
 */
TEST_F(ServerGamesFixture, KeepsTheEventsOfAGameAwayFromTheOtherGames) {
  startWith(shippedWaves());
  seatPlayerIn(*probe().games()->all()[FIRST_GAME], 0);
  seatPlayerIn(*probe().games()->all()[SECOND_GAME], 1);

  runTick({disconnectionFrom(0)});
  for (int tick = 0; tick < TICKS_FOR_THE_LAST_COPY; ++tick) {
    runTick({});
  }

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
