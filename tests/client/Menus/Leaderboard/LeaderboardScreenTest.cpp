#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <optional>
#include "InterfaceConstants.hpp"
#include "KeyPress.hpp"
#include "LeaderboardFrames.hpp"
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
#include "RecordingServerConnection.hpp"
#include "ScreenTestConstants.hpp"
#include "ServerConnectionRequest.hpp"
#include "WidgetConstants.hpp"

using rtype::client::CONNECTION_FOR_LEADERBOARD_SCREEN;
using rtype::client::INTERFACE_AREA;
using rtype::client::LEADERBOARD_SHEET_ID;
using rtype::client::leaderboardFrameOf;
using rtype::client::LeaderboardPhase;
using rtype::client::PRESS_DURATION;
using rtype::client::testing::BANNER_FRAME;
using rtype::client::testing::CENTERED_ASKING_ANCHOR;
using rtype::client::testing::FIRST_NAME;
using rtype::client::testing::LARGEST_TEXT;
using rtype::client::testing::LEFT_BANNER;
using rtype::client::testing::LONGEST_NICKNAME;
using rtype::client::testing::NEUTRAL_ASKING_TEXT;
using rtype::client::testing::ON_THE_BACK_CHIP;
using rtype::client::testing::ON_THE_SECOND_TAB;
using rtype::client::testing::RIGHT_BANNER;
using rtype::network::LeaderboardRequest;
using ::testing::ElementsAre;
using ::testing::Field;
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
 * Given a screen on NORMAL and two pilots, its filter open
 * When the player steps the difficulty, reaches APPLY and presses it
 * Then the source is asked for the leaderboard the player picked
 */
TEST_F(LeaderboardScreenFixture, ApplyingAFilterAsksAgain) {
  tap(sf::Keyboard::Key::F);
  tap(sf::Keyboard::Key::Right);
  tap(sf::Keyboard::Key::Down);
  tap(sf::Keyboard::Key::Down);
  tap(sf::Keyboard::Key::Down);

  tap(sf::Keyboard::Key::Enter);
  screen().update(PRESS_DURATION);

  EXPECT_THAT(
      source().asked(),
      ElementsAre(LeaderboardRequest{.playerCount = 2, .difficulty = "normal"},
                  LeaderboardRequest{.playerCount = 2, .difficulty = "hard"}));
}

/**
 * Given a screen on NORMAL and two pilots, its filter open
 * When the player reaches APPLY without changing a value and presses it
 * Then the source was asked nothing more, so the list shown stays
 */
TEST_F(LeaderboardScreenFixture, ApplyingTheSameFilterAsksNothing) {
  tap(sf::Keyboard::Key::F);
  tap(sf::Keyboard::Key::Down);
  tap(sf::Keyboard::Key::Down);
  tap(sf::Keyboard::Key::Down);

  tap(sf::Keyboard::Key::Enter);
  screen().update(PRESS_DURATION);

  EXPECT_EQ(source().asked().size(), 1);
}

/**
 * Given a screen whose filter opened, and a value stepped
 * When Escape closes the filter
 * Then the source was asked nothing more
 */
TEST_F(LeaderboardScreenFixture, ClosingTheFilterAsksNothing) {
  tap(sf::Keyboard::Key::F);
  tap(sf::Keyboard::Key::Right);

  tap(sf::Keyboard::Key::Escape);
  screen().update(ONE_FRAME);

  EXPECT_EQ(source().asked().size(), 1);
}

/**
 * Given a screen that went to the tab without a source
 * When the player comes back to the first tab
 * Then its source is asked again
 */
TEST_F(LeaderboardScreenFixture, ComingBackToATabAsksAgain) {
  screen().handleEvent(keyPress(sf::Keyboard::Key::E, sf::Keyboard::Scan::E));
  screen().handleEvent(keyRelease(sf::Keyboard::Key::E, sf::Keyboard::Scan::E));

  screen().handleEvent(keyPress(sf::Keyboard::Key::Q, sf::Keyboard::Scan::Q));

  EXPECT_EQ(source().asked().size(), 2);
}

/**
 * Given a screen reading its source
 * When the screen is destroyed
 * Then the source stops asking: no request outlives the screen
 */
TEST_F(LeaderboardScreenFixture, LeavingStopsAsking) {
  closeScreen();

  EXPECT_EQ(source().stopCount(), 1);
}

/**
 * Given a screen that reads its source
 * When the player goes to the other tab
 * Then the source stops asking: nobody shows what it would answer
 */
TEST_F(LeaderboardScreenFixture, LeavingATabStopsItsRequests) {
  screen().handleEvent(keyPress(sf::Keyboard::Key::E, sf::Keyboard::Scan::E));

  EXPECT_EQ(source().stopCount(), 1);
}

/**
 * Given the key of the next tab held down, whose press the system repeats
 * When its press comes again
 * Then the tab changed once: the other tab stays open, and nothing is asked
 */
TEST_F(LeaderboardScreenFixture, AHeldTabKeyChangesTabOnce) {
  screen().handleEvent(keyPress(sf::Keyboard::Key::E, sf::Keyboard::Scan::E));

  screen().handleEvent(keyPress(sf::Keyboard::Key::E, sf::Keyboard::Scan::E));
  screen().update(ONE_FRAME);

  EXPECT_EQ(source().asked().size(), 1);
  EXPECT_NE(writingOf(canvas(), "THE MASTER IS OUT OF REACH"), nullptr);
}

/**
 * Given the key of the next tab pressed and released
 * When it is pressed again
 * Then the tab changes again
 */
TEST_F(LeaderboardScreenFixture, AReleasedTabKeyChangesTabAgain) {
  screen().handleEvent(keyPress(sf::Keyboard::Key::E, sf::Keyboard::Scan::E));
  screen().handleEvent(keyRelease(sf::Keyboard::Key::E, sf::Keyboard::Scan::E));

  screen().handleEvent(keyPress(sf::Keyboard::Key::E, sf::Keyboard::Scan::E));

  EXPECT_EQ(source().asked().size(), 2);
}

/**
 * Given Escape held down when F opens the filter, its release going to the
 * filter
 * When the filter is closed, and Escape is released on the screen
 * Then the screen stays: BACK forgot the press when the filter took the events
 */
TEST_F(LeaderboardScreenFixture, TheFilterDropsWhatTheChipsHeld) {
  screen().handleEvent(keyPress(sf::Keyboard::Key::Escape));
  tap(sf::Keyboard::Key::F);
  tap(sf::Keyboard::Key::Escape);

  screen().handleEvent(keyRelease(sf::Keyboard::Key::Escape));

  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given a source with no session to ask through
 * When the screen updates
 * Then it offers to join a server
 */
TEST_F(LeaderboardScreenFixture, WithoutAServerTheBarToJoinShows) {
  show(pageIn(LeaderboardPhase::NotOnServer));

  EXPECT_NE(writingOf(canvas(), "NOT ON A SERVER"), nullptr);
  EXPECT_NE(writingOf(canvas(), "JOIN A SERVER"), nullptr);
}

/**
 * Given a source that answered
 * When the screen updates
 * Then the bar to join a server is not painted
 */
TEST_F(LeaderboardScreenFixture, OnAServerTheBarToJoinIsGone) {
  show(answeredPage({entryOf(1, "KIRA")}, std::nullopt));

  EXPECT_EQ(writingOf(canvas(), "JOIN A SERVER"), nullptr);
}

/**
 * Given a source with no session to ask through
 * When F is pressed and released, and the screen repaints
 * Then the filter does not open
 */
TEST_F(LeaderboardScreenFixture, WithoutAServerFOpensNothing) {
  show(pageIn(LeaderboardPhase::NotOnServer));

  tap(sf::Keyboard::Key::F);
  screen().update(ONE_FRAME);

  EXPECT_EQ(writingOf(canvas(), "PILOTS"), nullptr);
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
 * Then it says it is asking, naming neither a server nor the master: the
 * GLOBAL tab asks the master
 */
TEST_F(LeaderboardScreenFixture, AskingIsSaid) {
  show(pageIn(LeaderboardPhase::Asking));

  EXPECT_NE(writingOf(canvas(), NEUTRAL_ASKING_TEXT), nullptr);
}

/**
 * Given a source still asking
 * When the screen updates
 * Then the line starts where it and its three dots are centered on the panel
 */
TEST_F(LeaderboardScreenFixture, TheAskingLineIsCenteredWithItsDots) {
  show(pageIn(LeaderboardPhase::Asking));

  const RecordedWriting* asking = writingOf(canvas(), NEUTRAL_ASKING_TEXT);
  ASSERT_NE(asking, nullptr);
  EXPECT_EQ(asking->anchor, CENTERED_ASKING_ANCHOR);
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
 * Then the connection screen opens over this one, which it comes back to
 */
TEST_F(LeaderboardScreenFixture, WithoutAServerEnterOpensTheConnectionOverIt) {
  show(pageIn(LeaderboardPhase::NotOnServer));

  tap(sf::Keyboard::Key::Enter);

  EXPECT_THAT(
      navigator().calls(),
      ElementsAre(NavigatorCall{.request = NavigatorRequest::Push,
                                .screen = CONNECTION_FOR_LEADERBOARD_SCREEN}));
}

/**
 * Given a server joined from this screen, whose scores it now shows
 * When Escape is pressed and released
 * Then the session ends with the screen: nothing on the title holds it
 */
TEST_F(LeaderboardScreenFixture, BackLeavesAServerJoinedFromHere) {
  show(pageIn(LeaderboardPhase::NotOnServer));
  tap(sf::Keyboard::Key::Enter);
  show(answeredPage({entryOf(1, "KIRA")}, std::nullopt));

  tap(sf::Keyboard::Key::Escape);

  EXPECT_THAT(connection().calls(),
              ElementsAre(Field(&ServerConnectionCall::request,
                                ServerConnectionRequest::Cancel)));
  EXPECT_THAT(navigator().calls().back(),
              (NavigatorCall{.request = NavigatorRequest::Pop,
                             .screen = std::nullopt}));
}

/**
 * Given a server joined elsewhere, from the waiting room
 * When Escape is pressed and released
 * Then the player stays on the server
 */
TEST_F(LeaderboardScreenFixture, BackStaysOnAServerJoinedElsewhere) {
  show(answeredPage({entryOf(1, "KIRA")}, std::nullopt));

  tap(sf::Keyboard::Key::Escape);

  EXPECT_THAT(connection().calls(), IsEmpty());
}

/**
 * Given Escape held down when Enter opens the connection screen, its release
 * going to that screen
 * When Escape is released on this one, once back
 * Then the screen stays: BACK forgot the press when the events went elsewhere
 */
TEST_F(LeaderboardScreenFixture, JoiningDropsWhatTheChipsHeld) {
  show(pageIn(LeaderboardPhase::NotOnServer));
  screen().handleEvent(keyPress(sf::Keyboard::Key::Escape));
  tap(sf::Keyboard::Key::Enter);

  screen().handleEvent(keyRelease(sf::Keyboard::Key::Escape));

  EXPECT_EQ(navigator().calls().size(), 1);
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

/**
 * Given the leaderboard at the largest text size, ten pilots of the longest
 * nickname and the player's own row
 * When the screen shows them
 * Then every text it writes lies on screen
 */
TEST_F(LargestLeaderboardFixture, AtTheLargestSizeEveryTextStaysOnScreen) {
  show(answeredPage(
      {entryOf(1, LONGEST_NICKNAME), entryOf(2, LONGEST_NICKNAME),
       entryOf(3, LONGEST_NICKNAME), entryOf(4, LONGEST_NICKNAME),
       entryOf(5, LONGEST_NICKNAME), entryOf(6, LONGEST_NICKNAME),
       entryOf(7, LONGEST_NICKNAME), entryOf(8, LONGEST_NICKNAME),
       entryOf(9, LONGEST_NICKNAME), entryOf(10, LONGEST_NICKNAME)},
      entryOf(3, LONGEST_NICKNAME)));

  for (const RecordedWriting& writing : canvas().writings) {
    SCOPED_TRACE(writing.text);
    const sf::IntRect box = writtenBoxOf(canvas(), writing);
    EXPECT_EQ(box.findIntersection(INTERFACE_AREA), std::optional(box));
  }
}

/**
 * Given the same leaderboard
 * When the screen shows the ten pilots
 * Then each nickname it writes lies inside the panel
 */
TEST_F(LargestLeaderboardFixture, AtTheLargestSizeTheNamesStayInThePanel) {
  show(
      answeredPage({entryOf(1, LONGEST_NICKNAME), entryOf(2, LONGEST_NICKNAME),
                    entryOf(3, LONGEST_NICKNAME), entryOf(4, LONGEST_NICKNAME)},
                   entryOf(3, LONGEST_NICKNAME)));
  const sf::IntRect panel = leaderboardFrameOf(LARGEST_TEXT).panel;

  for (const RecordedWriting& writing : canvas().writings) {
    if (writing.text == LONGEST_NICKNAME) {
      const sf::IntRect box = writtenBoxOf(canvas(), writing);
      EXPECT_EQ(box.findIntersection(panel), std::optional(box));
    }
  }
}

/**
 * Given the leaderboard at the largest size, KIRA first of four pilots
 * When Down is pressed
 * Then the list scrolls by one: KIRA is no longer written
 */
TEST_F(LargestLeaderboardFixture, AtTheLargestSizeDownScrollsTheList) {
  show(answeredPage({entryOf(1, "KIRA"), entryOf(2, "ACE"), entryOf(3, "ACE"),
                     entryOf(4, "ACE")},
                    std::nullopt));

  tap(sf::Keyboard::Key::Down);
  screen().update(ONE_FRAME);

  EXPECT_EQ(writingOf(canvas(), "KIRA"), nullptr);
}
