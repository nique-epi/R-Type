#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "AdminConsoleTestConstants.hpp"
#include "ConsoleAnswers.hpp"
#include "RecordingConsoleOutput.hpp"
#include "RecordingTerminal.hpp"
#include "TerminalScreen.hpp"

using rtype::server::ConsoleAnswers;
using rtype::server::TerminalScreen;

/**
 * Given answers shown on no screen
 * When a command answers
 * Then the answer goes to the output
 */
TEST(ConsoleAnswers, AnswersOnTheOutputWithoutAScreen) {
  RecordingConsoleOutput output;
  ConsoleAnswers answers{output};

  answers.write(NO_GAME_LINE);

  EXPECT_EQ(output.lines(),
            std::vector<std::string>{std::string{NO_GAME_LINE}});
}

/**
 * Given answers shown on the screen of a terminal
 * When a command answers
 * Then the answer is written above the prompt, and the output gets nothing
 */
TEST(ConsoleAnswers, AnswersAboveThePromptOnAScreen) {
  RecordingConsoleOutput output;
  RecordingTerminal terminal{true};
  TerminalScreen screen{terminal};
  ConsoleAnswers answers{output};
  answers.showOn(&screen);

  answers.write(NO_GAME_LINE);

  EXPECT_EQ(terminal.written(), NO_GAME_ABOVE_THE_PROMPT);
  EXPECT_TRUE(output.lines().empty());
}

/**
 * Given answers shown on a screen, then on none again
 * When a command answers
 * Then the answer goes to the output, and the terminal gets nothing
 */
TEST(ConsoleAnswers, AnswersOnTheOutputAgainOnceTheScreenIsGone) {
  RecordingConsoleOutput output;
  RecordingTerminal terminal{true};
  TerminalScreen screen{terminal};
  ConsoleAnswers answers{output};
  answers.showOn(&screen);
  answers.showOn(nullptr);

  answers.write(NO_GAME_LINE);

  EXPECT_EQ(output.lines(),
            std::vector<std::string>{std::string{NO_GAME_LINE}});
  EXPECT_TRUE(terminal.written().empty());
}
