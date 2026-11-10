#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include "RecordingNavigator.hpp"
#include "UpdatePhase.hpp"
#include "UpdateScreenFixture.hpp"
#include "UpdateScreenTestConstants.hpp"

using rtype::client::UpdatePhase;
using rtype::client::testing::A_FEW_FRAMES;
using rtype::client::testing::DOWNLOADING_42_PERCENT;
using rtype::client::testing::PAST_THE_MESSAGE;
using rtype::client::testing::PERCENT_RECEIVED;
using rtype::client::testing::SHORT_OF_THE_FAILURE;
using rtype::client::testing::SHORT_OF_THE_RESTART;
using rtype::client::testing::UPDATE_FRAME;
using rtype::client::testing::VERSION_LINE;
using rtype::client::testing::WHOLE_PACKAGE;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given 42 % of a package received
 * When a frame passes
 * Then the panel shows the versions and DOWNLOADING 42%
 */
TEST_F(UpdateScreenFixture, ShowsTheShareOfThePackageReceived) {
  run().set({.phase = UpdatePhase::Downloading, .percent = PERCENT_RECEIVED});

  screen().update(UPDATE_FRAME);

  EXPECT_TRUE(wrote(DOWNLOADING_42_PERCENT));
  EXPECT_TRUE(wrote(VERSION_LINE));
}

/**
 * Given a download in progress
 * When the player presses Escape, then Enter, then clicks
 * Then the panel asks nothing of the screens
 */
TEST_F(UpdateScreenFixture, KeepsEveryKeyAndClickFromTheScreensBelow) {
  run().set({.phase = UpdatePhase::Downloading, .percent = PERCENT_RECEIVED});

  screen().handleEvent(
      sf::Event::KeyPressed{.code = sf::Keyboard::Key::Escape});
  screen().handleEvent(sf::Event::KeyPressed{.code = sf::Keyboard::Key::Enter});
  screen().handleEvent(sf::Event::MouseButtonPressed{});
  runFor(A_FEW_FRAMES);

  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given a failed update
 * When 2.9 s pass
 * Then the panel shows UPDATE FAILED and stays
 */
TEST_F(UpdateScreenFixture, ShowsTheFailureForThreeSeconds) {
  run().set({.phase = UpdatePhase::Failed, .percent = 0});

  runFor(SHORT_OF_THE_FAILURE);

  EXPECT_TRUE(wrote("UPDATE FAILED"));
  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given a failed update shown for 2.9 s
 * When 0.2 s more pass
 * Then the panel closes, once
 */
TEST_F(UpdateScreenFixture, LetsThePlayerGoThreeSecondsAfterAFailure) {
  run().set({.phase = UpdatePhase::Failed, .percent = 0});
  runFor(SHORT_OF_THE_FAILURE);

  runFor(PAST_THE_MESSAGE);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given an installed update
 * When 0.9 s pass
 * Then the panel shows RESTARTING and has not started the new version yet
 */
TEST_F(UpdateScreenFixture, ShowsTheRestartForASecond) {
  run().set({.phase = UpdatePhase::ReadyToRestart, .percent = WHOLE_PACKAGE});

  runFor(SHORT_OF_THE_RESTART);

  EXPECT_TRUE(wrote("RESTARTING"));
  EXPECT_EQ(run().restartCount(), 0);
}

/**
 * Given an installed update shown for 0.9 s
 * When 0.2 s more pass
 * Then the new version starts once and the client closes
 */
TEST_F(UpdateScreenFixture, RestartsOneSecondAfterTheInstallThenCloses) {
  run().set({.phase = UpdatePhase::ReadyToRestart, .percent = WHOLE_PACKAGE});
  runFor(SHORT_OF_THE_RESTART);

  runFor(PAST_THE_MESSAGE);

  EXPECT_EQ(run().restartCount(), 1);
  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::CloseAll,
                                        .screen = std::nullopt}));
}

/**
 * Given an installed update whose new version will not start
 * When 1.1 s pass
 * Then the client stays, and says the update failed
 */
TEST_F(UpdateScreenFixture, ShowsTheFailureWhenTheNewVersionCannotStart) {
  run().set({.phase = UpdatePhase::ReadyToRestart, .percent = WHOLE_PACKAGE});
  run().answerRestart(false);

  runFor(SHORT_OF_THE_RESTART + PAST_THE_MESSAGE);

  EXPECT_THAT(navigator().calls(), IsEmpty());
  EXPECT_TRUE(wrote("UPDATE FAILED"));
}
