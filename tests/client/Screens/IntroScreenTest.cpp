#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
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
using rtype::client::testing::OWN_SPEED;
using rtype::client::testing::PRESS_IN_THE_MUSIC;
using rtype::client::testing::WARP;
using ::testing::Contains;
using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::testing::Not;

/**
 * Given an intro just built
 * When its first frame passes
 * Then the growl of the Bydo in the dark plays
 */
TEST_F(IntroScreenFixture, TheIntroOpensOnTheGrowlOfTheBydo) {
  start();

  intro().update(ONE_FRAME);

  EXPECT_THAT(sounds().played(), ElementsAre(BYDO_GROWL_SOUND_ID));
}

/**
 * Given an intro without music
 * When a little less than ten beats pass
 * Then it asks for nothing yet
 */
TEST_F(IntroScreenFixture, TheIntroPlaysItsTenBeats) {
  start();

  playFor(JUST_BEFORE_THE_TITLE);

  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given an intro without music
 * When a little more than ten beats pass
 * Then it asks for the title in its place, once
 */
TEST_F(IntroScreenFixture, TheIntroGivesWayToTheTitleAfterTenBeats) {
  start();

  playFor(JUST_AFTER_THE_TITLE);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::ReplaceAll,
                                        .screen = ScreenId::Title}));
}

/**
 * Given an intro on its first frame
 * When a key is pressed
 * Then it asks for nothing: it jumped to the slam of the logo
 */
TEST_F(IntroScreenFixture, AFirstPressJumpsToTheSlam) {
  start();
  intro().update(ONE_FRAME);

  intro().handleEvent(keyPress(sf::Keyboard::Key::Space));

  EXPECT_THAT(navigator().calls(), IsEmpty());
}

/**
 * Given an intro skipped to the slam of the logo
 * When a key is pressed again
 * Then it asks for the title at once
 */
TEST_F(IntroScreenFixture, ASecondPressOpensTheTitle) {
  start();
  intro().update(ONE_FRAME);
  intro().handleEvent(keyPress(sf::Keyboard::Key::Space));

  intro().handleEvent(keyPress(sf::Keyboard::Key::Space));

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::ReplaceAll,
                                        .screen = ScreenId::Title}));
}

/**
 * Given an intro skipped on its first frame
 * When the next frame passes
 * Then none of the sounds jumped over plays: only the growl was heard
 */
TEST_F(IntroScreenFixture, SkippingLeavesTheSkippedSoundsUnplayed) {
  start();
  intro().update(ONE_FRAME);
  intro().handleEvent(keyPress(sf::Keyboard::Key::Space));

  intro().update(ONE_FRAME);

  EXPECT_THAT(sounds().played(), ElementsAre(BYDO_GROWL_SOUND_ID));
}

/**
 * Given music already 3 s in when the intro starts
 * When its first frame passes
 * Then it follows the music: the whoosh of the squadron, at 2.75 s, plays
 */
TEST_F(IntroScreenFixture, TheIntroFollowsTheMusic) {
  music().setPlayingOffset(MUSIC_AFTER_THE_ENTRY);
  start();

  intro().update(ONE_FRAME);

  EXPECT_THAT(sounds().played(), Contains(ENTRY_WHOOSH_SOUND_ID));
}

/**
 * Given music in the full warp of the boost
 * When a frame of the intro passes
 * Then the stars move twelve times their speed
 */
TEST_F(IntroScreenFixture, TheStarsWarpAtTheBoost) {
  music().setPlayingOffset(MUSIC_IN_THE_WARP);
  start();

  intro().update(ONE_FRAME);

  EXPECT_FLOAT_EQ(sky().lastSpeedFactor(), WARP);
}

/**
 * Given an intro in the warp of the boost
 * When it goes
 * Then the sky is back to its speed and its place
 */
TEST_F(IntroScreenFixture, TheSkyIsPutBackWhenTheIntroGoes) {
  music().setPlayingOffset(MUSIC_IN_THE_WARP);
  start();
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
  music().setPlayingOffset(PRESS_IN_THE_MUSIC);
  start();
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
  music().setPlayingOffset(PRESS_IN_THE_MUSIC);
  start();
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
  music().setPlayingOffset(MUSIC_AFTER_THE_RELEASE);
  start();

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
  music().setPlayingOffset(MUSIC_AFTER_THE_RELEASE);
  start();

  bool shook = false;
  for (int frame = 0; frame < FRAMES_AFTER_THE_RELEASE; ++frame) {
    playWithTheMusicFor(ONE_FRAME);
    shook = shook || sky().lastOffset() != sf::Vector2i();
  }

  EXPECT_FALSE(shook);
}
