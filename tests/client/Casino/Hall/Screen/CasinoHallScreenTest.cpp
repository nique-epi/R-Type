#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <optional>
#include "CasinoHallScreenFixture.hpp"
#include "CasinoHallScreenTestConstants.hpp"
#include "CasinoScreenIdConstants.hpp"
#include "MotionPreference.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"

using rtype::client::CASINO_CASHIER_SCREEN;
using rtype::client::MotionPreference;
using rtype::client::ROULETTE_SCREEN;
using rtype::client::testing::HALL_FREE_ROOM;
using rtype::client::testing::HALL_ROOMS_ASKED_AGAIN_AFTER;
using rtype::client::testing::MAIN_HALL_TITLE;
using rtype::client::testing::MOST_OF_THE_FADE;
using rtype::client::testing::OVER_THE_LEFT_ARCH;
using rtype::client::testing::OVER_THE_RIGHT_ARCH;
using rtype::client::testing::OVER_THE_RING;
using rtype::client::testing::THE_WHOLE_FADE;
using ::testing::ElementsAre;
using ::testing::IsEmpty;
using Key = sf::Keyboard::Key;

/**
 * Given a hall just opened
 * When it is painted
 * Then it shows its title
 */
TEST_F(CasinoHallScreenFixture, ShowsItsTitle) {
  EXPECT_NE(writingOf(canvas(), MAIN_HALL_TITLE), nullptr);
}

/**
 * Given a hall just opened
 * When nothing is pressed
 * Then the master was asked the rooms once
 */
TEST_F(CasinoHallScreenFixture, AsksTheRoomsAsItOpens) {
  EXPECT_EQ(casino().roomsAsks, 1U);
}

/**
 * Given a hall just opened
 * When 5 s pass
 * Then the master is asked the rooms again
 */
TEST_F(CasinoHallScreenFixture, AsksTheRoomsAgainEveryFiveSeconds) {
  wait(HALL_ROOMS_ASKED_AGAIN_AFTER);

  EXPECT_EQ(casino().roomsAsks, 2U);
}

/**
 * Given the hall
 * When Right is pressed and released, then the hall faded
 * Then the roulette opens over the hall, the player in the room with a
 * seat free
 */
TEST_F(CasinoHallScreenFixture, RightOpensTheRouletteOnceTheHallFaded) {
  press(Key::Right);

  wait(THE_WHOLE_FADE);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = ROULETTE_SCREEN}));
  EXPECT_THAT(casino().enteredRooms, ElementsAre(HALL_FREE_ROOM));
}

/**
 * Given the hall
 * When Right is pressed and released, then most of the fade passes
 * Then nothing opens yet, and the player sits in no room
 */
TEST_F(CasinoHallScreenFixture, TheRouletteWaitsForTheFade) {
  press(Key::Right);

  wait(MOST_OF_THE_FADE);

  EXPECT_THAT(navigator().calls(), IsEmpty());
  EXPECT_THAT(casino().enteredRooms, IsEmpty());
}

/**
 * Given the hall, every room of the master full
 * When Right is pressed and released, then the time of a fade passes
 * Then the roulette does not open, and the player enters no room
 */
TEST_F(CasinoHallScreenFixture, TheRouletteWaitsForASeatFree) {
  fillEveryRoom();

  press(Key::Right);
  wait(THE_WHOLE_FADE);

  EXPECT_THAT(navigator().calls(), IsEmpty());
  EXPECT_THAT(casino().enteredRooms, IsEmpty());
}

/**
 * Given the hall
 * When Left is pressed and released, then the hall faded
 * Then the cashier opens over the hall
 */
TEST_F(CasinoHallScreenFixture, LeftOpensTheCashierOnceTheHallFaded) {
  press(Key::Left);

  wait(THE_WHOLE_FADE);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = CASINO_CASHIER_SCREEN}));
}

/**
 * Given a hall opened with less motion
 * When Right is pressed and released
 * Then the roulette opens at once
 */
TEST_F(CasinoHallScreenFixture, LessMotionOpensTheRouletteAtOnce) {
  reopenWith(MotionPreference::Reduced);

  press(Key::Right);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = ROULETTE_SCREEN}));
}

/**
 * Given a hall opened with less motion
 * When the right arch is clicked
 * Then the roulette opens
 */
TEST_F(CasinoHallScreenFixture, TheRightArchOpensTheRoulette) {
  reopenWith(MotionPreference::Reduced);

  click(OVER_THE_RIGHT_ARCH);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = ROULETTE_SCREEN}));
}

/**
 * Given a hall opened with less motion
 * When the left arch is clicked
 * Then the cashier opens
 */
TEST_F(CasinoHallScreenFixture, TheLeftArchOpensTheCashier) {
  reopenWith(MotionPreference::Reduced);

  click(OVER_THE_LEFT_ARCH);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = CASINO_CASHIER_SCREEN}));
}

/**
 * Given a hall opened with less motion
 * When the ring, between the arches, is clicked
 * Then nothing opens
 */
TEST_F(CasinoHallScreenFixture, AClickBetweenTheArchesOpensNothing) {
  reopenWith(MotionPreference::Reduced);

  click(OVER_THE_RING);

  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given a hall opened with less motion
 * When Right is held down, its press repeated by the system, then released
 * Then the roulette opens once
 */
TEST_F(CasinoHallScreenFixture, AHeldKeyOfTheHallActsOnce) {
  reopenWith(MotionPreference::Reduced);

  hold(Key::Right);
  hold(Key::Right);
  release(Key::Right);

  EXPECT_EQ(std::ranges::count(navigator().calls(),
                               NavigatorCall{.request = NavigatorRequest::Push,
                                             .screen = ROULETTE_SCREEN}),
            1);
}

/**
 * Given the hall
 * When Escape is pressed and released
 * Then the player gets up from its room, and walks back to the doors
 */
TEST_F(CasinoHallScreenFixture, EscapeWalksBackToTheDoors) {
  press(Key::Escape);

  EXPECT_EQ(casino().leaves, 1U);
  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}
