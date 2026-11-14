#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include <algorithm>
#include <string>
#include <vector>
#include "RecordingInterfaceCanvas.hpp"
#include "SpokenLineFixture.hpp"
#include "SpokenLineTestConstants.hpp"
#include "WidgetColors.hpp"

using rtype::client::PANEL_BORDER_COLOR;
using rtype::client::testing::BOTTLES_LINE;
using rtype::client::testing::GREETING_LINE;
using rtype::client::testing::GREETING_LINE_SHOWN;
using rtype::client::testing::GREETING_LINE_WORDS;
using rtype::client::testing::RADIO_LINE;
using rtype::client::testing::SECOND_WORD_STARTS;
using rtype::client::testing::SHORT_FRAME;
using rtype::client::testing::SpokenLineFixture;
using rtype::client::testing::TEST_TAIL_FIRST_ROW_LEFT;
using rtype::client::testing::TEST_TAIL_FIRST_ROW_WIDTH;
using rtype::client::testing::TEST_VOICE;
using rtype::client::testing::TWO_LETTER_TIMES;

/**
 * Given a silent line
 * When it says a greeting
 * Then the first sound of the voice plays at once, and its first letter
 * shows
 */
TEST_F(SpokenLineFixture, TheFirstWordSpeaksAtOnce) {
  line().say(GREETING_LINE);
  EXPECT_EQ(played(), std::vector<std::string>{TEST_VOICE.at(0)});
  EXPECT_EQ(written(), std::vector<std::string>{"P"});
}

/**
 * Given a greeting being said
 * When two letter times pass
 * Then its first three letters show
 */
TEST_F(SpokenLineFixture, ALetterShowsEveryLetterTime) {
  line().say(GREETING_LINE);
  walk(TWO_LETTER_TIMES);
  EXPECT_EQ(written(), std::vector<std::string>{"PRI"});
}

/**
 * Given a greeting being said
 * When time passes up to a frame before its second word, then that frame
 * Then the second sound of the voice plays only once the second word starts
 */
TEST_F(SpokenLineFixture, EachWordStartsWithTheNextSoundOfTheVoice) {
  line().say(GREETING_LINE);
  walk(SECOND_WORD_STARTS - SHORT_FRAME);
  EXPECT_EQ(played().size(), 1U);
  walk(SHORT_FRAME);
  EXPECT_EQ(played(),
            (std::vector<std::string>{TEST_VOICE.at(0), TEST_VOICE.at(1)}));
}

/**
 * Given a greeting being said
 * When it was written and lingered all but a frame, then that frame
 * Then its bubble shows, then goes
 */
TEST_F(SpokenLineFixture, TheBubbleGoesOnceTheLineLingered) {
  line().say(GREETING_LINE);
  walk(GREETING_LINE_SHOWN - SHORT_FRAME);
  EXPECT_TRUE(line().isSpeaking());
  EXPECT_EQ(written(),
            (std::vector<std::string>{std::string(GREETING_LINE_WORDS.at(0)),
                                      std::string(GREETING_LINE_WORDS.at(1))}));
  walk(SHORT_FRAME);
  EXPECT_FALSE(line().isSpeaking());
  EXPECT_TRUE(written().empty());
}

/**
 * Given a greeting being said, and a line said next
 * When the greeting is gone
 * Then the line next starts, with the next sound of the voice
 */
TEST_F(SpokenLineFixture, TheLineNextFollowsTheLineShown) {
  line().say(GREETING_LINE);
  line().sayNext(RADIO_LINE);
  walk(SECOND_WORD_STARTS);
  EXPECT_EQ(played().size(), 2U);
  walk(GREETING_LINE_SHOWN - SECOND_WORD_STARTS);
  EXPECT_TRUE(line().isSpeaking());
  EXPECT_EQ(written(), std::vector<std::string>{"R"});
  EXPECT_EQ(played().back(), TEST_VOICE.at(0));
}

/**
 * Given a greeting being said, a line said next, then another line said
 * When the other line is gone
 * Then nothing follows it
 */
TEST_F(SpokenLineFixture, ANewLineForgetsTheLineNext) {
  line().say(GREETING_LINE);
  line().sayNext(RADIO_LINE);
  line().say(BOTTLES_LINE);
  walk(GREETING_LINE_SHOWN);
  EXPECT_FALSE(line().isSpeaking());
}

/**
 * Given a silent line
 * When a line is said next
 * Then it is said at once
 */
TEST_F(SpokenLineFixture, ALineNextWithNothingSaidIsSaidAtOnce) {
  line().sayNext(RADIO_LINE);
  EXPECT_TRUE(line().isSpeaking());
  EXPECT_EQ(played().size(), 1U);
}

/**
 * Given a player who asked for less motion
 * When a greeting is said, then written to its end
 * Then all of it shows at once, with a single sound
 */
TEST_F(SpokenLineFixture, LessMotionShowsTheWholeLineWithOneSound) {
  rebuildWithLessMotion();
  line().say(GREETING_LINE);
  EXPECT_EQ(written(),
            (std::vector<std::string>{std::string(GREETING_LINE_WORDS.at(0)),
                                      std::string(GREETING_LINE_WORDS.at(1))}));
  walk(SECOND_WORD_STARTS);
  EXPECT_EQ(played().size(), 1U);
}

/**
 * Given a line with no voice
 * When a greeting is said to its end
 * Then no sound plays
 */
TEST_F(SpokenLineFixture, ALineWithNoVoiceIsSilent) {
  rebuildWithoutVoice();
  line().say(GREETING_LINE);
  walk(SECOND_WORD_STARTS);
  EXPECT_TRUE(played().empty());
}

/**
 * Given a line with a tail, saying a greeting
 * When it is painted
 * Then the first row of the tail lies right under the bubble, in the color
 * of its border
 */
TEST_F(SpokenLineFixture, TheTailHangsUnderTheBubble) {
  line().say(GREETING_LINE);
  static_cast<void>(written());
  const RecordedFill firstRow{
      .area =
          sf::IntRect(TEST_TAIL_FIRST_ROW_LEFT, {TEST_TAIL_FIRST_ROW_WIDTH, 1}),
      .color = PANEL_BORDER_COLOR};
  EXPECT_NE(std::ranges::find(painting().fills, firstRow),
            painting().fills.end());
}
