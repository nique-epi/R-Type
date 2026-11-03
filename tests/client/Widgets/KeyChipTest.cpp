#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Event.hpp>
#include "KeyChipFixture.hpp"
#include "KeyChipTestConstants.hpp"
#include "KeyPress.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "SoundConstants.hpp"
#include "WidgetColors.hpp"

using rtype::client::CLICK_SOUND_ID;
using rtype::client::SELECTED_BAR_BORDER_COLOR;
using rtype::client::testing::CHIP_KEY;
using rtype::client::testing::OFF_THE_CHIP;
using rtype::client::testing::ON_THE_CHIP;
using rtype::client::testing::OTHER_KEY;
using ::testing::Contains;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::Not;

/**
 * Given a chip
 * When its key is pressed
 * Then nothing runs yet: a chip acts on the release
 */
TEST_F(KeyChipFixture, APressAloneRunsNothing) {
  EXPECT_TRUE(give(keyPress(CHIP_KEY)));

  EXPECT_EQ(activations(), 0);
}

/**
 * Given a chip whose key is pressed
 * When it is released
 * Then the action runs once, with the click
 */
TEST_F(KeyChipFixture, ReleasingItsKeyRunsItsAction) {
  give(keyPress(CHIP_KEY));

  EXPECT_TRUE(give(keyRelease(CHIP_KEY)));

  EXPECT_EQ(activations(), 1);
  EXPECT_THAT(sounds().played(), ElementsAre(CLICK_SOUND_ID));
}

/**
 * Given a chip that never saw the press of its key, pressed on another screen
 * When the key is released
 * Then nothing runs
 */
TEST_F(KeyChipFixture, AReleaseWithoutItsPressRunsNothing) {
  give(keyRelease(CHIP_KEY));

  EXPECT_EQ(activations(), 0);
}

/**
 * Given a key held down, whose press the system repeats
 * When it is released
 * Then the action runs once
 */
TEST_F(KeyChipFixture, AHeldKeyActsOnce) {
  give(keyPress(CHIP_KEY));
  give(keyPress(CHIP_KEY));

  give(keyRelease(CHIP_KEY));

  EXPECT_EQ(activations(), 1);
}

/**
 * Given a chip
 * When another key is pressed
 * Then the event is left to the others
 */
TEST_F(KeyChipFixture, AnotherKeyIsNotTheChips) {
  EXPECT_FALSE(give(keyPress(OTHER_KEY)));
}

/**
 * Given a chip
 * When the left button is pressed and released over it
 * Then the action runs once
 */
TEST_F(KeyChipFixture, AClickRunsItsAction) {
  give(leftPress(ON_THE_CHIP));
  give(leftRelease(ON_THE_CHIP));

  EXPECT_EQ(activations(), 1);
}

/**
 * Given a press of the left button over a chip
 * When the button is released away from it
 * Then nothing runs
 */
TEST_F(KeyChipFixture, ReleasingAwayCancelsTheClick) {
  give(leftPress(ON_THE_CHIP));
  give(leftRelease(OFF_THE_CHIP));

  EXPECT_EQ(activations(), 0);
}

/**
 * Given a disabled chip
 * When its key is pressed and released
 * Then nothing runs
 */
TEST_F(KeyChipFixture, ADisabledChipRunsNothing) {
  chip().setEnabled(false);

  give(keyPress(CHIP_KEY));
  give(keyRelease(CHIP_KEY));

  EXPECT_EQ(activations(), 0);
}

/**
 * Given a key pressed, then the focus lost
 * When the key is released
 * Then nothing runs
 */
TEST_F(KeyChipFixture, LosingTheFocusDropsThePress) {
  give(keyPress(CHIP_KEY));
  give(sf::Event::FocusLost{});

  give(keyRelease(CHIP_KEY));

  EXPECT_EQ(activations(), 0);
}

/**
 * Given the pointer over a chip
 * When the focus is lost
 * Then it looks selected no more: nothing tells it where the pointer goes
 */
TEST_F(KeyChipFixture, LosingTheFocusForgetsThePointer) {
  give(pointerMove(ON_THE_CHIP));

  give(sf::Event::FocusLost{});

  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  chip().paint(canvas);
  EXPECT_THAT(
      record.fills,
      Not(Contains(Field(&RecordedFill::color, SELECTED_BAR_BORDER_COLOR))));
}

/**
 * Given a chip
 * When the pointer moves over it
 * Then it looks selected, and the move is left to the others
 */
TEST_F(KeyChipFixture, ThePointerOverItLooksSelected) {
  EXPECT_FALSE(give(pointerMove(ON_THE_CHIP)));

  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  chip().paint(canvas);
  EXPECT_THAT(record.fills,
              Contains(Field(&RecordedFill::color, SELECTED_BAR_BORDER_COLOR)));
}
