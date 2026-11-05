#include <gtest/gtest.h>
#include <string>
#include <utility>
#include "CommandInterpreter.hpp"
#include "CommandResult.hpp"
#include "GameState.hpp"
#include "GamesSettings.hpp"
#include "InformationConstants.hpp"
#include "ProtocolConstants.hpp"
#include "ServerInformation.hpp"
#include "ServerInformationFixture.hpp"
#include "ServerInformationModuleTestConstants.hpp"
#include "ServerModuleTestConstants.hpp"

using rtype::network::ServerInformation;
using rtype::server::GameState;
using rtype::server::testing::LYON;
using rtype::server::testing::NAME_OF_A_SERVER_LEFT_AS_SHIPPED;
using rtype::server::testing::NAME_THE_SERVER_LYON_LINE;
using rtype::server::testing::NAME_TOO_LONG_LINE;
using rtype::server::testing::SEATS_OF_THE_SECOND_GAME;
using rtype::server::testing::SEATS_OF_TWO_GAMES_HOSTED;
using rtype::server::testing::TWO_GAMES_HOSTED;

/**
 * Given a server whose configuration leaves server-name as it is
 * When a stranger asks for its information
 * Then it is told the name of server.cfg.example, the game version of the
 * build and the protocol this code speaks
 */
TEST_F(ServerInformationFixture, AnswersTheNameTheVersionsAsShipped) {
  startWith(shippedWaves());

  const ServerInformation answer = askFrom(0);

  EXPECT_EQ(answer.name, NAME_OF_A_SERVER_LEFT_AS_SHIPPED);
  EXPECT_EQ(answer.gameVersion, rtype::server::SERVER_GAME_VERSION);
  EXPECT_EQ(answer.protocolVersion, rtype::network::PROTOCOL_VERSION);
}

/**
 * Given a configuration that names the server Lyon
 * When a stranger asks for its information
 * Then it is told Lyon
 */
TEST_F(ServerInformationFixture, AnswersTheNameTheConfigurationGives) {
  startWith(shippedWaves(), [](rtype::engine::CommandInterpreter& commands) {
    static_cast<void>(commands.execute(NAME_THE_SERVER_LYON_LINE));
  });

  EXPECT_EQ(askFrom(0).name, LYON);
}

/**
 * Given the information module installed
 * When the configuration gives the server a name of 33 bytes
 * Then the line is refused, so the module declared the setting with its rule
 */
TEST_F(ServerInformationFixture, RefusesANameTooLong) {
  rtype::engine::CommandResult result;

  startWith(shippedWaves(),
            [&result](rtype::engine::CommandInterpreter& commands) {
              result = commands.execute(NAME_TOO_LONG_LINE);
            });

  EXPECT_FALSE(result.accepted);
}

/**
 * Given a server of two games and nobody seated
 * When a stranger asks for its information
 * Then it is told two games, no player and their eight seats free
 */
TEST_F(ServerInformationFixture, CountsTheSeatsOfAnEmptyServerAsFree) {
  rtype::server::GamesSettings settings = shippedWaves();
  settings.gameCount = TWO_GAMES_HOSTED;
  startWith(std::move(settings));

  const ServerInformation answer = askFrom(1);

  EXPECT_EQ(answer.gameCount, TWO_GAMES_HOSTED);
  EXPECT_EQ(answer.playerCount, 0U);
  EXPECT_EQ(answer.freeSeatCount, SEATS_OF_TWO_GAMES_HOSTED);
}

/**
 * Given a server of two games whose first left the waiting room because its
 * only player is ready
 * When a stranger asks for its information
 * Then it is told one player, and only the four seats of the second game
 * free: a game that started gives no seat
 */
TEST_F(ServerInformationFixture, CountsNoFreeSeatInAGameThatStarted) {
  rtype::server::GamesSettings settings = shippedWaves();
  settings.gameCount = TWO_GAMES_HOSTED;
  startWith(std::move(settings));
  join(0);
  sayReady(0);
  ASSERT_EQ(probe().games()->all()[FIRST_GAME]->state(), GameState::Starting);

  const ServerInformation answer = askFrom(1);

  EXPECT_EQ(answer.playerCount, 1U);
  EXPECT_EQ(answer.freeSeatCount, SEATS_OF_THE_SECOND_GAME);
}
