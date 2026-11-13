#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include "Ambience.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "ShopBoothFixture.hpp"
#include "ShopScreenTestConstants.hpp"

using rtype::client::Ambience;
using rtype::client::testing::A_QUARTER_OF_HIS_BREATH;
using rtype::client::testing::BESIDE_THE_RADIO;
using rtype::client::testing::FAR_FROM_THE_RADIO;
using rtype::client::testing::KEEPER_AT_HIS_LOWEST;
using rtype::client::testing::KEEPER_AT_REST;
using rtype::client::testing::RADIO_FRAME_CORNER;
using rtype::client::testing::RADIO_HINT_LINE;
using rtype::client::testing::RADIO_MIDDLE;
using rtype::client::testing::SHOP_KEEPER_SHEET;
using rtype::client::testing::ShopBoothFixture;
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
 * Given the radio playing its own music
 * When R is pressed and released again
 * Then the music of the shop comes back
 */
TEST_F(ShopBoothFixture, ASecondRBringsTheMusicOfTheShopBack) {
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
