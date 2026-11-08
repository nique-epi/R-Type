#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "AdminConsoleTestConstants.hpp"
#include "ConsoleLineSplitterTestConstants.hpp"
#include "LineEdit.hpp"
#include "LineEditor.hpp"
#include "LineEditorTestConstants.hpp"
#include "ServerModuleTestConstants.hpp"

using rtype::server::LineEdit;
using rtype::server::LineEditor;

/**
 * Given an empty line
 * When "gam" is typed
 * Then the keys are echoed and no line ends
 */
TEST(LineEditor, EchoesWhatIsTyped) {
  LineEditor editor;

  const LineEdit edit = editor.feed(GAM_TYPED);

  EXPECT_EQ(edit.echo, GAM_TYPED);
  EXPECT_TRUE(edit.chunk.lines.empty());
}

/**
 * Given an empty line
 * When "games" then a carriage return are typed
 * Then the line "games" ends, and a new row shows the prompt
 */
TEST(LineEditor, EndsALineOnCarriageReturn) {
  LineEditor editor;

  const LineEdit edit = editor.feed(GAMES_ENTERED);

  EXPECT_EQ(edit.chunk.lines,
            std::vector<std::string>{std::string{GAMES_LINE}});
  EXPECT_TRUE(edit.echo.ends_with(NEW_ROW_AND_PROMPT));
}

/**
 * Given "games" ended by a carriage return in one read
 * When the new line of the same Enter comes in the next read
 * Then no second line ends
 */
TEST(LineEditor, CountsCarriageReturnAndNewLineOnce) {
  LineEditor editor;
  static_cast<void>(editor.feed(GAMES_ENTERED));

  const LineEdit edit = editor.feed(NEW_LINE_ALONE);

  EXPECT_TRUE(edit.chunk.lines.empty());
}

/**
 * Given an empty line
 * When "games" then a new line are typed
 * Then the line "games" ends
 */
TEST(LineEditor, EndsALineOnNewLine) {
  LineEditor editor;

  EXPECT_EQ(editor.feed(GAMES_AND_NEW_LINE).chunk.lines,
            std::vector<std::string>{std::string{GAMES_LINE}});
}

/**
 * Given "gamx" typed
 * When Delete is pressed
 * Then the line holds "gam", and the row of the prompt is drawn again with it
 */
TEST(LineEditor, ErasesTheLastCharacter) {
  LineEditor editor;
  static_cast<void>(editor.feed(GAMES_AND_A_WRONG_KEY));

  const LineEdit edit = editor.feed(DELETE_KEY);

  EXPECT_EQ(editor.typed(), GAM_TYPED);
  EXPECT_EQ(edit.echo, REDRAWN_GAM);
}

/**
 * Given "café" typed, its é two bytes of UTF-8
 * When Backspace is pressed
 * Then both bytes of the é go
 */
TEST(LineEditor, ErasesAWholeUtf8Character) {
  LineEditor editor;
  static_cast<void>(editor.feed(CAFE_WITH_AN_ACUTE_E));

  static_cast<void>(editor.feed(BACKSPACE_KEY));

  EXPECT_EQ(editor.typed(), CAFE_WITHOUT_ITS_E);
}

/**
 * Given an "e" and a combining acute accent typed, one column on screen
 * When Backspace is pressed
 * Then the accent goes, and the row is drawn again with the "e" kept
 */
TEST(LineEditor, DrawsTheLineAgainAfterAnErase) {
  LineEditor editor;
  static_cast<void>(editor.feed(E_WITH_A_COMBINING_ACUTE));

  const LineEdit edit = editor.feed(BACKSPACE_KEY);

  EXPECT_EQ(edit.echo, REDRAWN_E);
}

/**
 * Given an empty line
 * When Delete is pressed
 * Then nothing is echoed
 */
TEST(LineEditor, ErasesNothingFromAnEmptyLine) {
  LineEditor editor;

  EXPECT_TRUE(editor.feed(DELETE_KEY).echo.empty());
}

/**
 * Given "ga" typed
 * When the up arrow, then "mes", are typed
 * Then the line holds "games": the sequence of the arrow left no byte
 */
TEST(LineEditor, DropsTheSequenceOfAnArrow) {
  LineEditor editor;

  static_cast<void>(editor.feed(GAMES_WITH_AN_UP_ARROW));

  EXPECT_EQ(editor.typed(), GAMES_LINE);
}

/**
 * Given an empty line
 * When Escape alone, then "g", are typed
 * Then the line holds "g": a lone Escape does not take the next key
 */
TEST(LineEditor, KeepsTheKeyAfterALoneEscape) {
  LineEditor editor;

  static_cast<void>(editor.feed(LONE_ESCAPE_THEN_G));

  EXPECT_EQ(editor.typed(), G_KEY);
}

/**
 * Given an empty line
 * When as many bytes as the longest line are typed
 * Then the line keeps them all
 */
TEST(LineEditor, KeepsALineAtTheLimit) {
  LineEditor editor;

  static_cast<void>(
      editor.feed(std::string(LONGEST_LINE_LENGTH, TYPED_FILLING)));

  EXPECT_EQ(editor.typed().size(), LONGEST_LINE_LENGTH);
}

/**
 * Given a line as long as the longest line
 * When one more byte is typed
 * Then the line does not keep it, and the terminal shows nothing
 */
TEST(LineEditor, RefusesTheByteAfterTheLimit) {
  LineEditor editor;
  static_cast<void>(
      editor.feed(std::string(LONGEST_LINE_LENGTH, TYPED_FILLING)));

  const LineEdit edit = editor.feed(G_KEY);

  EXPECT_EQ(editor.typed().size(), LONGEST_LINE_LENGTH);
  EXPECT_TRUE(edit.echo.empty());
}

/**
 * Given a line one byte short of the longest line
 * When a character of two bytes is typed
 * Then the line refuses it whole, and the terminal shows nothing
 */
TEST(LineEditor, RefusesACharacterThatWouldPassTheLimit) {
  LineEditor editor;
  static_cast<void>(
      editor.feed(std::string(LONGEST_LINE_LENGTH - 1, TYPED_FILLING)));

  const LineEdit edit = editor.feed(TWO_BYTE_CHARACTER);

  EXPECT_EQ(editor.typed().size(), LONGEST_LINE_LENGTH - 1);
  EXPECT_TRUE(edit.echo.empty());
}

/**
 * Given "ban" typed, never ended
 * When the input ends
 * Then "ban" comes as a last line
 */
TEST(LineEditor, FinishesWithTheTypedText) {
  LineEditor editor;
  static_cast<void>(editor.feed(BAN_TYPED));

  EXPECT_EQ(editor.finish().lines,
            std::vector<std::string>{std::string{BAN_TYPED}});
}
