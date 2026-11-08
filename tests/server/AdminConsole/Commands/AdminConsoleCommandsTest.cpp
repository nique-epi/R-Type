#include <gtest/gtest.h>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>
#include "AdminConsoleCommandsFixture.hpp"
#include "AdminConsoleCommandsTestConstants.hpp"
#include "AdminConsoleTestConstants.hpp"
#include "CommandResult.hpp"
#include "EngineException.hpp"
#include "ExpulsionOrder.hpp"
#include "MessageType.hpp"
#include "ModuleConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "SentDatagrams.hpp"
#include "ServerModuleTestConstants.hpp"
#include "SupervisedGamesFixtures.hpp"
#include "SupervisedGamesTestConstants.hpp"

using rtype::engine::DuplicateCommandException;
using rtype::network::ExpulsionOrder;
using rtype::network::MessageType;
using rtype::network::OutgoingDatagram;
using rtype::server::testing::headerOf;
using rtype::server::testing::onlyMessageIn;
using rtype::server::testing::onlyMessageTypeOf;

/**
 * Given game 3 in place 1
 * When "close-game 1" runs
 * Then the game gets FAREWELL_COPIES Disconnections under its link token, on
 * its port, as the supervisor sends when it stops
 */
TEST_F(AdminConsoleCommandsFixture, SendsTheGameOfThePlaceItsStopOrders) {
  openGameThree();

  EXPECT_TRUE(run(CLOSE_FIRST_PLACE_LINE).accepted);

  ASSERT_EQ(sent().size(), rtype::server::FAREWELL_COPIES);
  for (const OutgoingDatagram& datagram : sent()) {
    EXPECT_EQ(datagram.destination, loopbackOn(portOfGame(GAME_THREE)));
    EXPECT_EQ(headerOf(datagram).sessionToken, tokenOfGame(GAME_THREE));
    EXPECT_EQ(onlyMessageTypeOf(datagram), MessageType::Disconnection);
  }
}

/**
 * Given game 3 in place 1
 * When "close-game 1" runs
 * Then the output says that game was told to stop
 */
TEST_F(AdminConsoleCommandsFixture, SaysWhichGameWasToldToStop) {
  openGameThree();

  static_cast<void>(run(CLOSE_FIRST_PLACE_LINE));

  EXPECT_EQ(
      lines(),
      (std::vector<std::string>{std::string{GAME_THREE_TOLD_TO_STOP_LINE}}));
}

/**
 * Given game 3 in place 1, which reports PILOT1
 * When "kick pilot1" runs
 * Then the game gets FAREWELL_COPIES datagrams under its link token, on its
 * port, each holding one ExpulsionOrder of the nickname as typed
 */
TEST_F(AdminConsoleCommandsFixture, SendsTheGameOfThePilotItsExpulsionOrders) {
  openGameThreeWithItsPilots();

  EXPECT_TRUE(run(KICK_FIRST_PILOT_LINE).accepted);

  ASSERT_EQ(sent().size(), rtype::server::FAREWELL_COPIES);
  for (const OutgoingDatagram& datagram : sent()) {
    EXPECT_EQ(datagram.destination, loopbackOn(portOfGame(GAME_THREE)));
    EXPECT_EQ(headerOf(datagram).sessionToken, tokenOfGame(GAME_THREE));
    EXPECT_EQ(onlyMessageIn<ExpulsionOrder>(datagram).nickname,
              KICKED_NICKNAME);
  }
}

/**
 * Given game 3 in place 1, which reports PILOT1
 * When "kick pilot1" runs
 * Then the output says the pilot was told to leave game 3 in place 1
 */
TEST_F(AdminConsoleCommandsFixture, SaysWhichGameThePilotWasToldToLeave) {
  openGameThreeWithItsPilots();

  EXPECT_TRUE(run(KICK_FIRST_PILOT_LINE).accepted);

  EXPECT_EQ(lines(), std::vector<std::string>{
                         std::string{PILOT_ONE_TOLD_TO_LEAVE_LINE}});
}

/**
 * Given game 3 in place 1, which reports PILOT1
 * When "kick NOBODY" runs
 * Then the line is refused and nothing is sent
 */
TEST_F(AdminConsoleCommandsFixture, RefusesAPilotNoGameHolds) {
  openGameThreeWithItsPilots();

  EXPECT_FALSE(run(KICK_NOBODY_LINE).accepted);

  EXPECT_TRUE(sent().empty());
}

/**
 * Given game 3 in place 1, which reports PILOT1
 * When "kick" runs without a nickname
 * Then the line is refused and nothing is sent
 */
TEST_F(AdminConsoleCommandsFixture, RefusesKickWithoutANickname) {
  openGameThreeWithItsPilots();

  EXPECT_FALSE(run(KICK_WITHOUT_NICKNAME_LINE).accepted);

  EXPECT_TRUE(sent().empty());
}

/**
 * Given game 3 in place 1, which reports PILOT1
 * When "kick PILOT1 PILOT2" runs, naming two nicknames
 * Then the line is refused and nothing is sent
 */
TEST_F(AdminConsoleCommandsFixture, RefusesTwoNicknames) {
  openGameThreeWithItsPilots();

  EXPECT_FALSE(run(KICK_TWO_NICKNAMES_LINE).accepted);

  EXPECT_TRUE(sent().empty());
}

/**
 * Given game 3 in place 1, which reports PILOT1
 * When kick names 17 bytes, one past the longest nickname
 * Then the line is refused because it names no nickname, not because no game
 * holds that pilot, and nothing is sent
 */
TEST_F(AdminConsoleCommandsFixture, RefusesWhatIsNotANickname) {
  openGameThreeWithItsPilots();

  const rtype::engine::CommandResult result = run(KICK_TOO_LONG_NICKNAME_LINE);

  EXPECT_FALSE(result.accepted);
  EXPECT_NE(result.message.find(NOT_A_NICKNAME_REASON), std::string::npos)
      << result.message;
  EXPECT_TRUE(sent().empty());
}

/**
 * Given game 3 in place 1
 * When "close-game 2" runs
 * Then the line is refused and nothing is sent
 */
TEST_F(AdminConsoleCommandsFixture, RefusesAPlaceNoGameHolds) {
  openGameThree();

  EXPECT_FALSE(run(CLOSE_EMPTY_PLACE_LINE).accepted);

  EXPECT_TRUE(sent().empty());
}

/**
 * Given game 3 in place 1
 * When "close-game" runs without a place
 * Then the line is refused and nothing is sent
 */
TEST_F(AdminConsoleCommandsFixture, RefusesCloseGameWithoutAPlace) {
  openGameThree();

  EXPECT_FALSE(run(CLOSE_WITHOUT_PLACE_LINE).accepted);

  EXPECT_TRUE(sent().empty());
}

/**
 * Given game 3 in place 1
 * When "close-game 1 2" runs, naming two places
 * Then the line is refused and nothing is sent
 */
TEST_F(AdminConsoleCommandsFixture, RefusesTwoPlaces) {
  openGameThree();

  EXPECT_FALSE(run(CLOSE_TWO_PLACES_LINE).accepted);

  EXPECT_TRUE(sent().empty());
}

/**
 * Given game 3 in place 1
 * When "close-game one" runs
 * Then the line is refused and nothing is sent
 */
TEST_F(AdminConsoleCommandsFixture, RefusesAPlaceThatIsNotANumber) {
  openGameThree();

  EXPECT_FALSE(run(CLOSE_WORD_PLACE_LINE).accepted);

  EXPECT_TRUE(sent().empty());
}

/**
 * Given game 3 in place 1
 * When "close-game -65535" runs, a place that cut to 16 bits is 1
 * Then the line is refused and nothing is sent
 */
TEST_F(AdminConsoleCommandsFixture, RefusesANegativePlace) {
  openGameThree();

  EXPECT_FALSE(run(CLOSE_NEGATIVE_PLACE_LINE).accepted);

  EXPECT_TRUE(sent().empty());
}

/**
 * Given game 3 in place 1
 * When "close-game 65537" runs, a place that cut to 16 bits is 1
 * Then the line is refused and nothing is sent
 */
TEST_F(AdminConsoleCommandsFixture, RefusesAPlacePastSixteenBits) {
  openGameThree();

  EXPECT_FALSE(run(CLOSE_PLACE_PAST_SIXTEEN_BITS_LINE).accepted);

  EXPECT_TRUE(sent().empty());
}

/**
 * Given no game
 * When "games" runs
 * Then the output says no game runs
 */
TEST_F(AdminConsoleCommandsFixture, SaysWhenNoGameRuns) {
  EXPECT_TRUE(run(GAMES_LINE).accepted);

  EXPECT_EQ(lines(), (std::vector<std::string>{std::string{NO_GAME_LINE}}));
}

/**
 * Given no game
 * When "games now" runs
 * Then the line is refused and nothing is written
 */
TEST_F(AdminConsoleCommandsFixture, RefusesWordsAfterACommandWithoutArgument) {
  EXPECT_FALSE(run(GAMES_WITH_A_WORD_LINE).accepted);

  EXPECT_TRUE(lines().empty());
}

/**
 * Given two datagrams rejected, and no measure of the games
 * When "network" runs
 * Then the output gives the traffic of the socket, the two rejected
 * datagrams, then says the games were not measured yet
 */
TEST_F(AdminConsoleCommandsFixture, ShowsTheNetworkFigures) {
  rejectTwoDatagrams();

  EXPECT_TRUE(run(NETWORK_LINE).accepted);

  const std::vector<std::string> written = lines();
  ASSERT_EQ(written.size(), 3U);
  EXPECT_TRUE(written[0].starts_with(TRAFFIC_LINE_START));
  EXPECT_EQ(written[1], TWO_REJECTED_LINE);
  EXPECT_EQ(written[2], NO_MEASURE_LINE);
}

/**
 * Given the commands registered
 * When "help" runs
 * Then it writes one line per command, each starting with its name
 */
TEST_F(AdminConsoleCommandsFixture, ListsTheCommandsInTheHelp) {
  EXPECT_TRUE(run(HELP_LINE).accepted);

  const std::vector<std::string> written = lines();
  ASSERT_EQ(written.size(), CONSOLE_COMMAND_NAMES.size());
  for (std::size_t index = 0; index < written.size(); ++index) {
    EXPECT_TRUE(written[index].starts_with(CONSOLE_COMMAND_NAMES.at(index)))
        << written[index];
  }
}

/**
 * Given the commands registered once
 * When they are registered again in the same interpreter
 * Then the interpreter refuses the second registration
 */
TEST_F(AdminConsoleCommandsFixture, RefusesASecondRegistration) {
  EXPECT_THROW(registerAgain(), DuplicateCommandException);
}
