#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "RecordingTerminal.hpp"
#include "TerminalScreen.hpp"
#include "TerminalScreenTestConstants.hpp"

using rtype::server::TerminalScreen;

/**
 * Given a screen on a terminal
 * When it opens
 * Then the terminal shows the ready line, then the prompt
 */
TEST(TerminalScreen, OpensWithTheReadyLineAndThePrompt) {
  RecordingTerminal terminal{true};
  TerminalScreen screen{terminal};

  screen.open();

  EXPECT_EQ(terminal.written(), OPENING);
}

/**
 * Given an open screen where "gam" is typed
 * When a line of the journal is written above
 * Then the row is erased, the line written, and the prompt and "gam" written
 * again under it
 */
TEST(TerminalScreen, WritesALineAboveTheTypedText) {
  RecordingTerminal terminal{true};
  TerminalScreen screen{terminal};
  screen.open();
  static_cast<void>(screen.feed(SCREEN_HALF_OF_GAMES));

  screen.writeAbove(JOURNAL_LINE);

  EXPECT_TRUE(terminal.written().ends_with(JOURNAL_LINE_ABOVE_GAM));
}

/**
 * Given an open screen
 * When "games" and Enter are typed
 * Then the screen hands the line "games"
 */
TEST(TerminalScreen, HandsTheLinesTyped) {
  RecordingTerminal terminal{true};
  TerminalScreen screen{terminal};

  EXPECT_EQ(screen.feed(SCREEN_GAMES_ENDED).lines,
            std::vector<std::string>{std::string{SCREEN_GAMES}});
}

/**
 * Given an open screen
 * When it closes
 * Then the row of the prompt is erased
 */
TEST(TerminalScreen, ErasesThePromptWhenClosed) {
  RecordingTerminal terminal{true};
  TerminalScreen screen{terminal};
  screen.open();

  screen.close();

  EXPECT_TRUE(terminal.written().ends_with(ERASED_ROW));
}
