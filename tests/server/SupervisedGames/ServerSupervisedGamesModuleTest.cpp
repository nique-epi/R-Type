#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include "GameStatus.hpp"
#include "MessageType.hpp"
#include "NetworkConstants.hpp"
#include "SentDatagrams.hpp"
#include "ServerSupervisedGamesFixture.hpp"
#include "SupervisedGamesFixtures.hpp"
#include "SupervisedGamesTestConstants.hpp"

using rtype::network::GameStatus;
using rtype::network::GameStatusState;
using rtype::network::MessageType;
using rtype::server::testing::onlyMessageTypeOf;
using rtype::server::testing::sentTo;

/**
 * Given a stranger that answered its challenge with the cookie
 * When the supervisor reads that request
 * Then it started one game process, naming its own port in its command
 */
TEST_F(ServerSupervisedGamesFixture, StartsAGameForAValidRequest) {
  startSupervisor();

  askForAPlaceAs(0);

  ASSERT_EQ(launcher().commands().size(), 1U);
  EXPECT_EQ(argumentAfter(launcher().commands().front(), "--supervisor"),
            std::to_string(rtype::network::DEFAULT_SERVER_PORT));
}

/**
 * Given a game process that was just started
 * When the network admission is read
 * Then the game's port on the loopback is admitted under the game's token
 */
TEST_F(ServerSupervisedGamesFixture, AdmitsTheGameItStarts) {
  startSupervisor();

  askForAPlaceAs(0);

  ASSERT_EQ(network().admission().admitted().size(), 1U);
  const auto admitted = network().admission().admitted().front();
  EXPECT_EQ(std::to_string(admitted.player.port),
            argumentAfter(launcher().commands().front(), "-p"));
  EXPECT_EQ(std::to_string(admitted.sessionToken),
            argumentAfter(launcher().commands().front(), "--link-token"));
}

/**
 * Given a game that sent a status
 * When the supervisor read it
 * Then it answered the game with an acknowledgement
 */
TEST_F(ServerSupervisedGamesFixture, AnswersAStatusWithAnAcknowledgement) {
  startSupervisor();
  askForAPlaceAs(0);

  gameReports(
      0,
      GameStatus{.state = GameStatusState::WaitingForPlayers, .playerCount = 0},
      SEQUENCE_FIVE);

  const auto answers = sentTo(
      network().sender().sent(),
      loopbackOn(static_cast<std::uint16_t>(
          std::stoi(argumentAfter(launcher().commands().front(), "-p")))));
  ASSERT_EQ(answers.size(), 1U);
  EXPECT_EQ(onlyMessageTypeOf(answers.front()),
            MessageType::GameStatusAcknowledged);
}

/**
 * Given two games of which the first crashes
 * When the supervisor ticks
 * Then the crashed game left, the second was left alone, and the network forgot
 * the first
 */
TEST_F(ServerSupervisedGamesFixture, KeepsServingWhenAGameCrashes) {
  startSupervisor();
  askForAPlaceAs(0);
  gameReports(
      0, GameStatus{.state = GameStatusState::InProgress, .playerCount = 4},
      SEQUENCE_FIVE);
  askForAPlaceAs(1);

  launcher().exitChild(0, CRASH_EXIT_CODE);
  runTick({});

  EXPECT_FALSE(launcher().childWasTerminated(1));
  EXPECT_EQ(network().admission().forgotten().size(), 1U);
}

/**
 * Given a launcher that refuses to start a process
 * When a player asks for a place with a valid cookie
 * Then no game starts, and the next player's request starts one
 */
TEST_F(ServerSupervisedGamesFixture, KeepsServingWhenAGameCannotStart) {
  startSupervisor();
  launcher().failNextLaunch();
  askForAPlaceAs(0);

  askForAPlaceAs(1);

  EXPECT_EQ(launcher().commands().size(), 1U);
}

/**
 * Given a game that never says a word
 * When ten seconds pass
 * Then its process is terminated
 */
TEST_F(ServerSupervisedGamesFixture, TerminatesAGameThatNeverReports) {
  startSupervisor();
  askForAPlaceAs(0);

  runFor(TEN_SECONDS_TO_START);

  EXPECT_TRUE(launcher().childWasTerminated(0));
}

/**
 * Given a game process that keeps running
 * When the supervisor stops
 * Then the game was told to stop with a Disconnection, and terminated after the
 * grace
 */
TEST_F(ServerSupervisedGamesFixture, StopsEveryGameWhenItStops) {
  startSupervisor();
  askForAPlaceAs(0);

  stopSupervisor();

  EXPECT_TRUE(launcher().childWasTerminated(0));
  bool toldToStop = false;
  for (const auto& datagram : network().sender().sent()) {
    toldToStop =
        toldToStop || onlyMessageTypeOf(datagram) == MessageType::Disconnection;
  }
  EXPECT_TRUE(toldToStop);
}

/**
 * Given two lines of the configuration that belong to the games
 * When the supervisor starts
 * Then the file of the games holds exactly those lines, in their order, before
 * any game is started
 */
TEST_F(ServerSupervisedGamesFixture, WritesTheLinesOfTheGamesWhenItStarts) {
  startSupervisor({std::string{GAME_SEED_LINE}, std::string{GAME_PLUGIN_LINE}});

  EXPECT_TRUE(launcher().commands().empty());
  EXPECT_EQ(gameConfigurationText(), GAME_LINES_TEXT);
}

/**
 * Given a supervisor with no line for its games
 * When it starts
 * Then the file of the games exists and is empty
 */
TEST_F(ServerSupervisedGamesFixture, WritesAnEmptyFileWhenNoLineIsForTheGames) {
  startSupervisor();

  EXPECT_TRUE(std::filesystem::exists(gameConfigurationFile()));
  EXPECT_EQ(gameConfigurationText(), "");
}

/**
 * Given a started supervisor
 * When a player asks for a game
 * Then the game is told to read the file of the games
 */
TEST_F(ServerSupervisedGamesFixture, TellsEachGameToReadTheFileOfTheGames) {
  startSupervisor();

  askForAPlaceAs(0);

  ASSERT_EQ(launcher().commands().size(), 1U);
  EXPECT_EQ(argumentAfter(launcher().commands().front(), "--configuration"),
            gameConfigurationFile().string());
}
