#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "CommandInterpreter.hpp"
#include "CommandInterpreterFixtures.hpp"
#include "CommandInterpreterTestConstants.hpp"
#include "EngineException.hpp"

using rtype::engine::CommandDefinition;
using rtype::engine::CommandInterpreter;
using rtype::engine::DuplicateCommandException;

/**
 * Given a registered command
 * When a line runs it with two words
 * Then the line is accepted and the handler gets the two words
 */
TEST(CommandInterpreter, KnownCommandRunsWithItsWords) {
  CommandInterpreter interpreter;
  std::vector<std::string> received;
  interpreter.add(CommandDefinition{.name = std::string(ECHO_COMMAND)},
                  recordingHandler(received));
  const auto result = interpreter.execute(lineRunning(ECHO_COMMAND));
  EXPECT_TRUE(result.accepted);
  EXPECT_EQ(received, (std::vector<std::string>{std::string(FIRST_ARGUMENT),
                                                std::string(SECOND_ARGUMENT)}));
}

/**
 * Given an interpreter with no command of a given name
 * When a line starts with that name
 * Then the line is refused with a message naming the command and the line
 */
TEST(CommandInterpreter, UnknownCommandIsRefusedNamingCommandAndLine) {
  const CommandInterpreter interpreter;
  const std::string line = lineRunning(UNREGISTERED_COMMAND);
  const auto result = interpreter.execute(line);
  EXPECT_FALSE(result.accepted);
  EXPECT_NE(result.message.find(std::string(UNREGISTERED_COMMAND)),
            std::string::npos);
  EXPECT_NE(result.message.find(line), std::string::npos);
}

/**
 * Given a command whose handler refuses its words
 * When a line runs it
 * Then the line is refused with a message holding the command, the line and
 * the reason
 */
TEST(CommandInterpreter, RefusingHandlerGivesCommandLineAndReason) {
  CommandInterpreter interpreter;
  interpreter.add(CommandDefinition{.name = std::string(ECHO_COMMAND)},
                  refusingHandler(REFUSAL_REASON));
  const std::string line = lineRunning(ECHO_COMMAND);
  const auto result = interpreter.execute(line);
  EXPECT_FALSE(result.accepted);
  EXPECT_NE(result.message.find(std::string(ECHO_COMMAND)), std::string::npos);
  EXPECT_NE(result.message.find(line), std::string::npos);
  EXPECT_NE(result.message.find(std::string(REFUSAL_REASON)),
            std::string::npos);
}

/**
 * Given a registered command
 * When a blank line runs
 * Then the line is accepted and the handler is not called
 */
TEST(CommandInterpreter, BlankLineIsAcceptedAndRunsNothing) {
  CommandInterpreter interpreter;
  std::vector<std::string> received;
  interpreter.add(CommandDefinition{.name = std::string(ECHO_COMMAND)},
                  recordingHandler(received));
  EXPECT_TRUE(interpreter.execute(BLANK_LINE).accepted);
  EXPECT_TRUE(received.empty());
}

/**
 * Given a registered command
 * When a comment line naming it runs
 * Then the line is accepted and the handler is not called
 */
TEST(CommandInterpreter, CommentLineRunsNothing) {
  CommandInterpreter interpreter;
  std::vector<std::string> received;
  interpreter.add(CommandDefinition{.name = std::string(ECHO_COMMAND)},
                  recordingHandler(received));
  EXPECT_TRUE(interpreter.execute(COMMENT_LINE).accepted);
  EXPECT_TRUE(received.empty());
}

/**
 * Given a registered command
 * When a command is registered under the same name
 * Then the registration is refused with a DuplicateCommandException
 */
TEST(CommandInterpreter, SecondCommandWithTheSameNameIsRefused) {
  CommandInterpreter interpreter;
  std::vector<std::string> received;
  interpreter.add(CommandDefinition{.name = std::string(ECHO_COMMAND)},
                  recordingHandler(received));
  EXPECT_THROW(
      interpreter.add(CommandDefinition{.name = std::string(ECHO_COMMAND)},
                      recordingHandler(received)),
      DuplicateCommandException);
}

/**
 * Given two registered commands
 * When the help is read
 * Then it holds one line per command, sorted by name, with the arguments and
 * the help of each
 */
TEST(CommandInterpreter, HelpListsEachCommandSortedByName) {
  CommandInterpreter interpreter;
  std::vector<std::string> received;
  interpreter.add(CommandDefinition{.name = std::string(ECHO_COMMAND),
                                    .arguments = std::string(ECHO_ARGUMENTS),
                                    .help = std::string(ECHO_HELP)},
                  recordingHandler(received));
  interpreter.add(CommandDefinition{.name = std::string(ALPHA_COMMAND),
                                    .help = std::string(ALPHA_HELP)},
                  recordingHandler(received));
  EXPECT_EQ(interpreter.help(),
            (std::vector<std::string>{
                std::string(ALPHA_COMMAND) + " - " + std::string(ALPHA_HELP),
                std::string(ECHO_COMMAND) + " " + std::string(ECHO_ARGUMENTS) +
                    " - " + std::string(ECHO_HELP)}));
}
