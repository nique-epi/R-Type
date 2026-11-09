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
using rtype::client::testing::BYTES_RECEIVED;
using rtype::client::testing::DOWNLOADING_42_PERCENT;
using rtype::client::testing::PACKAGE_SIZE;
using rtype::client::testing::PAST_THE_MESSAGE;
using rtype::client::testing::SHORT_OF_THE_FAILURE;
using rtype::client::testing::SHORT_OF_THE_RESTART;
using rtype::client::testing::UPDATE_FRAME;
using rtype::client::testing::VERSION_LINE;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given 42 bytes of a package of 100 received
 * When a frame passes
 * Then the panel shows the versions and DOWNLOADING 42%
 */
TEST_F(UpdateScreenFixture, ShowsTheShareOfThePackageReceived) {
  run_.set({.phase = UpdatePhase::Downloading,
            .received = BYTES_RECEIVED,
            .size = PACKAGE_SIZE});

  screen_->update(UPDATE_FRAME);

  EXPECT_TRUE(wrote(DOWNLOADING_42_PERCENT));
  EXPECT_TRUE(wrote(VERSION_LINE));
}

/**
 * Given a download in progress
 * When the player presses Escape, then Enter, then clicks
 * Then the panel asks nothing of the screens
 */
TEST_F(UpdateScreenFixture, KeepsEveryKeyAndClickFromTheScreensBelow) {
  run_.set({.phase = UpdatePhase::Downloading,
            .received = BYTES_RECEIVED,
            .size = PACKAGE_SIZE});

  screen_->handleEvent(
      sf::Event::KeyPressed{.code = sf::Keyboard::Key::Escape});
  screen_->handleEvent(sf::Event::KeyPressed{.code = sf::Keyboard::Key::Enter});
  screen_->handleEvent(sf::Event::MouseButtonPressed{});
  runFor(A_FEW_FRAMES);

  EXPECT_THAT(navigator_.calls(), IsEmpty());
}

/**
 * Given a failed update
 * When 2.9 s pass
 * Then the panel shows UPDATE FAILED and stays
 */
TEST_F(UpdateScreenFixture, ShowsTheFailureForThreeSeconds) {
  run_.set({.phase = UpdatePhase::Failed, .size = PACKAGE_SIZE});

  runFor(SHORT_OF_THE_FAILURE);

  EXPECT_TRUE(wrote("UPDATE FAILED"));
  EXPECT_THAT(navigator_.calls(), IsEmpty());
}

/**
 * Given a failed update shown for 2.9 s
 * When 0.2 s more pass
 * Then the panel closes, once
 */
TEST_F(UpdateScreenFixture, LetsThePlayerGoThreeSecondsAfterAFailure) {
  run_.set({.phase = UpdatePhase::Failed, .size = PACKAGE_SIZE});
  runFor(SHORT_OF_THE_FAILURE);

  runFor(PAST_THE_MESSAGE);

  EXPECT_THAT(navigator_.calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given an installed update
 * When 0.9 s pass
 * Then the panel shows RESTARTING and has not started the new version yet
 */
TEST_F(UpdateScreenFixture, ShowsTheRestartForASecond) {
  run_.set({.phase = UpdatePhase::ReadyToRestart,
            .received = PACKAGE_SIZE,
            .size = PACKAGE_SIZE});

  runFor(SHORT_OF_THE_RESTART);

  EXPECT_TRUE(wrote("RESTARTING"));
  EXPECT_EQ(run_.restartCount(), 0);
}

/**
 * Given an installed update shown for 0.9 s
 * When 0.2 s more pass
 * Then the new version starts once and the client closes
 */
TEST_F(UpdateScreenFixture, RestartsOneSecondAfterTheInstallThenCloses) {
  run_.set({.phase = UpdatePhase::ReadyToRestart,
            .received = PACKAGE_SIZE,
            .size = PACKAGE_SIZE});
  runFor(SHORT_OF_THE_RESTART);

  runFor(PAST_THE_MESSAGE);

  EXPECT_EQ(run_.restartCount(), 1);
  EXPECT_THAT(navigator_.calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::CloseAll,
                                        .screen = std::nullopt}));
}

/**
 * Given an installed update whose new version will not start
 * When 1.1 s pass
 * Then the client stays, and says the update failed
 */
TEST_F(UpdateScreenFixture, ShowsTheFailureWhenTheNewVersionCannotStart) {
  run_.set({.phase = UpdatePhase::ReadyToRestart,
            .received = PACKAGE_SIZE,
            .size = PACKAGE_SIZE});
  run_.answerRestart(false);

  runFor(SHORT_OF_THE_RESTART + PAST_THE_MESSAGE);

  EXPECT_THAT(navigator_.calls(), IsEmpty());
  EXPECT_TRUE(wrote("UPDATE FAILED"));
}
