#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "AdminConsoleTestConstants.hpp"
#include "RecordingTerminal.hpp"
#include "ServerModuleTestConstants.hpp"
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

  EXPECT_EQ(terminal.written(), READY_LINE_AND_PROMPT);
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
  static_cast<void>(screen.feed(GAM_TYPED));

  screen.writeAbove(GAME_PROCESS_LINE);

  EXPECT_TRUE(terminal.written().ends_with(GAME_LINE_ABOVE_GAM));
}

/**
 * Given an open screen
 * When two lines, one end of line between them, are written above
 * Then each starts a row from its first column, the prompt under them
 */
TEST(TerminalScreen, StartsEachLineAboveOnItsOwnRow) {
  RecordingTerminal terminal{true};
  TerminalScreen screen{terminal};
  screen.open();

  screen.writeAbove(TWO_GAME_LINES);

  EXPECT_TRUE(terminal.written().ends_with(TWO_GAME_ROWS));
}

/**
 * Given an open screen
 * When "games" and Enter are typed
 * Then the screen hands the line "games"
 */
TEST(TerminalScreen, HandsTheLinesTyped) {
  RecordingTerminal terminal{true};
  TerminalScreen screen{terminal};

  EXPECT_EQ(screen.feed(GAMES_ENTERED).lines,
            std::vector<std::string>{std::string{GAMES_LINE}});
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
