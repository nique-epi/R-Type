#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "AdminConsoleTestConstants.hpp"
#include "ServerAdminConsoleModuleFixture.hpp"
#include "ServerAdminConsoleModuleTestConstants.hpp"
#include "ServerModuleTestConstants.hpp"

/**
 * Given a supervisor whose console input hands "games"
 * When it starts and runs its ticks
 * Then the console writes that no game runs
 */
TEST_F(ServerAdminConsoleModuleFixture, RunsATypedLine) {
  const auto application = supervisorTyping({std::string{GAMES_LINE} + "\n"});
  application->start();

  const std::vector<std::string> answer = answerOf(*application);

  application->stop();
  EXPECT_EQ(answer, (std::vector<std::string>{std::string{NO_GAME_LINE}}));
}

/**
 * Given a supervisor whose console input hands a mistyped command
 * When it starts and runs its ticks
 * Then the console writes the refusal of the interpreter, naming the line
 */
TEST_F(ServerAdminConsoleModuleFixture, WritesWhyALineWasRefused) {
  const auto application =
      supervisorTyping({std::string{MISTYPED_GAMES_LINE} + "\n"});
  application->start();

  const std::vector<std::string> answer = answerOf(*application);

  application->stop();
  EXPECT_EQ(answer,
            (std::vector<std::string>{std::string{MISTYPED_GAMES_ANSWER}}));
}

/**
 * Given a server.cfg that runs "games"
 * When the supervisor starts
 * Then the line is not refused, and it writes that no game runs: the command
 * of the console runs from the file as it does from the console
 */
TEST_F(ServerAdminConsoleModuleFixture, RunsItsCommandsFromTheConfiguration) {
  writeConfiguration({std::string{GAMES_LINE}});
  const auto application = supervisorTyping({});
  application->start();

  const std::vector<std::string> answer = answerOf(*application);

  application->stop();
  EXPECT_TRUE(report().refusals.empty());
  EXPECT_EQ(answer, (std::vector<std::string>{std::string{NO_GAME_LINE}}));
}

/**
 * Given a server.cfg that turns the console off and then runs "games"
 * When the supervisor starts
 * Then "games" is refused as an unknown command
 */
TEST_F(ServerAdminConsoleModuleFixture, IsTurnedOffByTheConfiguration) {
  writeConfiguration({std::string{CONSOLE_OFF_LINE}, std::string{GAMES_LINE}});
  const auto application = supervisor({});

  application->start();

  application->stop();
  EXPECT_EQ(report().refusals.size(), 1U);
}

/**
 * Given a supervisor whose console waits for a line nobody types
 * When the supervisor stops
 * Then the stop returns, and the read of the console was woken by it
 */
TEST_F(ServerAdminConsoleModuleFixture, StopsItsThreadWithTheSupervisor) {
  const auto application = supervisorTyping({});
  application->start();

  application->stop();

  EXPECT_TRUE(consoleWokeOnStop());
}
