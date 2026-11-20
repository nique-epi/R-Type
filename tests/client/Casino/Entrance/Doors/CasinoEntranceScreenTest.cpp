#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <optional>
#include "Ambience.hpp"
#include "CasinoEntranceScreenFixture.hpp"
#include "CasinoEntranceScreenTestConstants.hpp"
#include "CasinoScreenIdConstants.hpp"
#include "MotionPreference.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"

using rtype::client::Ambience;
using rtype::client::CASINO_HALL_SCREEN;
using rtype::client::MotionPreference;
using rtype::client::testing::BUTLER_NAME_ON_HIS_PANEL;
using rtype::client::testing::DOORS_CLOSED_PICTURE;
using rtype::client::testing::DOORS_OPEN_PICTURE;
using rtype::client::testing::DOORS_OPENING_TIME;
using rtype::client::testing::DURING_THE_BOW;
using rtype::client::testing::DURING_THE_GREETING;
using rtype::client::testing::FIRST_GRUNT;
using rtype::client::testing::JUST_BEFORE_THE_HALL;
using rtype::client::testing::THE_WHOLE_WELCOME;
using rtype::client::testing::WELCOME_FIRST_WORD;
using ::testing::ElementsAre;
using ::testing::IsEmpty;
using Key = sf::Keyboard::Key;

/**
 * Given a casino just opened
 * When it is painted
 * Then its doors are closed, and the panel of the butler shows his name
 */
TEST_F(CasinoEntranceScreenFixture, OpensBeforeTheClosedDoors) {
  ASSERT_FALSE(canvas().images.empty());
  EXPECT_EQ(canvas().images.front().assetId, DOORS_CLOSED_PICTURE);
  EXPECT_NE(writingOf(canvas(), BUTLER_NAME_ON_HIS_PANEL), nullptr);
}

/**
 * Given a casino just opened
 * When the time of the whole welcome passes
 * Then the butler wrote it in his panel, where it stays
 */
TEST_F(CasinoEntranceScreenFixture, TheButlerWritesHisWelcome) {
  wait(THE_WHOLE_WELCOME);

  EXPECT_NE(writingOf(canvas(), WELCOME_FIRST_WORD), nullptr);
}

/**
 * Given a casino just opened
 * When the butler starts his welcome
 * Then he speaks in the voice of the Bydo of the shop
 */
TEST_F(CasinoEntranceScreenFixture, TheButlerSpeaksInTheVoiceOfTheBydo) {
  ASSERT_FALSE(played().empty());
  EXPECT_EQ(played().front(), FIRST_GRUNT);
}

/**
 * Given a casino just opened
 * When the music is read
 * Then the music before the doors plays
 */
TEST_F(CasinoEntranceScreenFixture, TheFrontPlaysItsMusic) {
  EXPECT_EQ(audio().ambience(), Ambience::CasinoFront);
}

/**
 * Given a casino whose player just entered
 * When the greeting of the butler runs
 * Then the doors stay closed until his bow
 */
TEST_F(CasinoEntranceScreenFixture, TheDoorsStayClosedDuringTheGreeting) {
  press(Key::Enter);

  wait(DURING_THE_GREETING);

  ASSERT_FALSE(canvas().images.empty());
  EXPECT_EQ(canvas().images.front().assetId, DOORS_CLOSED_PICTURE);
}

/**
 * Given a casino whose player just entered
 * When the butler bows
 * Then the doors stand open
 */
TEST_F(CasinoEntranceScreenFixture, TheDoorsOpenWithTheBow) {
  press(Key::Enter);

  wait(DURING_THE_BOW);

  ASSERT_FALSE(canvas().images.empty());
  EXPECT_EQ(canvas().images.front().assetId, DOORS_OPEN_PICTURE);
}

/**
 * Given a casino whose player just entered
 * When the doors have not finished opening
 * Then the hall does not open yet
 */
TEST_F(CasinoEntranceScreenFixture, TheHallWaitsForTheDoors) {
  press(Key::Enter);

  wait(JUST_BEFORE_THE_HALL);

  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given a casino whose player just entered
 * When the doors finished opening
 * Then the hall opens over the doors
 */
TEST_F(CasinoEntranceScreenFixture, TheHallOpensOnceTheDoorsOpened) {
  press(Key::Enter);

  wait(DOORS_OPENING_TIME);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = CASINO_HALL_SCREEN}));
}

/**
 * Given a casino whose hall just opened over its doors
 * When the doors are painted again, under the hall
 * Then they stand closed, for the player who walks back to them
 */
TEST_F(CasinoEntranceScreenFixture, TheDoorsCloseBehindThePlayer) {
  press(Key::Enter);
  wait(DOORS_OPENING_TIME);

  wait(DOORS_OPENING_TIME);

  ASSERT_FALSE(canvas().images.empty());
  EXPECT_EQ(canvas().images.front().assetId, DOORS_CLOSED_PICTURE);
  EXPECT_EQ(navigator().calls().size(), 1U);
}

/**
 * Given a casino whose doors are opening
 * When a key is pressed
 * Then the hall opens at once
 */
TEST_F(CasinoEntranceScreenFixture, AKeySkipsTheOpening) {
  press(Key::Enter);

  press(Key::Space);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = CASINO_HALL_SCREEN}));
}

/**
 * Given a casino opened with less motion
 * When Enter is pressed and released
 * Then the hall opens at once
 */
TEST_F(CasinoEntranceScreenFixture, LessMotionOpensTheHallAtOnce) {
  reopenWith(MotionPreference::Reduced);

  press(Key::Enter);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = CASINO_HALL_SCREEN}));
}

/**
 * Given a casino opened with less motion
 * When Enter is held down, its press repeated by the system, then released
 * Then the hall opens once
 */
TEST_F(CasinoEntranceScreenFixture, AHeldEnterOpensTheHallOnce) {
  reopenWith(MotionPreference::Reduced);

  hold(Key::Enter);
  hold(Key::Enter);
  release(Key::Enter);

  EXPECT_EQ(std::ranges::count(navigator().calls(),
                               NavigatorCall{.request = NavigatorRequest::Push,
                                             .screen = CASINO_HALL_SCREEN}),
            1);
}

/**
 * Given a casino before its doors
 * When Escape is pressed and released
 * Then the casino closes, back to the map
 */
TEST_F(CasinoEntranceScreenFixture, BackClosesTheCasino) {
  press(Key::Escape);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}
