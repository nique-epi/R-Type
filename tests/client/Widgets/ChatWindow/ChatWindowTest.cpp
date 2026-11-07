#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include <string_view>
#include "BoundAction.hpp"
#include "ChatEntry.hpp"
#include "ChatWindowFixture.hpp"
#include "ChatWindowTestConstants.hpp"
#include "ConnectionConstants.hpp"
#include "InterfaceConstants.hpp"
#include "KeyPress.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "WidgetColors.hpp"

using rtype::client::BoundAction;
using rtype::client::CHAT_NOTICE_COLOR;
using rtype::client::ChatEntry;
using rtype::client::CLIENT_WRITER;
using rtype::client::INTERFACE_AREA;
using rtype::client::UNANSWERED_CHAT_TEXT;
using rtype::client::testing::CHAT_KEY;
using rtype::client::testing::FEW_OPEN_LINES;
using rtype::client::testing::FOUR_LINES;
using rtype::client::testing::GONE;
using rtype::client::testing::HALF_ALPHA;
using rtype::client::testing::HALF_BACKGROUND;
using rtype::client::testing::HALF_BACKGROUND_ALPHA;
using rtype::client::testing::HALF_FADED;
using rtype::client::testing::LINE_PREFIX;
using rtype::client::testing::LONG_LINE;
using rtype::client::testing::LONG_LINE_FIRST;
using rtype::client::testing::LONG_LINE_SECOND;
using rtype::client::testing::LONG_LINE_THIRD;
using rtype::client::testing::MORE_AFTER_SCROLL;
using rtype::client::testing::MORE_BEFORE_SCROLL;
using rtype::client::testing::NARROW_LETTERS;
using rtype::client::testing::NEWEST_SHOWN_FIRST;
using rtype::client::testing::NOVA;
using rtype::client::testing::NOVA_HEAD;
using rtype::client::testing::NOVA_NUMBER;
using rtype::client::testing::NUMBERED_LINES;
using rtype::client::testing::OLDEST_OF_FOUR;
using rtype::client::testing::OLDEST_SHOWN_OF_FOUR;
using rtype::client::testing::SCROLLED_HIDDEN;
using rtype::client::testing::SCROLLED_SHOWN_FIRST;
using rtype::client::testing::SHORT_LINE;
using rtype::client::testing::SPACES;
using ::testing::ElementsAre;
using ::testing::IsEmpty;
using Key = sf::Keyboard::Key;
using Scan = sf::Keyboard::Scan;

/**
 * Given the chat of a first launch, closed
 * When T is pressed
 * Then the chat takes the press and opens
 */
TEST_F(ChatWindowFixture, TheChatKeyOpensTheChat) {
  EXPECT_TRUE(chat().handleEvent(keyPress(Key::T, CHAT_KEY)));

  EXPECT_TRUE(chat().isOpen());
}

/**
 * Given the chat opened by T, the keyboard typing its t
 * When gg is typed and ENTER pressed
 * Then the channel is sent gg, without the t, and the chat closes
 */
TEST_F(ChatWindowFixture, EnterSendsTheLineTypedWithoutTheLetterOfTheKey) {
  openChat();
  type(SHORT_LINE);

  tap(Key::Enter, Scan::Enter);

  EXPECT_THAT(channel().sent(), ElementsAre(std::string{SHORT_LINE}));
  EXPECT_FALSE(chat().isOpen());
}

/**
 * Given T held down, the keyboard repeating its press and its letter
 * When T is released, then gg typed and ENTER pressed
 * Then only gg is sent
 */
TEST_F(ChatWindowFixture, HoldingTheChatKeyTypesNothing) {
  chat().handleEvent(keyPress(Key::T, CHAT_KEY));
  chat().handleEvent(keyPress(Key::T, CHAT_KEY));
  chat().handleEvent(typedCharacter(U't'));
  chat().handleEvent(keyRelease(Key::T, CHAT_KEY));
  type(SHORT_LINE);

  tap(Key::Enter, Scan::Enter);

  EXPECT_THAT(channel().sent(), ElementsAre(std::string{SHORT_LINE}));
}

/**
 * Given the open chat holding gg
 * When ESC is pressed
 * Then nothing is sent and the chat closes
 */
TEST_F(ChatWindowFixture, EscapeClosesWithoutSending) {
  openChat();
  type(SHORT_LINE);

  tap(Key::Escape, Scan::Escape);

  EXPECT_THAT(channel().sent(), IsEmpty());
  EXPECT_FALSE(chat().isOpen());
}

/**
 * Given the chat closed by the press of ESC
 * When ESC is released
 * Then the chat takes the release, so the game opens no options on it
 */
TEST_F(ChatWindowFixture, TheReleaseOfTheEscapeThatClosedTheChatIsTaken) {
  openChat();
  chat().handleEvent(keyPress(Key::Escape, Scan::Escape));

  EXPECT_TRUE(chat().handleEvent(keyRelease(Key::Escape, Scan::Escape)));
}

/**
 * Given the open chat holding only spaces
 * When ENTER is pressed
 * Then nothing is sent
 */
TEST_F(ChatWindowFixture, ALineOfSpacesIsNotSent) {
  openChat();
  type(SPACES);

  tap(Key::Enter, Scan::Enter);

  EXPECT_THAT(channel().sent(), IsEmpty());
}

/**
 * Given the chat hidden in the options
 * When T is pressed
 * Then the chat leaves the press to the screen and stays closed
 */
TEST_F(ChatWindowFixture, AHiddenChatDoesNotOpen) {
  settings().chatShown = false;

  EXPECT_FALSE(chat().handleEvent(keyPress(Key::T, CHAT_KEY)));
  EXPECT_FALSE(chat().isOpen());
}

/**
 * Given SHOOT bound to T as well, SHOOT first in the list
 * When T is pressed
 * Then T shoots: the chat leaves it to the screen
 */
TEST_F(ChatWindowFixture, AKeySharedWithAnEarlierActionDoesNotOpen) {
  settings().keys.bind(BoundAction::Shoot, CHAT_KEY);

  EXPECT_FALSE(chat().handleEvent(keyPress(Key::T, CHAT_KEY)));
}

/**
 * Given the open chat
 * When the up arrow is pressed and released
 * Then the chat takes both: no key reaches the screen
 */
TEST_F(ChatWindowFixture, TheOpenChatTakesEveryKey) {
  openChat();

  EXPECT_TRUE(tap(Key::Up, Scan::Up));
}

/**
 * Given four short lines just received and the chat closed
 * When it is painted
 * Then the three newest lines are written outlined, the oldest not
 */
TEST_F(ChatWindowFixture, ClosedItShowsTheNewestLinesOutlined) {
  receiveNumberedLines(FOUR_LINES);

  const InterfaceCanvasRecord& record = painted();

  EXPECT_EQ(writingOf(record, OLDEST_OF_FOUR), nullptr);
  for (int line = OLDEST_SHOWN_OF_FOUR; line <= FOUR_LINES; ++line) {
    const std::string text = std::string{LINE_PREFIX} + std::to_string(line);
    const RecordedWriting* writing = writingOf(record, text);
    ASSERT_NE(writing, nullptr) << text;
    EXPECT_TRUE(writing->style.outlined) << text;
  }
}

/**
 * Given a line received eight seconds ago, the default time
 * When the closed chat is painted
 * Then the line is gone, and the chat paints nothing
 */
TEST_F(ChatWindowFixture, ALineOlderThanItsTimeIsGone) {
  channel().receive(ChatEntry{.playerNumber = NOVA_NUMBER,
                              .nickname = std::string{NOVA},
                              .text = std::string{SHORT_LINE},
                              .age = GONE});

  EXPECT_EQ(writingOf(painted(), SHORT_LINE), nullptr);
  EXPECT_FALSE(chat().isShown());
}

/**
 * Given a line half through the last second of its eight
 * When the closed chat is painted
 * Then its letters are written at half their alpha
 */
TEST_F(ChatWindowFixture, ALineFadesOverItsLastSecond) {
  channel().receive(ChatEntry{.playerNumber = NOVA_NUMBER,
                              .nickname = std::string{NOVA},
                              .text = std::string{SHORT_LINE},
                              .age = HALF_FADED});

  const RecordedWriting* writing = writingOf(painted(), SHORT_LINE);

  ASSERT_NE(writing, nullptr);
  EXPECT_EQ(writing->style.color.a, HALF_ALPHA);
}

/**
 * Given the background of the chat set to half
 * When the open chat is painted
 * Then the dim layer covers the whole picture at half its alpha
 */
TEST_F(ChatWindowFixture, OpenTheLayerHasTheOpacityOfTheSetting) {
  settings().chatBackground = HALF_BACKGROUND;
  openChat();

  const InterfaceCanvasRecord& record = painted();

  ASSERT_FALSE(record.fills.empty());
  EXPECT_EQ(record.fills.front().area, INTERFACE_AREA);
  EXPECT_EQ(record.fills.front().color.a, HALF_BACKGROUND_ALPHA);
}

/**
 * Given lines of twelve letters and a line of NOVA longer than one
 * When the closed chat is painted
 * Then the line is cut at its spaces, NOVA: starting the first part
 */
TEST_F(ChatWindowFixture, ALongLineWrapsAtTheLetterCount) {
  settings().chatLetters = NARROW_LETTERS;
  channel().receive(ChatEntry{.playerNumber = NOVA_NUMBER,
                              .nickname = std::string{NOVA},
                              .text = std::string{LONG_LINE},
                              .age = {}});

  const InterfaceCanvasRecord& record = painted();

  EXPECT_NE(writingOf(record, NOVA_HEAD), nullptr);
  EXPECT_NE(writingOf(record, LONG_LINE_FIRST), nullptr);
  EXPECT_NE(writingOf(record, LONG_LINE_SECOND), nullptr);
  EXPECT_NE(writingOf(record, LONG_LINE_THIRD), nullptr);
}

/**
 * Given ten lines and four open lines
 * When the open chat is painted, then PGUP pressed
 * Then lines 7 to 10 show with 6 above, then lines 3 to 6 with 2 above
 */
TEST_F(ChatWindowFixture, PageUpShowsOlderLines) {
  settings().chatOpenLines = FEW_OPEN_LINES;
  receiveNumberedLines(NUMBERED_LINES);
  openChat();
  EXPECT_NE(writingOf(painted(), NEWEST_SHOWN_FIRST), nullptr);
  EXPECT_NE(writingOf(painted(), MORE_BEFORE_SCROLL), nullptr);

  tap(Key::PageUp, Scan::PageUp);

  const InterfaceCanvasRecord& record = painted();
  EXPECT_NE(writingOf(record, SCROLLED_SHOWN_FIRST), nullptr);
  EXPECT_EQ(writingOf(record, SCROLLED_HIDDEN), nullptr);
  EXPECT_NE(writingOf(record, MORE_AFTER_SCROLL), nullptr);
}

/**
 * Given a line the client wrote itself
 * When the closed chat is painted
 * Then it is written in the notice color, with no name before it
 */
TEST_F(ChatWindowFixture, ALineOfTheClientIsPaleAndNameless) {
  channel().receive(ChatEntry{.playerNumber = CLIENT_WRITER,
                              .nickname = {},
                              .text = std::string{UNANSWERED_CHAT_TEXT},
                              .age = {}});

  const InterfaceCanvasRecord& record = painted();

  ASSERT_EQ(record.writings.size(), 1U);
  EXPECT_EQ(record.writings.front().text, UNANSWERED_CHAT_TEXT);
  EXPECT_EQ(record.writings.front().style.color, CHAT_NOTICE_COLOR);
}
