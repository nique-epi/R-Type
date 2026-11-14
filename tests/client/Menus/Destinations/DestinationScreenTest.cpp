#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <optional>
#include "DestinationScreenFixture.hpp"
#include "DestinationScreenTestConstants.hpp"
#include "HangarScreenIdConstants.hpp"
#include "KeyPress.hpp"
#include "MenuScreenIdConstants.hpp"
#include "PlayStart.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "ScreenTestConstants.hpp"
#include "ShopScreenIdConstants.hpp"
#include "SoundConstants.hpp"

using rtype::client::CLICK_SOUND_ID;
using rtype::client::CONNECTION_TO_CHOSEN_SERVER_SCREEN;
using rtype::client::HANGAR_SCREEN;
using rtype::client::PlayStart;
using rtype::client::SERVER_MAP_SCREEN;
using rtype::client::SHOP_SCREEN;
using rtype::client::testing::BALANCE_TEXT;
using rtype::client::testing::CHOSEN_CARD_MIDDLE;
using rtype::client::testing::HANGAR_HOUSE_SOURCE;
using rtype::client::testing::HOUSE_FILLING_THE_BOX;
using rtype::client::testing::MAP_SHEET;
using rtype::client::testing::RIGHT_CARD_MIDDLE;
using rtype::client::testing::SERVER_DESCRIPTION_LINE;
using rtype::client::testing::SPENT_BALANCE;
using rtype::client::testing::SPENT_BALANCE_TEXT;
using rtype::client::testing::TWICE_A_LINE;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given the destination screen just opened
 * When it is painted
 * Then SERVER stands in the middle, its name twice the size of a line, and
 * its description is written under the cards
 */
TEST_F(DestinationScreenFixture, SERVERStandsInTheMiddleWhenTheScreenOpens) {
  const RecordedWriting* name = writingOf(canvas(), "SERVER");

  ASSERT_NE(name, nullptr);
  EXPECT_EQ(name->style.scale, TWICE_A_LINE);
  EXPECT_NE(writingOf(canvas(), SERVER_DESCRIPTION_LINE), nullptr);
}

/**
 * Given the destination screen, SERVER in the middle
 * When Enter is pressed and released
 * Then the server map is asked for over it
 */
TEST_F(DestinationScreenFixture, EnterOnSERVEROpensTheServerMap) {
  press(sf::Keyboard::Key::Enter);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = SERVER_MAP_SCREEN}));
}

/**
 * Given the destination screen of a client launched with -h
 * When Enter is pressed and released on SERVER
 * Then the connection to the server given at launch is asked for, not the
 * map
 */
TEST_F(DestinationScreenFixture, SERVERJoinsTheServerGivenAtLaunch) {
  reopenWith(PlayStart::GivenServer);

  press(sf::Keyboard::Key::Enter);

  EXPECT_THAT(
      navigator().calls(),
      ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                .screen = CONNECTION_TO_CHOSEN_SERVER_SCREEN}));
}

/**
 * Given the destination screen, SERVER in the middle
 * When Right is pressed and released
 * Then SHOP comes to the middle, its name twice the size of a line, and
 * nothing opens
 */
TEST_F(DestinationScreenFixture, RightBringsSHOPToTheMiddle) {
  press(sf::Keyboard::Key::Right);

  const RecordedWriting* name = writingOf(canvas(), "SHOP");
  ASSERT_NE(name, nullptr);
  EXPECT_EQ(name->style.scale, TWICE_A_LINE);
  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given the destination screen, SERVER in the middle
 * When Right, then Enter, is pressed and released
 * Then the shop is asked for
 */
TEST_F(DestinationScreenFixture, RightThenEnterOpensTheShop) {
  press(sf::Keyboard::Key::Right);

  press(sf::Keyboard::Key::Enter);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = SHOP_SCREEN}));
}

/**
 * Given the destination screen, SERVER in the middle
 * When Left, then Enter, is pressed and released
 * Then the hangar is asked for
 */
TEST_F(DestinationScreenFixture, LeftThenEnterOpensTheHangar) {
  press(sf::Keyboard::Key::Left);

  press(sf::Keyboard::Key::Enter);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = HANGAR_SCREEN}));
}

/**
 * Given the destination screen, SERVER in the middle
 * When the card at the right is clicked
 * Then SHOP comes to the middle, and nothing opens
 */
TEST_F(DestinationScreenFixture, AClickOnTheCardAtTheRightTurnsWithoutOpening) {
  clickAt(RIGHT_CARD_MIDDLE);

  const RecordedWriting* name = writingOf(canvas(), "SHOP");
  ASSERT_NE(name, nullptr);
  EXPECT_EQ(name->style.scale, TWICE_A_LINE);
  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given the destination screen, SERVER in the middle
 * When the card in the middle is clicked
 * Then the server map is asked for, and the click plays once
 */
TEST_F(DestinationScreenFixture, AClickOnTheCardInTheMiddleOpensIt) {
  clickAt(CHOSEN_CARD_MIDDLE);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = SERVER_MAP_SCREEN}));
  EXPECT_THAT(sounds().played(), ElementsAre(CLICK_SOUND_ID));
}

/**
 * Given the destination screen
 * When Enter is held down, so its press repeats, then released
 * Then the card in the middle opens once
 */
TEST_F(DestinationScreenFixture, AHeldEnterOpensOnce) {
  screen().handleEvent(keyPress(sf::Keyboard::Key::Enter));
  screen().handleEvent(keyPress(sf::Keyboard::Key::Enter));
  screen().handleEvent(keyRelease(sf::Keyboard::Key::Enter));

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = SERVER_MAP_SCREEN}));
}

/**
 * Given the destination screen
 * When Escape is pressed and released
 * Then the screen asks to be closed, back to the title
 */
TEST_F(DestinationScreenFixture, EscapeClosesTheScreen) {
  press(sf::Keyboard::Key::Escape);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given the destination screen just opened, HANGAR at the left
 * When it is painted
 * Then the sprite of HANGAR is the house of the hangar of the sheet of the
 * map, enlarged to fill the box of a sprite, at the side as in the middle
 */
TEST_F(DestinationScreenFixture, TheHangarCardShowsTheHouseOfTheMap) {
  EXPECT_TRUE(std::ranges::any_of(
      canvas().images, [](const RecordedImage& image) noexcept {
        return image.assetId == MAP_SHEET &&
               image.source == HANGAR_HOUSE_SOURCE &&
               image.scale == HOUSE_FILLING_THE_BOX;
      }));
}

/**
 * Given a player with a balance of kopecks
 * When the destination screen is painted
 * Then the balance is written, thousands apart
 */
TEST_F(DestinationScreenFixture, TheBalanceOfKopecksIsShown) {
  EXPECT_NE(writingOf(canvas(), BALANCE_TEXT), nullptr);
}

/**
 * Given the destination screen, the shop pushed over it
 * When the player spends kopecks there, and a frame passes with no event
 * Then the screen shows the new balance
 */
TEST_F(DestinationScreenFixture, TheBalanceSpentInTheShopIsShownOnTheWayBack) {
  settings().kopecks = SPENT_BALANCE;

  screen().update(ONE_FRAME);

  EXPECT_NE(writingOf(canvas(), SPENT_BALANCE_TEXT), nullptr);
}
