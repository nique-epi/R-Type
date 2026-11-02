#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <memory>
#include <optional>
#include "KeyPress.hpp"
#include "LeaderboardPage.hpp"
#include "LeaderboardPhase.hpp"
#include "LeaderboardPresentationFixtures.hpp"
#include "LeaderboardRequest.hpp"
#include "LeaderboardScreenFixture.hpp"
#include "LeaderboardScreenTestConstants.hpp"
#include "LeaderboardSpriteConstants.hpp"
#include "MenuScreenIdConstants.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "ScreenTestConstants.hpp"

using rtype::client::CONNECTION_SCREEN;
using rtype::client::LEADERBOARD_SHEET_ID;
using rtype::client::LeaderboardPage;
using rtype::client::LeaderboardPhase;
using rtype::client::testing::BANNER_FRAME;
using rtype::client::testing::FIRST_NAME;
using rtype::client::testing::LEFT_BANNER;
using rtype::client::testing::ON_THE_BACK_CHIP;
using rtype::client::testing::ON_THE_SECOND_TAB;
using rtype::client::testing::RIGHT_BANNER;
using rtype::network::LeaderboardRequest;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given a screen just built on NORMAL and two pilots
 * When its source is read
 * Then it was asked for that leaderboard, at once
 */
TEST_F(LeaderboardScreenFixture, ItAsksTheOpenTabAtOnce) {
  EXPECT_THAT(source().asked(), ElementsAre(LeaderboardRequest{
                                    .playerCount = 2, .difficulty = "normal"}));
}

/**
 * Given a source that answered
 * When the screen updates
 * Then the name of its first entry stands on the first row
 */
TEST_F(LeaderboardScreenFixture, AnAnswerShowsItsRows) {
  show(answeredPage({entryOf(1, "KIRA")}, entryOf(1, "KIRA")));

  EXPECT_EQ(anchorOf(canvas(), "KIRA"), FIRST_NAME);
}

/**
 * Given a source that answered, and a player without a score
 * When the screen updates
 * Then a note says so under the rows
 */
TEST_F(LeaderboardScreenFixture, APlayerWithoutAScoreIsTold) {
  show(answeredPage({entryOf(1, "KIRA")}, std::nullopt));

  EXPECT_NE(writingOf(canvas(), "YOU HAVE NO SCORE IN THIS MODE YET"), nullptr);
}

/**
 * Given a source still asking
 * When the screen updates
 * Then it says it is asking
 */
TEST_F(LeaderboardScreenFixture, AskingIsSaid) {
  show(LeaderboardPage{.phase = LeaderboardPhase::Asking});

  EXPECT_NE(writingOf(canvas(), "ASKING THE SERVER"), nullptr);
}

/**
 * Given a screen
 * When Escape is pressed and released
 * Then the screen on top is closed, once
 */
TEST_F(LeaderboardScreenFixture, EscapeClosesIt) {
  tap(sf::Keyboard::Key::Escape);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given a screen just opened with a key still held on the screen below
 * When Escape is released without its press
 * Then nothing is asked
 */
TEST_F(LeaderboardScreenFixture, TheReleaseOfAKeyPressedElsewhereDoesNothing) {
  screen().handleEvent(keyRelease(sf::Keyboard::Key::Escape));

  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given a screen
 * When BACK is clicked
 * Then the screen on top is closed
 */
TEST_F(LeaderboardScreenFixture, TheBackChipClosesIt) {
  screen().handleEvent(leftPress(ON_THE_BACK_CHIP));
  screen().handleEvent(leftRelease(ON_THE_BACK_CHIP));

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}

/**
 * Given a screen whose filter opened
 * When Escape is pressed and released
 * Then only the filter closes: the screen stays
 */
TEST_F(LeaderboardScreenFixture, EscapeInTheFilterClosesOnlyTheFilter) {
  tap(sf::Keyboard::Key::F);

  tap(sf::Keyboard::Key::Escape);
  screen().update(ONE_FRAME);

  EXPECT_THAT(navigator().calls(), IsEmpty());
  EXPECT_EQ(writingOf(canvas(), "DIFFICULTY"), nullptr);
}

/**
 * Given a screen
 * When F is pressed and released, and the screen repaints
 * Then the dialog of the filter shows, with its row of pilots
 */
TEST_F(LeaderboardScreenFixture, FOpensTheFilter) {
  tap(sf::Keyboard::Key::F);
  screen().update(ONE_FRAME);

  EXPECT_NE(writingOf(canvas(), "PILOTS"), nullptr);
}

/**
 * Given a screen
 * When the second tab is clicked
 * Then it says the master is out of reach, and F opens nothing
 */
TEST_F(LeaderboardScreenFixture, TheGlobalTabSaysWhyItIsEmpty) {
  screen().handleEvent(leftPress(ON_THE_SECOND_TAB));
  tap(sf::Keyboard::Key::F);
  screen().update(ONE_FRAME);

  EXPECT_NE(writingOf(canvas(), "THE MASTER IS OUT OF REACH"), nullptr);
  EXPECT_EQ(writingOf(canvas(), "PILOTS"), nullptr);
}

/**
 * Given a source with no session to ask through
 * When Enter is pressed and released
 * Then the connection screen replaces this one
 */
TEST_F(LeaderboardScreenFixture, WithoutAServerEnterJoinsOne) {
  show(LeaderboardPage{.phase = LeaderboardPhase::NotOnServer});

  tap(sf::Keyboard::Key::Enter);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt},
                          NavigatorCall{.request = NavigatorRequest::Push,
                                        .screen = CONNECTION_SCREEN}));
}

/**
 * Given a source that answered
 * When Enter is pressed and released
 * Then nothing is asked: the bar to join a server is gone
 */
TEST_F(LeaderboardScreenFixture, OnAServerEnterJoinsNothing) {
  show(answeredPage({entryOf(1, "KIRA")}, std::nullopt));

  tap(sf::Keyboard::Key::Enter);

  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given a screen just repainted
 * When a frame shorter than a frame of the banner goes by
 * Then it does not repaint
 */
TEST_F(LeaderboardScreenFixture, AShortFrameDoesNotRepaint) {
  const int before = canvas().displayCount;

  screen().update(BANNER_FRAME - ONE_TICK);

  EXPECT_EQ(canvas().displayCount, before);
}

/**
 * Given a screen just repainted
 * When a frame of the banner goes by
 * Then it repaints, with the next frame of the left banner
 */
TEST_F(LeaderboardScreenFixture, ABannerFrameRepaintsTheNextBanner) {
  const sf::IntRect first = canvas().images.front().source;

  screen().update(BANNER_FRAME);

  EXPECT_NE(canvas().images.front().source, first);
  EXPECT_EQ(canvas().images.front().assetId, LEADERBOARD_SHEET_ID);
  EXPECT_EQ(canvas().images.front().topLeft, LEFT_BANNER);
}

/**
 * Given a screen just repainted
 * When an event comes in, and a frame shorter than a banner goes by
 * Then it repaints: what the pointer changes shows at once
 */
TEST_F(LeaderboardScreenFixture, AnEventRepaintsOnTheNextFrame) {
  const int before = canvas().displayCount;
  screen().handleEvent(pointerMove(ON_THE_BACK_CHIP));

  screen().update(ONE_TICK);

  EXPECT_EQ(canvas().displayCount, before + 1);
}

/**
 * Given a screen
 * When its two banners are read
 * Then they show different frames, so they never move together
 */
TEST_F(LeaderboardScreenFixture, TheTwoBannersAreOutOfStep) {
  const auto& images = canvas().images;
  const auto right =
      std::ranges::find(images, RIGHT_BANNER, &RecordedImage::topLeft);

  ASSERT_NE(right, images.end());
  EXPECT_NE(right->source, images.front().source);
}

/**
 * Given a screen while the system asks for less motion
 * When a frame of the banner goes by
 * Then the left banner stays on its frame
 */
TEST_F(StillLeaderboardFixture, TheBannersStayStill) {
  const sf::IntRect first = canvas().images.front().source;

  screen().update(BANNER_FRAME);

  EXPECT_EQ(canvas().images.front().source, first);
}
