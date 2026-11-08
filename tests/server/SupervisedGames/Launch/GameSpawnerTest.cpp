#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include "GameRules.hpp"
#include "GameSpawnerFixture.hpp"
#include "NetworkConstants.hpp"
#include "SupervisedGamesFixtures.hpp"
#include "SupervisedGamesTestConstants.hpp"

/**
 * Given a spawner whose free port is 50000
 * When it opens a game
 * Then a process was launched with that port, the registry holds a game on it,
 * and the network admits that port on the loopback under the game's token
 */
TEST_F(GameSpawnerFixture, StartsTheProcessRegistersTheGameAndAdmitsIt) {
  const bool opened =
      spawner().openGame(std::nullopt, rtype::network::defaultGameRules());

  EXPECT_TRUE(opened);
  ASSERT_EQ(launcher().commands().size(), 1U);
  EXPECT_EQ(argumentAfter(launcher().commands().front(), "-p"),
            std::to_string(FIRST_GAME_PORT));
  ASSERT_EQ(registry().all().size(), 1U);
  EXPECT_EQ(registry().all().front().port, FIRST_GAME_PORT);
  ASSERT_EQ(admission().admitted().size(), 1U);
  EXPECT_EQ(admission().admitted().front().player, loopbackOn(FIRST_GAME_PORT));
}

/**
 * Given a spawner that opened a game
 * When the admission is read
 * Then it carries the link token the command gave the game
 */
TEST_F(GameSpawnerFixture, AdmitsTheGameUnderTheTokenOfItsCommand) {
  EXPECT_TRUE(
      spawner().openGame(std::nullopt, rtype::network::defaultGameRules()));

  EXPECT_EQ(std::to_string(admission().admitted().front().sessionToken),
            argumentAfter(launcher().commands().front(), "--link-token"));
}

/**
 * Given a spawner whose supervisor listens on 4242
 * When it opens a game
 * Then the command tells the game that supervisor port
 */
TEST_F(GameSpawnerFixture, TellsTheGameThePortOfTheSupervisor) {
  EXPECT_TRUE(
      spawner().openGame(std::nullopt, rtype::network::defaultGameRules()));

  EXPECT_EQ(argumentAfter(launcher().commands().front(), "--supervisor"),
            std::to_string(rtype::network::DEFAULT_SERVER_PORT));
}

/**
 * Given a spawner that opened a game
 * When it opens a second one
 * Then the second game has the journal of game 1 and a token of its own
 */
TEST_F(GameSpawnerFixture, NumbersTheGamesInTheOrderItOpensThem) {
  EXPECT_TRUE(
      spawner().openGame(std::nullopt, rtype::network::defaultGameRules()));

  EXPECT_TRUE(
      spawner().openGame(std::nullopt, rtype::network::defaultGameRules()));

  EXPECT_NE(argumentAfter(launcher().commands().back(), "--log-file")
                .find("r-type_game_1.log"),
            std::string::npos);
  EXPECT_NE(registry().all().front().linkToken,
            registry().all().back().linkToken);
}

/**
 * Given a launcher that refuses to start the process
 * When the spawner opens a game
 * Then nothing is registered or admitted, and the next game still gets the
 * number 0
 */
TEST_F(GameSpawnerFixture, GoesOnWhenAProcessCannotStart) {
  launcher().failNextLaunch();

  const bool opened =
      spawner().openGame(std::nullopt, rtype::network::defaultGameRules());

  EXPECT_FALSE(opened);
  EXPECT_TRUE(registry().all().empty());
  EXPECT_TRUE(admission().admitted().empty());
  EXPECT_TRUE(
      spawner().openGame(std::nullopt, rtype::network::defaultGameRules()));
  EXPECT_NE(argumentAfter(launcher().commands().front(), "--log-file")
                .find("r-type_game_0.log"),
            std::string::npos);
}

/**
 * Given a free port function that throws
 * When the spawner opens a game
 * Then it says so and nothing is launched
 */
TEST_F(GameSpawnerFixture, GoesOnWhenNoPortIsFree) {
  useFreePort([]() -> std::uint16_t { throw std::runtime_error("none"); });

  EXPECT_FALSE(
      spawner().openGame(std::nullopt, rtype::network::defaultGameRules()));
  EXPECT_TRUE(launcher().commands().empty());
}
