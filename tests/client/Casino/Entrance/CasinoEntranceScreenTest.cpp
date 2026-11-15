#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <optional>
#include "CasinoEntranceScreenFixture.hpp"
#include "CasinoEntranceScreenTestConstants.hpp"
#include "CasinoPlace.hpp"
#include "CasinoScreenIdConstants.hpp"
#include "MotionPreference.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"

using rtype::client::CASINO_BUY_CHIPS_SCREEN;
using rtype::client::CASINO_CASH_OUT_SCREEN;
using rtype::client::MotionPreference;
using rtype::client::ROULETTE_SCREEN;
using rtype::client::testing::CHIPS_HELD_TEXT;
using rtype::client::testing::DOORS_AJAR_PICTURE;
using rtype::client::testing::DOORS_CLOSED_PICTURE;
using rtype::client::testing::DOORS_OPEN_PICTURE;
using rtype::client::testing::DURING_THE_GREETING;
using rtype::client::testing::HALL_FIRST_LINE;
using rtype::client::testing::JUST_BEFORE_THE_HALL;
using rtype::client::testing::KOPECKS_HELD_TEXT;
using rtype::client::testing::ROULETTE_WORD;
using rtype::client::testing::WELCOME_FIRST_LINE;
using rtype::network::TablePlace;
using ::testing::ElementsAre;
using ::testing::IsEmpty;
using Key = sf::Keyboard::Key;

/**
 * Given a casino just opened
 * When it is painted
 * Then its doors are closed, the butler welcomes the player, and the player
 * is not seated at the table yet
 */
TEST_F(CasinoEntranceScreenFixture, OpensBeforeTheClosedDoors) {
  ASSERT_FALSE(canvas().images.empty());
  EXPECT_EQ(canvas().images.front().assetId, DOORS_CLOSED_PICTURE);
  EXPECT_NE(writingOf(canvas(), WELCOME_FIRST_LINE), nullptr);
  EXPECT_THAT(casino().chosenPlaces(), IsEmpty());
}

/**
 * Given a casino before its doors
 * When Enter is pressed and released
 * Then the player is seated at the table of the room
 */
TEST_F(CasinoEntranceScreenFixture, EnterSeatsThePlayerAtTheTable) {
  press(Key::Enter);

  EXPECT_THAT(casino().chosenPlaces(), ElementsAre(TablePlace::AtTable));
}

/**
 * Given a casino before its doors
 * When Enter is held down, its press repeated by the system, then released
 * Then the player is seated once
 */
TEST_F(CasinoEntranceScreenFixture, AHeldEnterSeatsThePlayerOnce) {
  hold(Key::Enter);
  hold(Key::Enter);
  release(Key::Enter);

  EXPECT_THAT(casino().chosenPlaces(), ElementsAre(TablePlace::AtTable));
}

/**
 * Given a casino before its doors
 * When Escape is pressed and released
 * Then the casino closes, back to the waiting room, the player never seated
 */
TEST_F(CasinoEntranceScreenFixture, BackClosesTheCasinoWithoutSeating) {
  press(Key::Escape);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
  EXPECT_THAT(casino().chosenPlaces(), IsEmpty());
}

/**
 * Given a casino whose player just entered
 * When the greeting of the butler runs
 * Then the doors stand ajar
 */
TEST_F(CasinoEntranceScreenFixture, TheDoorsStandAjarDuringTheGreeting) {
  press(Key::Enter);

  wait(DURING_THE_GREETING);

  ASSERT_FALSE(canvas().images.empty());
  EXPECT_EQ(canvas().images.front().assetId, DOORS_AJAR_PICTURE);
}

/**
 * Given a casino whose player just entered
 * When the doors have not finished opening
 * Then the hall does not show yet
 */
TEST_F(CasinoEntranceScreenFixture, TheHallWaitsForTheDoors) {
  press(Key::Enter);

  wait(JUST_BEFORE_THE_HALL);

  EXPECT_EQ(writingOf(canvas(), ROULETTE_WORD), nullptr);
}

/**
 * Given a casino whose player just entered
 * When the doors finished opening
 * Then the hall shows, its doors open, with what the butler offers and the
 * balances of the player
 */
TEST_F(CasinoEntranceScreenFixture, TheHallShowsOnceTheDoorsOpened) {
  enterTheHall();

  ASSERT_FALSE(canvas().images.empty());
  EXPECT_EQ(canvas().images.front().assetId, DOORS_OPEN_PICTURE);
  EXPECT_NE(writingOf(canvas(), ROULETTE_WORD), nullptr);
  EXPECT_NE(writingOf(canvas(), HALL_FIRST_LINE), nullptr);
  EXPECT_NE(writingOf(canvas(), KOPECKS_HELD_TEXT), nullptr);
  EXPECT_NE(writingOf(canvas(), CHIPS_HELD_TEXT), nullptr);
}

/**
 * Given a casino whose doors are opening
 * When a key is pressed
 * Then the hall shows at once
 */
TEST_F(CasinoEntranceScreenFixture, AKeySkipsTheOpening) {
  press(Key::Enter);

  press(Key::Space);

  EXPECT_NE(writingOf(canvas(), ROULETTE_WORD), nullptr);
}

/**
 * Given a casino opened with less motion
 * When Enter is pressed and released
 * Then the hall shows at once
 */
TEST_F(CasinoEntranceScreenFixture, LessMotionOpensTheHallAtOnce) {
  reopenWith(MotionPreference::Reduced);

  press(Key::Enter);

  EXPECT_NE(writingOf(canvas(), ROULETTE_WORD), nullptr);
}

/**
 * Given a casino opened with less motion, Enter just released on the doors
 * When nothing more is pressed
 * Then the roulette does not open: the release of ENTER is not a press of
 * ROULETTE
 */
TEST_F(CasinoEntranceScreenFixture, TheReleaseOfEnterDoesNotOpenTheRoulette) {
  reopenWith(MotionPreference::Reduced);

  press(Key::Enter);

  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given the hall
 * When Enter is pressed and released
 * Then the roulette opens over the hall
 */
TEST_F(CasinoEntranceScreenFixture, EnterInTheHallOpensTheRoulette) {
  enterTheHall();

  press(Key::Enter);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = ROULETTE_SCREEN}));
}

/**
 * Given the hall
 * When B is pressed and released
 * Then the cashier opens over the hall, to buy chips
 */
TEST_F(CasinoEntranceScreenFixture, BOpensTheCashierToBuyChips) {
  enterTheHall();

  press(Key::B);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = CASINO_BUY_CHIPS_SCREEN}));
}

/**
 * Given the hall
 * When C is pressed and released
 * Then the cashier opens over the hall, to cash out
 */
TEST_F(CasinoEntranceScreenFixture, COpensTheCashierToCashOut) {
  enterTheHall();

  press(Key::C);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = CASINO_CASH_OUT_SCREEN}));
}

/**
 * Given the hall
 * When B is held down, its press repeated by the system, then released
 * Then the cashier opens once
 */
TEST_F(CasinoEntranceScreenFixture, AHeldKeyOfTheHallActsOnce) {
  enterTheHall();

  hold(Key::B);
  hold(Key::B);
  release(Key::B);

  EXPECT_EQ(
      std::ranges::count(navigator().calls(),
                         NavigatorCall{.request = NavigatorRequest::Push,
                                       .screen = CASINO_BUY_CHIPS_SCREEN}),
      1);
}

/**
 * Given the hall
 * When Escape is pressed and released
 * Then the player goes back to the room, after being seated, and the casino
 * closes
 */
TEST_F(CasinoEntranceScreenFixture, LeaveTakesThePlayerBackToTheRoom) {
  enterTheHall();

  press(Key::Escape);

  EXPECT_THAT(casino().chosenPlaces(),
              ElementsAre(TablePlace::AtTable, TablePlace::InRoom));
  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}
