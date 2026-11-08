#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "AdminConsoleTestConstants.hpp"
#include "EngineException.hpp"
#include "Logger.hpp"
#include "ServerAdminConsoleModuleFixture.hpp"
#include "ServerAdminConsoleModuleTestConstants.hpp"
#include "ServerModuleTestConstants.hpp"
#include "ThrowingTerminal.hpp"

using rtype::engine::ModuleStartFailedException;
using rtype::logging::writeStandardErrorLine;

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

/**
 * Given a supervisor whose console runs at a terminal
 * When it starts
 * Then the terminal shows the ready line of the console, then the prompt
 */
TEST_F(ServerAdminConsoleModuleFixture, ShowsThePromptAtATerminal) {
  const auto application = supervisorAtATerminal({});
  application->start();

  const std::string shown =
      terminalShowing(*application, READY_LINE_AND_PROMPT);

  application->stop();
  EXPECT_NE(shown.find(READY_LINE_AND_PROMPT), std::string::npos);
}

/**
 * Given a supervisor whose console runs at a terminal, where "games" and Enter
 * are typed
 * When it runs its ticks
 * Then the answer is written above the prompt, on the terminal
 */
TEST_F(ServerAdminConsoleModuleFixture, AnswersAboveThePromptAtATerminal) {
  const auto application = supervisorAtATerminal({std::string{GAMES_ENTERED}});
  application->start();

  const std::string shown =
      terminalShowing(*application, NO_GAME_ABOVE_THE_PROMPT);

  application->stop();
  EXPECT_NE(shown.find(NO_GAME_ABOVE_THE_PROMPT), std::string::npos);
}

/**
 * Given a supervisor whose console runs at a terminal, where "gam" is typed
 * When a game process writes a line
 * Then the line is written above the prompt, and "gam" is shown again under it
 */
TEST_F(ServerAdminConsoleModuleFixture, WritesTheGamesAboveTheTypedLine) {
  const auto application = supervisorAtATerminal({std::string{GAM_TYPED}});
  application->start();
  static_cast<void>(terminalShowing(*application, PROMPT_AND_GAM));

  writeStandardErrorLine(GAME_PROCESS_LINE);

  const std::string shown = terminalShowing(*application, GAME_LINE_ABOVE_GAM);
  application->stop();
  EXPECT_TRUE(shown.ends_with(GAME_LINE_ABOVE_GAM));
}

/**
 * Given a supervisor whose console runs at a terminal
 * When the supervisor stops
 * Then the terminal leaves typing mode
 */
TEST_F(ServerAdminConsoleModuleFixture, GivesTheTerminalBackOnStop) {
  const auto application = supervisorAtATerminal({});
  application->start();
  static_cast<void>(terminalShowing(*application, READY_LINE_AND_PROMPT));

  application->stop();

  EXPECT_FALSE(terminalInTypingMode());
}

/**
 * Given a supervisor whose console ran at a terminal and stopped
 * When a game process writes a line
 * Then the terminal does not receive it
 */
TEST_F(ServerAdminConsoleModuleFixture, WritesNothingOnTheTerminalOnceStopped) {
  const auto application = supervisorAtATerminal({});
  application->start();
  static_cast<void>(terminalShowing(*application, READY_LINE_AND_PROMPT));
  application->stop();

  writeStandardErrorLine(GAME_PROCESS_LINE);

  EXPECT_EQ(terminalWritten().find(GAME_PROCESS_LINE), std::string::npos);
}

/**
 * Given a supervisor whose console runs on no terminal
 * When it starts and stops
 * Then nothing is written on the terminal
 */
TEST_F(ServerAdminConsoleModuleFixture, ShowsNoPromptWithoutATerminal) {
  const auto application = supervisorTyping({});
  application->start();

  application->stop();

  EXPECT_TRUE(terminalWritten().empty());
}

/**
 * Given a supervisor whose console runs at a terminal that cannot be written
 * When it starts
 * Then the start fails, and the terminal left typing mode
 */
TEST_F(ServerAdminConsoleModuleFixture,
       GivesTheTerminalBackWhenThePromptCannotBeShown) {
  auto terminal = std::make_unique<ThrowingTerminal>();
  const ThrowingTerminal& failing = *terminal;
  const auto application = supervisorOn({}, std::move(terminal));

  EXPECT_THROW(application->start(), ModuleStartFailedException);

  EXPECT_FALSE(failing.inTypingMode());
}
