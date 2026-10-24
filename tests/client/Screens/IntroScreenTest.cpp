#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "EffectTestConstants.hpp"
#include "IntroScreenFixture.hpp"
#include "IntroScreenTestConstants.hpp"
#include "KeyPress.hpp"
#include "RecordingNavigator.hpp"
#include "ScreenId.hpp"
#include "ScreenTestConstants.hpp"
#include "SoundConstants.hpp"

using rtype::client::BYDO_GROWL_SOUND_ID;
using rtype::client::ENTRY_WHOOSH_SOUND_ID;
using rtype::client::LOGO_IMPACT_SOUND_ID;
using rtype::client::ScreenId;
using rtype::client::testing::AFTER_THE_SKIPPED_LANDING;
using rtype::client::testing::BEFORE_THE_SKIPPED_LANDING;
using rtype::client::testing::FRAMES_AFTER_THE_RELEASE;
using rtype::client::testing::JUST_AFTER_THE_TITLE;
using rtype::client::testing::JUST_BEFORE_THE_TITLE;
using rtype::client::testing::MUSIC_AFTER_THE_ENTRY;
using rtype::client::testing::MUSIC_AFTER_THE_RELEASE;
using rtype::client::testing::MUSIC_IN_THE_WARP;
using rtype::client::testing::PAST_THE_BLACK_SECOND;
using rtype::client::testing::PRESS_IN_THE_MUSIC;
using rtype::client::testing::WITHIN_THE_BLACK_SECOND;
using ::testing::Contains;
using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::testing::Not;

/**
 * Given an intro just built
 * When most of its black second passes
 * Then no sound plays
 */
TEST_F(IntroScreenFixture, TheBlackSecondIsSilent) {
  start();

  playWithTheMusicFor(WITHIN_THE_BLACK_SECOND);

  EXPECT_THAT(sounds().played(), IsEmpty());
}

/**
 * Given an intro just built
 * When most of its black second passes
 * Then the music has not started yet
 */
TEST_F(IntroScreenFixture, TheMusicWaitsForTheEndOfTheBlackSecond) {
  start();

  playWithTheMusicFor(WITHIN_THE_BLACK_SECOND);

  EXPECT_FALSE(music().playingOffset().has_value());
}

/**
 * Given an intro just built
 * When its black second is over
 * Then it started the music
 */
TEST_F(IntroScreenFixture, TheIntroStartsTheMusicAfterItsBlackSecond) {
  start();

  playWithTheMusicFor(PAST_THE_BLACK_SECOND);

  EXPECT_TRUE(music().playingOffset().has_value());
}

/**
 * Given an intro just built
 * When its black second is over
 * Then the growl of the Bydo in the dark plays, and nothing else yet
 */
TEST_F(IntroScreenFixture, TheIntroOpensOnTheGrowlOfTheBydo) {
  start();

  playWithTheMusicFor(PAST_THE_BLACK_SECOND);

  EXPECT_THAT(sounds().played(), ElementsAre(BYDO_GROWL_SOUND_ID));
}

/**
 * Given an intro on the first frame of its black second
 * When a key is pressed
 * Then it asks at once for the title, whose squadron flies in
 */
TEST_F(IntroScreenFixture, APressDuringTheBlackSecondOpensTheTitle) {
  start();
  intro().update(ONE_FRAME);

  intro().handleEvent(keyPress(sf::Keyboard::Key::Space));

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::ReplaceAll,
                                        .screen = ScreenId::Title}));
}

/**
 * Given an intro without a music file
 * When a little less than a black second and ten beats pass
 * Then it asks for nothing yet
 */
TEST_F(IntroScreenFixture, TheIntroPlaysItsTenBeats) {
  music().removeTheFile();
  start();

  playFor(JUST_BEFORE_THE_TITLE);

  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given an intro without a music file
 * When a little more than a black second and ten beats pass
 * Then it asks once for the title, whose squadron starts in place
 */
TEST_F(IntroScreenFixture, TheIntroGivesWayToTheTitleAfterTenBeats) {
  music().removeTheFile();
  start();

  playFor(JUST_AFTER_THE_TITLE);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::ReplaceAll,
                                        .screen = ScreenId::TitleAfterIntro}));
}

/**
 * Given an intro that starts its music
 * When a little more than a black second and ten beats pass, the music with
 * them
 * Then it asks once for the title, whose squadron starts in place
 */
TEST_F(IntroScreenFixture, WithItsMusicTheIntroGivesWayAfterTenBeats) {
  start();

  playWithTheMusicFor(JUST_AFTER_THE_TITLE);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::ReplaceAll,
                                        .screen = ScreenId::TitleAfterIntro}));
}

/**
 * Given an intro past its black second
 * When a key is pressed
 * Then it does not open the title yet: it jumped to the slam of the logo
 */
TEST_F(IntroScreenFixture, AFirstPressJumpsToTheSlam) {
  start();
  playWithTheMusicFor(PAST_THE_BLACK_SECOND);

  intro().handleEvent(keyPress(sf::Keyboard::Key::Space));

  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given an intro skipped to the slam of the logo
 * When a key is pressed again
 * Then it asks for the title at once, whose squadron starts in place
 */
TEST_F(IntroScreenFixture, ASecondPressOpensTheTitle) {
  start();
  playWithTheMusicFor(PAST_THE_BLACK_SECOND);
  intro().handleEvent(keyPress(sf::Keyboard::Key::Space));

  intro().handleEvent(keyPress(sf::Keyboard::Key::Space));

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::ReplaceAll,
                                        .screen = ScreenId::TitleAfterIntro}));
}

/**
 * Given an intro that already asked for the title, on a second press
 * When a key is pressed once more, before it is replaced
 * Then it asks for nothing more: the title is asked for once
 */
TEST_F(IntroScreenFixture, ThePressesAfterTheTitleAskNothingMore) {
  start();
  playWithTheMusicFor(PAST_THE_BLACK_SECOND);
  intro().handleEvent(keyPress(sf::Keyboard::Key::Space));
  intro().handleEvent(keyPress(sf::Keyboard::Key::Space));

  intro().handleEvent(keyPress(sf::Keyboard::Key::Space));

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::ReplaceAll,
                                        .screen = ScreenId::TitleAfterIntro}));
}

/**
 * Given an intro skipped just after its growl
 * When the next frame passes
 * Then none of the sounds jumped over plays: only the growl was heard
 */
TEST_F(IntroScreenFixture, SkippingLeavesTheSkippedSoundsUnplayed) {
  start();
  playWithTheMusicFor(PAST_THE_BLACK_SECOND);
  intro().handleEvent(keyPress(sf::Keyboard::Key::Space));

  playWithTheMusicFor(ONE_FRAME);

  EXPECT_THAT(sounds().played(), ElementsAre(BYDO_GROWL_SOUND_ID));
}

/**
 * Given an intro past its black second, and its music moved to 3 s
 * When a frame passes
 * Then it follows the music: the whoosh of the squadron, at 2.75 s, plays
 */
TEST_F(IntroScreenFixture, TheIntroFollowsTheMusic) {
  start();
  playWithTheMusicFor(PAST_THE_BLACK_SECOND);
  music().setPlayingOffset(MUSIC_AFTER_THE_ENTRY);

  intro().update(ONE_FRAME);

  EXPECT_THAT(sounds().played(), Contains(ENTRY_WHOOSH_SOUND_ID));
}

/**
 * Given an intro past its black second, and its music in the full warp of
 * the boost
 * When a frame passes
 * Then the stars move twelve times their speed
 */
TEST_F(IntroScreenFixture, TheStarsWarpAtTheBoost) {
  start();
  playWithTheMusicFor(PAST_THE_BLACK_SECOND);
  music().setPlayingOffset(MUSIC_IN_THE_WARP);

  intro().update(ONE_FRAME);

  EXPECT_FLOAT_EQ(sky().lastSpeedFactor(), TOP_WARP_SPEED);
}

/**
 * Given an intro in the warp of the boost
 * When it goes
 * Then the sky is back to its speed and its place
 */
TEST_F(IntroScreenFixture, TheSkyIsPutBackWhenTheIntroGoes) {
  start();
  playWithTheMusicFor(PAST_THE_BLACK_SECOND);
  music().setPlayingOffset(MUSIC_IN_THE_WARP);
  intro().update(ONE_FRAME);

  close();

  EXPECT_FLOAT_EQ(sky().lastSpeedFactor(), OWN_SPEED);
  EXPECT_EQ(sky().lastOffset(), sf::Vector2i());
}

/**
 * Given music 1.23 s in, between its second and third beats, and a key
 * pressed then
 * When the intro and the music play on for 0.70 s
 * Then the logo has not landed yet: it waits for the third beat
 */
TEST_F(IntroScreenFixture, ASkippedLogoWaitsForTheNextBeat) {
  start();
  playWithTheMusicFor(PAST_THE_BLACK_SECOND);
  music().setPlayingOffset(PRESS_IN_THE_MUSIC);
  intro().update(ONE_FRAME);
  intro().handleEvent(keyPress(sf::Keyboard::Key::Space));

  playWithTheMusicFor(BEFORE_THE_SKIPPED_LANDING);

  EXPECT_THAT(sounds().played(), Not(Contains(LOGO_IMPACT_SOUND_ID)));
}

/**
 * Given music 1.23 s in, and a key pressed then
 * When the intro and the music play on for 0.74 s
 * Then the logo landed with its impact, 0.3 s after the third beat
 */
TEST_F(IntroScreenFixture, ASkippedLogoLandsOnTheNextBeat) {
  start();
  playWithTheMusicFor(PAST_THE_BLACK_SECOND);
  music().setPlayingOffset(PRESS_IN_THE_MUSIC);
  intro().update(ONE_FRAME);
  intro().handleEvent(keyPress(sf::Keyboard::Key::Space));

  playWithTheMusicFor(AFTER_THE_SKIPPED_LANDING);

  EXPECT_THAT(sounds().played(), Contains(LOGO_IMPACT_SOUND_ID));
}

/**
 * Given flashes, and music just after the release
 * When ten frames pass, the music with them
 * Then the sky shakes on at least one of them
 */
TEST_F(IntroScreenFixture, TheReleaseShakesTheSky) {
  start();
  playWithTheMusicFor(PAST_THE_BLACK_SECOND);
  music().setPlayingOffset(MUSIC_AFTER_THE_RELEASE);

  bool shook = false;
  for (int frame = 0; frame < FRAMES_AFTER_THE_RELEASE; ++frame) {
    playWithTheMusicFor(ONE_FRAME);
    shook = shook || sky().lastOffset() != sf::Vector2i();
  }

  EXPECT_TRUE(shook);
}

/**
 * Given REDUCE FLASHES, and music just after the release
 * When ten frames pass, the music with them
 * Then the sky never shakes
 */
TEST_F(IntroScreenFixture, WithReducedFlashesTheSkyNeverShakes) {
  settings().reduceFlashes = true;
  start();
  playWithTheMusicFor(PAST_THE_BLACK_SECOND);
  music().setPlayingOffset(MUSIC_AFTER_THE_RELEASE);

  bool shook = false;
  for (int frame = 0; frame < FRAMES_AFTER_THE_RELEASE; ++frame) {
    playWithTheMusicFor(ONE_FRAME);
    shook = shook || sky().lastOffset() != sf::Vector2i();
  }

  EXPECT_FALSE(shook);
}
