#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include "Ambience.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "ShopBoothFixture.hpp"
#include "ShopScreenTestConstants.hpp"
#include "SoundConstants.hpp"
#include "SpokenLineConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::Ambience;
using rtype::client::BOTTLES_CLINK_SOUND_ID;
using rtype::client::BYDO_VOICE_FIRST_SOUND_ID;
using rtype::client::SPOKEN_LINGER_TIME;
using rtype::client::testing::A_QUARTER_OF_HIS_BREATH;
using rtype::client::testing::BESIDE_THE_BOTTLES;
using rtype::client::testing::BESIDE_THE_RADIO;
using rtype::client::testing::BORDER_RADIO_WORD;
using rtype::client::testing::BOTTLES_MIDDLE;
using rtype::client::testing::DANCE_RADIO_WORD;
using rtype::client::testing::FAR_FROM_THE_RADIO;
using rtype::client::testing::FIRST_BOTTLES_WORD;
using rtype::client::testing::KEEPER_AT_HIS_LOWEST;
using rtype::client::testing::KEEPER_AT_REST;
using rtype::client::testing::RADIO_FRAME_CORNER;
using rtype::client::testing::RADIO_HINT_LINE;
using rtype::client::testing::RADIO_HINT_WORD;
using rtype::client::testing::RADIO_MIDDLE;
using rtype::client::testing::SECOND_BOTTLES_WORD;
using rtype::client::testing::SHOP_KEEPER_SHEET;
using rtype::client::testing::SHOP_MUSIC_WORD;
using rtype::client::testing::ShopBoothFixture;
using rtype::engine::Duration;
using Key = sf::Keyboard::Key;

namespace {

/** @returns Whether @p record holds the shopkeeper laid at @p topLeft. */
bool showsTheKeeperAt(const InterfaceCanvasRecord& record,
                      sf::Vector2i topLeft) {
  return std::ranges::any_of(
      record.images, [topLeft](const RecordedImage& image) noexcept {
        return image.assetId == SHOP_KEEPER_SHEET && image.topLeft == topLeft;
      });
}

}  // namespace

/**
 * Given the server map, which holds the space ambience
 * When the shop opens over it
 * Then the music of the shop plays
 */
TEST_F(ShopBoothFixture, OpensWithTheMusicOfTheShop) {
  EXPECT_EQ(ambience(), Ambience::Shop);
}

/**
 * Given the shop, its music playing
 * When the radio of the counter is clicked, on a window whose pixels are not
 * those of the interface
 * Then the radio plays its own music
 */
TEST_F(ShopBoothFixture, AClickOnTheRadioChangesTheMusic) {
  click(RADIO_MIDDLE);

  EXPECT_EQ(ambience(), Ambience::ShopRadio);
}

/**
 * Given the shop, its music playing
 * When the counter is clicked beside the radio
 * Then the music of the shop goes on
 */
TEST_F(ShopBoothFixture, AClickBesideTheRadioLeavesTheMusic) {
  click(BESIDE_THE_RADIO);

  EXPECT_EQ(ambience(), Ambience::Shop);
}

/**
 * Given the shop, its music playing
 * When R is pressed and released
 * Then the radio plays its own music
 */
TEST_F(ShopBoothFixture, RTunesTheRadio) {
  press(Key::R);

  EXPECT_EQ(ambience(), Ambience::ShopRadio);
}

/**
 * Given the radio playing the theme of Papers, Please
 * When R is pressed and released again
 * Then the radio plays Kavorka
 */
TEST_F(ShopBoothFixture, ASecondRTunesTheRadioToKavorka) {
  press(Key::R);

  press(Key::R);

  EXPECT_EQ(ambience(), Ambience::ShopDance);
}

/**
 * Given the radio playing Kavorka
 * When R is pressed and released again
 * Then the music of the shop comes back
 */
TEST_F(ShopBoothFixture, AThirdRBringsTheMusicOfTheShopBack) {
  press(Key::R);
  press(Key::R);

  press(Key::R);

  EXPECT_EQ(ambience(), Ambience::Shop);
}

/**
 * Given the shop, its music playing
 * When R is held, its press repeated before any release
 * Then the radio is tuned once: it plays its own music
 */
TEST_F(ShopBoothFixture, AHeldRTunesTheRadioOnce) {
  hold(Key::R);

  hold(Key::R);

  EXPECT_EQ(ambience(), Ambience::ShopRadio);
}

/**
 * Given the shop opened over the map, its radio tuned
 * When the shop closes
 * Then the map gets its space ambience back
 */
TEST_F(ShopBoothFixture, ClosingTheShopGivesTheMapItsSpaceBack) {
  press(Key::R);

  close();

  EXPECT_EQ(ambience(), Ambience::Space);
}

/**
 * Given a shop just opened
 * When it is painted
 * Then the shopkeeper stands at his place of rest
 */
TEST_F(ShopBoothFixture, TheKeeperStartsAtRest) {
  EXPECT_TRUE(showsTheKeeperAt(painting(), KEEPER_AT_REST));
}

/**
 * Given a shop just opened
 * When a quarter of a breath of the shopkeeper passes
 * Then he is drawn 2 pixels lower: he breathes
 */
TEST_F(ShopBoothFixture, TheKeeperSinksAsHeBreathes) {
  wait(A_QUARTER_OF_HIS_BREATH);

  EXPECT_TRUE(showsTheKeeperAt(painting(), KEEPER_AT_HIS_LOWEST));
}

/**
 * Given a shop opened for a player who asked for less motion
 * When a quarter of a breath of the shopkeeper passes
 * Then he still stands at his place of rest
 */
TEST_F(ShopBoothFixture, WithLessMotionTheKeeperStandsStill) {
  reopenWithLessMotion();

  wait(A_QUARTER_OF_HIS_BREATH);

  EXPECT_TRUE(showsTheKeeperAt(painting(), KEEPER_AT_REST));
}

/**
 * Given the shop, on a window whose pixels are not those of the interface
 * When the pointer moves over the radio of the counter
 * Then the frame of a choice shows around it, and the status line says a
 * click changes the music
 */
TEST_F(ShopBoothFixture, TheRadioUnderThePointerShowsItCanBePressed) {
  pointAt(RADIO_MIDDLE);

  EXPECT_TRUE(litPixelsOf(painting()).contains(RADIO_FRAME_CORNER));
  EXPECT_NE(writingOf(painting(), RADIO_HINT_LINE), nullptr);
}

/**
 * Given the pointer over the radio, its frame showing
 * When the pointer moves away
 * Then the frame and the line of the radio go
 */
TEST_F(ShopBoothFixture, TheFrameOfTheRadioGoesWithThePointer) {
  pointAt(RADIO_MIDDLE);

  pointAt(FAR_FROM_THE_RADIO);

  EXPECT_FALSE(litPixelsOf(painting()).contains(RADIO_FRAME_CORNER));
  EXPECT_EQ(writingOf(painting(), RADIO_HINT_LINE), nullptr);
}

/**
 * Given a shop opened over the server map
 * When it shows
 * Then the shopkeeper speaks at once, with the first sound of his voice
 */
TEST_F(ShopBoothFixture, TheShopkeeperGreetsThePlayer) {
  EXPECT_NE(std::ranges::find(played(), BYDO_VOICE_FIRST_SOUND_ID),
            played().end());
}

/**
 * Given a shop that shows lines whole, its greeting said
 * When the greeting lingered its time
 * Then the shopkeeper says where his radio is
 */
TEST_F(ShopBoothFixture, TheRadioHintFollowsTheGreeting) {
  reopenWithLessMotion();
  wait(Duration::zero());
  ASSERT_EQ(writingOf(painting(), RADIO_HINT_WORD), nullptr);

  wait(SPOKEN_LINGER_TIME);

  EXPECT_NE(writingOf(painting(), RADIO_HINT_WORD), nullptr);
}

/**
 * Given a shop that shows lines whole
 * When the bottles are clicked, then clicked again
 * Then the shopkeeper says his first line on them, then his second
 */
TEST_F(ShopBoothFixture, AClickOnTheBottlesHasTheShopkeeperGuardThem) {
  reopenWithLessMotion();

  click(BOTTLES_MIDDLE);
  wait(Duration::zero());
  EXPECT_NE(writingOf(painting(), FIRST_BOTTLES_WORD), nullptr);

  click(BOTTLES_MIDDLE);
  wait(Duration::zero());
  EXPECT_NE(writingOf(painting(), SECOND_BOTTLES_WORD), nullptr);
}

/**
 * Given a shop that shows lines whole
 * When the wood just left of the bottles is clicked
 * Then the shopkeeper says nothing of them
 */
TEST_F(ShopBoothFixture, AClickBesideTheBottlesLeavesThemUnguarded) {
  reopenWithLessMotion();

  click(BESIDE_THE_BOTTLES);
  wait(Duration::zero());

  EXPECT_EQ(writingOf(painting(), FIRST_BOTTLES_WORD), nullptr);
}

/**
 * Given a shop that shows lines whole, its greeting said
 * When the bottles are clicked, then the line lingers its time
 * Then the radio hint, which was to follow the greeting, never comes
 */
TEST_F(ShopBoothFixture, ANewLineCutsTheRadioHint) {
  reopenWithLessMotion();
  click(BOTTLES_MIDDLE);

  wait(SPOKEN_LINGER_TIME);

  EXPECT_EQ(writingOf(painting(), RADIO_HINT_WORD), nullptr);
}

/**
 * Given a shop that shows lines whole
 * When R tunes the radio to Papers, Please, to Kavorka, then back
 * Then the shopkeeper speaks of Arstotzka, of disco, then of Moscow
 */
TEST_F(ShopBoothFixture, TheRadioTunedHasTheShopkeeperSpeakOfIt) {
  reopenWithLessMotion();

  press(Key::R);
  wait(Duration::zero());
  EXPECT_NE(writingOf(painting(), BORDER_RADIO_WORD), nullptr);

  press(Key::R);
  wait(Duration::zero());
  EXPECT_NE(writingOf(painting(), DANCE_RADIO_WORD), nullptr);

  press(Key::R);
  wait(Duration::zero());
  EXPECT_NE(writingOf(painting(), SHOP_MUSIC_WORD), nullptr);
}

/**
 * Given a shop
 * When the bottles are clicked, then the wood just left of them
 * Then the bottles clink once, for the click on them only
 */
TEST_F(ShopBoothFixture, AClickOnTheBottlesMakesThemClink) {
  click(BOTTLES_MIDDLE);
  click(BESIDE_THE_BOTTLES);

  EXPECT_EQ(std::ranges::count(played(), BOTTLES_CLINK_SOUND_ID), 1);
}
