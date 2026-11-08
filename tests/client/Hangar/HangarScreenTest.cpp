#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <optional>
#include <vector>
#include "FittingTimelineConstants.hpp"
#include "HangarScreenFixture.hpp"
#include "HangarScreenFixtures.hpp"
#include "HangarScreenTestConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "ReducedMotionHangarFixture.hpp"

using rtype::client::FITTING_DURATION;
using rtype::client::testing::CROWN_CELL_MIDDLE;
using rtype::client::testing::CROWN_ICON_LEFT_ON_THE_SHEET;
using rtype::client::testing::HangarScreenFixture;
using rtype::client::testing::LOCKED_STATUS_TEXT;
using rtype::client::testing::LOOK_SHEET_ID;
using rtype::client::testing::lookImagesAtShipScale;
using rtype::client::testing::PARTY_HAT_STATUS;
using rtype::client::testing::ReducedMotionHangarFixture;
using rtype::client::testing::TOP_HAT_IN_THE_SHEET;
using rtype::client::testing::WEARS_CROWN;
using rtype::client::testing::WEARS_NOTHING;
using rtype::client::testing::WEARS_TOP_HAT;
using Key = sf::Keyboard::Key;

/**
 * Given a hangar, the first item chosen
 * When Right is pressed
 * Then the party hat is chosen, as its status line says
 */
TEST_F(HangarScreenFixture, RightChoosesTheNextItem) {
  holdEveryItem();
  press(Key::Right);

  EXPECT_NE(writingOf(painting(), PARTY_HAT_STATUS), nullptr);
}

/**
 * Given a hangar, the top hat chosen, the ship wearing nothing
 * When Enter is pressed
 * Then the settings wear the top hat, and the next update saves them once
 */
TEST_F(HangarScreenFixture, EnterWearsTheChosenItemAndSavesIt) {
  press(Key::Enter);

  EXPECT_EQ(settings().shipLook.head, WEARS_TOP_HAT);
  ASSERT_EQ(storage().saves().size(), 1U);
  EXPECT_EQ(storage().saves().back().shipLook.head, WEARS_TOP_HAT);
}

/**
 * Given a hangar whose ship wears the top hat, its fitting over
 * When Enter is pressed on the top hat again
 * Then the ship wears nothing
 */
TEST_F(HangarScreenFixture, EnterOnAWornItemTakesItOff) {
  press(Key::Enter);
  update(FITTING_DURATION);

  press(Key::Enter);

  EXPECT_EQ(settings().shipLook.head, WEARS_NOTHING);
}

/**
 * Given a hangar where an arm fits the top hat
 * When the crown is chosen and Enter pressed before the fitting ends
 * Then the ship still gets the top hat only
 */
TEST_F(HangarScreenFixture, EnterDuringAFittingChangesNothing) {
  press(Key::Enter);
  press(Key::Right);
  press(Key::Right);

  press(Key::Enter);

  EXPECT_EQ(settings().shipLook.head, WEARS_TOP_HAT);
}

/**
 * Given a hangar under reduced motion
 * When Enter is held, its press repeated before any release
 * Then the top hat is worn, not worn then taken off
 */
TEST_F(ReducedMotionHangarFixture, AHeldEnterActsOnce) {
  hold(Key::Enter);

  hold(Key::Enter);

  EXPECT_EQ(settings().shipLook.head, WEARS_TOP_HAT);
}

/**
 * Given a hangar where Enter just asked for the top hat
 * When the arm has not reached the ship yet
 * Then nothing of the item sheet is drawn at the scale of the ship
 */
TEST_F(HangarScreenFixture, TheItemIsNotOnTheShipBeforeTheArmSetsItDown) {
  press(Key::Enter);

  EXPECT_EQ(lookImagesAtShipScale(painting(), std::nullopt), 0U);
}

/**
 * Given a hangar where Enter asked for the top hat
 * When the whole fitting has run
 * Then the ship wears it: one drawing of the top hat at the scale of the ship
 */
TEST_F(HangarScreenFixture, TheItemIsOnTheShipOnceTheFittingEnded) {
  press(Key::Enter);

  update(FITTING_DURATION);

  EXPECT_EQ(lookImagesAtShipScale(painting(), TOP_HAT_IN_THE_SHEET), 1U);
}

/**
 * Given a hangar under reduced motion
 * When Enter asks for the top hat
 * Then the ship wears it in the same frame, no arm bringing it
 */
TEST_F(ReducedMotionHangarFixture, TheItemShowsAtOnceUnderReducedMotion) {
  press(Key::Enter);

  EXPECT_EQ(lookImagesAtShipScale(painting(), TOP_HAT_IN_THE_SHEET), 1U);
}

/**
 * Given a hangar
 * When Escape is pressed
 * Then the hangar asks to be popped, and nothing else
 */
TEST_F(HangarScreenFixture, EscapeLeavesTheHangar) {
  press(Key::Escape);

  EXPECT_EQ(navigator().calls(),
            (std::vector<NavigatorCall>{
                {.request = NavigatorRequest::Pop, .screen = std::nullopt}}));
}

/**
 * Given a hangar
 * When the cell of the crown is clicked
 * Then the ship wears the crown
 */
TEST_F(HangarScreenFixture, AClickOnACellWearsItsItem) {
  holdEveryItem();
  clickAt(CROWN_CELL_MIDDLE);

  EXPECT_EQ(settings().shipLook.head, WEARS_CROWN);
}

/**
 * Given a hangar with the starter kit, the crown chosen
 * When Enter is pressed
 * Then the ship wears nothing, nothing is saved and no click plays
 */
TEST_F(HangarScreenFixture, ALockedItemIsNotWorn) {
  press(Key::Right);
  press(Key::Right);

  press(Key::Enter);

  EXPECT_EQ(settings().shipLook.head, 0);
  EXPECT_TRUE(storage().saves().empty());
}

/**
 * Given a hangar with the starter kit
 * When it is painted
 * Then no icon of the crown, a locked item, is drawn on the sheet of the
 * looks: its cell stays empty
 */
TEST_F(HangarScreenFixture, ALockedCellShowsNoIcon) {
  update(rtype::engine::Duration::zero());

  EXPECT_FALSE(
      std::ranges::any_of(painting().images, [](const RecordedImage& image) {
        return image.assetId == LOOK_SHEET_ID &&
               image.source.position.x == CROWN_ICON_LEFT_ON_THE_SHEET;
      }));
}

/**
 * Given a hangar with the starter kit, the crown chosen
 * When it is painted
 * Then the status line says the item is locked and how to get it
 */
TEST_F(HangarScreenFixture, TheStatusOfALockedItemSaysHowToGetIt) {
  press(Key::Right);
  press(Key::Right);

  EXPECT_NE(writingOf(painting(), LOCKED_STATUS_TEXT), nullptr);
}
