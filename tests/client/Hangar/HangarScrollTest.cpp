#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include "HangarScreenTestConstants.hpp"
#include "HangarScrollFixture.hpp"
#include "InterfaceColorConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "SoundConstants.hpp"

using rtype::client::HUD_CYAN;
using rtype::client::ROBOT_ARM_SOUND_ID;
using rtype::client::testing::CROWN_CELL_MIDDLE;
using rtype::client::testing::DOWNS_PAST_THE_ROWS_SHOWN;
using rtype::client::testing::DOWNS_TO_THE_ROW_OF_THE_HULLS;
using rtype::client::testing::FIRST_CELL_MIDDLE;
using rtype::client::testing::HangarScrollFixture;
using rtype::client::testing::LOCKED_HULL_STATUS_TEXT;
using rtype::client::testing::THUMB_AFTER_ONE_ROW;
using rtype::client::testing::THUMB_AT_OPENING;
using rtype::client::testing::WEARS_NOTHING;
using rtype::client::testing::WEARS_PROPELLER_CAP;
using rtype::client::testing::WEARS_VIKING_HELMET;
using rtype::client::testing::WEARS_WAR_SHARK;
using ::testing::Contains;
using ::testing::Not;
using Key = sf::Keyboard::Key;

/**
 * Given a hangar holding every item, its first four rows shown
 * When Down is pressed four times, then the cell of the crown is clicked
 * Then the locker scrolled one row: the ship wears the viking helmet
 */
TEST_F(HangarScrollFixture, TheArrowsScrollTheLockerWithTheChoice) {
  holdEveryItem();
  press(Key::Down, DOWNS_PAST_THE_ROWS_SHOWN);

  clickAt(CROWN_CELL_MIDDLE);

  EXPECT_EQ(settings().shipLook.head, WEARS_VIKING_HELMET);
}

/**
 * Given a hangar holding every item, its first four rows shown
 * When the wheel turns one notch down, then the first cell is clicked
 * Then the first cell holds the first item of the second row: the ship
 * wears the propeller cap
 */
TEST_F(HangarScrollFixture, TheWheelScrollsTheLockerOneRow) {
  holdEveryItem();
  wheelDownOver(FIRST_CELL_MIDDLE);

  clickAt(FIRST_CELL_MIDDLE);

  EXPECT_EQ(settings().shipLook.head, WEARS_PROPELLER_CAP);
}

/**
 * Given a hangar of thirty items in ten rows, just opened
 * When it is painted
 * Then the thumb of the scroll mark is at the top of its track
 */
TEST_F(HangarScrollFixture, TheScrollMarkStartsAtTheTop) {
  EXPECT_THAT(painting().fills, Contains(RecordedFill{.area = THUMB_AT_OPENING,
                                                      .color = HUD_CYAN}));
}

/**
 * Given a hangar of thirty items in ten rows
 * When the choice goes down past the rows shown
 * Then the thumb of the scroll mark is one row lower, 16 pixels
 */
TEST_F(HangarScrollFixture, TheScrollMarkFollowsTheRowsShown) {
  press(Key::Down, DOWNS_PAST_THE_ROWS_SHOWN);

  EXPECT_THAT(
      painting().fills,
      Contains(RecordedFill{.area = THUMB_AFTER_ONE_ROW, .color = HUD_CYAN}));
}

/**
 * Given a hangar holding every item, the war shark chosen
 * When Enter is pressed
 * Then the ship flies that hull at once, and no robot arm is heard
 */
TEST_F(HangarScrollFixture, AHullIsWornWithoutTheArms) {
  holdEveryItem();
  press(Key::Down, DOWNS_TO_THE_ROW_OF_THE_HULLS);
  press(Key::Right, 1);

  press(Key::Enter, 1);

  EXPECT_EQ(settings().shipLook.hull, WEARS_WAR_SHARK);
  EXPECT_THAT(sounds().played(),
              Not(Contains(std::string{ROBOT_ARM_SOUND_ID})));
}

/**
 * Given a hangar whose ship flies the war shark
 * When Enter is pressed on it again
 * Then the ship is back to the standard hull
 */
TEST_F(HangarScrollFixture, EnterOnAWornHullTakesItOff) {
  holdEveryItem();
  press(Key::Down, DOWNS_TO_THE_ROW_OF_THE_HULLS);
  press(Key::Right, 1);
  press(Key::Enter, 1);

  press(Key::Enter, 1);

  EXPECT_EQ(settings().shipLook.hull, WEARS_NOTHING);
}

/**
 * Given a hangar with the starter kit, the war shark chosen
 * When it is painted
 * Then the status line says a hull is bought at the shop
 */
TEST_F(HangarScrollFixture, TheStatusOfALockedHullSendsToTheShop) {
  press(Key::Down, DOWNS_TO_THE_ROW_OF_THE_HULLS);
  press(Key::Right, 1);

  EXPECT_NE(writingOf(painting(), LOCKED_HULL_STATUS_TEXT), nullptr);
}
