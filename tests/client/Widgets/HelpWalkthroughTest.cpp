#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include "BoundAction.hpp"
#include "EntitySpriteConstants.hpp"
#include "HelpWalkthroughFixture.hpp"
#include "HelpWalkthroughTestConstants.hpp"
#include "InterfaceConstants.hpp"
#include "KeyPress.hpp"
#include "MotionPreference.hpp"
#include "PointerEvents.hpp"
#include "ProfessorSpriteConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "SoundConstants.hpp"
#include "StatusBandConstants.hpp"
#include "TextStyle.hpp"

using rtype::client::BoundAction;
using rtype::client::BYDO_FRAME_TIME;
using rtype::client::CLICK_SOUND_ID;
using rtype::client::MotionPreference;
using rtype::client::PROFESSOR_FACING_LEFT_FRAMES;
using rtype::client::PROFESSOR_FACING_RIGHT_FRAMES;
using rtype::client::REGULAR_FONT_ADVANCE;
using rtype::client::STATUS_BAND_AREA;
using rtype::client::TextFont;
using rtype::client::testing::BACK_STEP;
using rtype::client::testing::BESIDE_HELP_CHIP;
using rtype::client::testing::BUBBLE_TEXT_LEFT;
using rtype::client::testing::BUBBLE_TEXT_RIGHT;
using rtype::client::testing::BYDOS_STEP;
using rtype::client::testing::CHIP_LABEL;
using rtype::client::testing::FIRST_STEP_WORD;
using rtype::client::testing::HELP_CHIP_MIDDLE;
using rtype::client::testing::LAST_ROOM_STEP;
using rtype::client::testing::LAST_STEP;
using rtype::client::testing::LAST_STEP_WORD;
using rtype::client::testing::MOVES_STEP;
using rtype::client::testing::OWN_BAY_STEP;
using rtype::client::testing::OWN_BAY_STEP_WORD;
using rtype::client::testing::READY_STEP;
using rtype::client::testing::READY_STEP_WORD;
using rtype::client::testing::ROOM_AREAS;
using rtype::client::testing::SECOND_STEP_WORD;
using rtype::client::testing::W_KEY_NAME;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::Gt;
using ::testing::Lt;
using ::testing::Optional;

/**
 * Given a closed help
 * When it is painted
 * Then it paints its HELP chip and no bubble
 */
TEST_F(HelpWalkthroughFixture, AClosedHelpPaintsItsChipOnly) {
  const InterfaceCanvasRecord& record = painted();

  EXPECT_NE(writingOf(record, CHIP_LABEL), nullptr);
  EXPECT_EQ(writingOf(record, FIRST_STEP_WORD), nullptr);
}

/**
 * Given a closed help
 * When H is pressed
 * Then the help takes the key and shows its first step
 */
TEST_F(HelpWalkthroughFixture, TheHelpKeyOpensTheFirstStep) {
  EXPECT_TRUE(press(sf::Keyboard::Key::H));

  EXPECT_NE(writingOf(painted(), FIRST_STEP_WORD), nullptr);
}

/**
 * Given a closed help
 * When its chip is clicked
 * Then the help takes the click and shows its first step
 */
TEST_F(HelpWalkthroughFixture, AClickOnTheChipOpensTheFirstStep) {
  EXPECT_TRUE(click(HELP_CHIP_MIDDLE));

  EXPECT_NE(writingOf(painted(), FIRST_STEP_WORD), nullptr);
}

/**
 * Given a closed help
 * When the room is clicked left of the chip
 * Then the help leaves the click to the room and stays closed
 */
TEST_F(HelpWalkthroughFixture, AClickBesideTheChipIsLeftToTheRoom) {
  EXPECT_FALSE(click(BESIDE_HELP_CHIP));

  EXPECT_EQ(writingOf(painted(), FIRST_STEP_WORD), nullptr);
}

/**
 * Given the help on its first step
 * When Enter is pressed
 * Then it shows the second step
 */
TEST_F(HelpWalkthroughFixture, EnterShowsTheNextStep) {
  showStep(0);

  press(sf::Keyboard::Key::Enter);

  EXPECT_NE(writingOf(painted(), SECOND_STEP_WORD), nullptr);
}

/**
 * Given the help on its first step
 * When the help is clicked anywhere
 * Then it shows the second step
 */
TEST_F(HelpWalkthroughFixture, AClickShowsTheNextStep) {
  showStep(0);

  click(BESIDE_HELP_CHIP);

  EXPECT_NE(writingOf(painted(), SECOND_STEP_WORD), nullptr);
}

/**
 * Given the help on its second step
 * When the left arrow is pressed
 * Then it shows the first step again
 */
TEST_F(HelpWalkthroughFixture, LeftShowsThePreviousStep) {
  showStep(1);

  press(sf::Keyboard::Key::Left);

  EXPECT_NE(writingOf(painted(), FIRST_STEP_WORD), nullptr);
}

/**
 * Given the help on its first step
 * When the left arrow is pressed
 * Then it stays on the first step, without a click
 */
TEST_F(HelpWalkthroughFixture, LeftOnTheFirstStepStaysThere) {
  showStep(0);
  const std::size_t clicksBefore = sounds().played().size();

  press(sf::Keyboard::Key::Left);

  EXPECT_NE(writingOf(painted(), FIRST_STEP_WORD), nullptr);
  EXPECT_EQ(sounds().played().size(), clicksBefore);
}

/**
 * Given the help on the step before the last
 * When Enter is pressed
 * Then it shows the last step, still open
 */
TEST_F(HelpWalkthroughFixture, EnterBeforeTheLastStepShowsTheLast) {
  showStep(LAST_STEP - 1);

  press(sf::Keyboard::Key::Enter);

  EXPECT_NE(writingOf(painted(), LAST_STEP_WORD), nullptr);
}

/**
 * Given the help on its last step
 * When Enter is pressed
 * Then the help closes: it paints no step, and leaves the next key to the
 * room
 */
TEST_F(HelpWalkthroughFixture, EnterOnTheLastStepClosesTheHelp) {
  showStep(LAST_STEP);

  press(sf::Keyboard::Key::Enter);

  EXPECT_EQ(writingOf(painted(), LAST_STEP_WORD), nullptr);
  EXPECT_FALSE(press(sf::Keyboard::Key::R));
}

/**
 * Given the help on a step in the middle
 * When Escape is pressed
 * Then the help closes and leaves the next key to the room
 */
TEST_F(HelpWalkthroughFixture, EscapeClosesTheHelp) {
  showStep(BACK_STEP);

  press(sf::Keyboard::Key::Escape);

  EXPECT_FALSE(press(sf::Keyboard::Key::R));
}

/**
 * Given an open help
 * When a key it does not use is pressed, then released
 * Then it takes the press and leaves the release to the room
 */
TEST_F(HelpWalkthroughFixture, AnOpenHelpTakesPressesButNotReleases) {
  showStep(0);

  EXPECT_TRUE(press(sf::Keyboard::Key::R));
  EXPECT_FALSE(
      help().handleEvent(keyRelease(sf::Keyboard::Key::R), samePlayfieldPoint));
}

/**
 * Given an open help
 * When a mouse button is released, then the window loses the focus
 * Then the help leaves both to the room, which may have started a click or
 * held a key before the help opened
 */
TEST_F(HelpWalkthroughFixture, AnOpenHelpLeavesTheEndOfAnInputToTheRoom) {
  showStep(0);

  EXPECT_FALSE(
      help().handleEvent(leftRelease(BESIDE_HELP_CHIP), samePlayfieldPoint));
  EXPECT_FALSE(help().handleEvent(sf::Event::FocusLost{}, samePlayfieldPoint));
}

/**
 * Given the help on the last step of the room
 * When Enter is pressed
 * Then the next step, the eighth, is the first to show a game
 */
TEST_F(HelpWalkthroughFixture, TheEighthStepIsTheFirstToShowAGame) {
  showStep(LAST_ROOM_STEP);
  EXPECT_FALSE(help().showsGame());

  press(sf::Keyboard::Key::Enter);

  EXPECT_TRUE(help().showsGame());
}

/**
 * Given the move up action bound to W
 * When the help shows how to move
 * Then a keycap names W
 */
TEST_F(HelpWalkthroughFixture, TheKeycapsNameTheBoundKeys) {
  bind(BoundAction::MoveUp, sf::Keyboard::Scan::W);

  showStep(MOVES_STEP);

  EXPECT_NE(writingOf(painted(), W_KEY_NAME), nullptr);
}

/**
 * Given the four moves bound to the keys with the longest names
 * When the help shows how to move
 * Then every word of its sentence, and every name on a keycap, stays inside
 * the bubble
 */
TEST_F(HelpWalkthroughFixture, LongKeyNamesStayInsideTheBubble) {
  bind(BoundAction::MoveUp, sf::Keyboard::Scan::RShift);
  bind(BoundAction::MoveDown, sf::Keyboard::Scan::RControl);
  bind(BoundAction::MoveLeft, sf::Keyboard::Scan::LShift);
  bind(BoundAction::MoveRight, sf::Keyboard::Scan::RAlt);

  showStep(MOVES_STEP);

  for (const RecordedWriting& writing : painted().writings) {
    if (writing.anchor.y < STATUS_BAND_AREA.position.y &&
        writing.style.font == TextFont::Regular) {
      SCOPED_TRACE(writing.text);
      EXPECT_GE(writing.anchor.x, BUBBLE_TEXT_LEFT);
      EXPECT_LE(writing.anchor.x + (static_cast<int>(writing.text.size()) *
                                    REGULAR_FONT_ADVANCE),
                BUBBLE_TEXT_RIGHT);
    }
  }
}

/**
 * Given the help on the step that explains READY, a part under the low bubble
 * When it is painted
 * Then its bubble climbs above READY
 */
TEST_F(HelpWalkthroughFixture, TheBubbleClimbsAboveReady) {
  showStep(READY_STEP);

  EXPECT_THAT(
      anchorOf(painted(), READY_STEP_WORD),
      Optional(Field(&sf::Vector2i::y, Lt(ROOM_AREAS.ready.position.y))));
}

/**
 * Given the help on the step that explains the bay of the local player
 * When it is painted
 * Then its bubble stays under the bays
 */
TEST_F(HelpWalkthroughFixture, TheBubbleStaysUnderTheBays) {
  showStep(OWN_BAY_STEP);

  EXPECT_THAT(anchorOf(painted(), OWN_BAY_STEP_WORD),
              Optional(Field(&sf::Vector2i::y, Gt(ROOM_AREAS.bays.position.y +
                                                  ROOM_AREAS.bays.size.y))));
}

/**
 * Given the help on the step that explains the Bydos (middle at x 361),
 * which push the professor to the left of the bubble (x 38 to 86), so they
 * lie on his right
 * When it is painted
 * Then the professor is drawn facing right
 */
TEST_F(HelpWalkthroughFixture, TheProfessorFacesThePartOnItsRight) {
  showStep(BYDOS_STEP);

  EXPECT_THAT(
      professorImage(),
      Optional(Field(&RecordedImage::source,
                     Field(&sf::IntRect::position,
                           Field(&sf::Vector2i::y,
                                 PROFESSOR_FACING_RIGHT_FRAMES.front().top)))));
}

/**
 * Given a help built while the system asks for less motion, on its first step
 * When a frame time of the Bydo goes by
 * Then the professor keeps his first frame
 */
TEST_F(HelpWalkthroughFixture, LessMotionKeepsTheFirstFrameOfTheProfessor) {
  rebuild(MotionPreference::Reduced);
  showStep(0);

  help().advance(BYDO_FRAME_TIME);

  EXPECT_THAT(
      professorImage(),
      Optional(Field(
          &RecordedImage::source,
          Field(&sf::IntRect::position,
                sf::Vector2i(PROFESSOR_FACING_LEFT_FRAMES.front().left,
                             PROFESSOR_FACING_LEFT_FRAMES.front().top)))));
}

/**
 * Given a closed help
 * When it opens, then shows its second step
 * Then each plays the click of the menus
 */
TEST_F(HelpWalkthroughFixture, EachStepPlaysTheClick) {
  press(sf::Keyboard::Key::H);
  press(sf::Keyboard::Key::Enter);

  EXPECT_THAT(sounds().played(), ElementsAre(CLICK_SOUND_ID, CLICK_SOUND_ID));
}
