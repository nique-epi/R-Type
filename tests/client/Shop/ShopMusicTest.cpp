#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include "Ambience.hpp"
#include "ShopMusicFixture.hpp"
#include "ShopScreenTestConstants.hpp"

using rtype::client::Ambience;
using rtype::client::testing::BESIDE_THE_RADIO;
using rtype::client::testing::RADIO_MIDDLE;
using rtype::client::testing::ShopMusicFixture;
using Key = sf::Keyboard::Key;

/**
 * Given the server map, which holds the space ambience
 * When the shop opens over it
 * Then the music of the shop plays
 */
TEST_F(ShopMusicFixture, OpensWithTheMusicOfTheShop) {
  EXPECT_EQ(ambience(), Ambience::Shop);
}

/**
 * Given the shop, its music playing
 * When the radio of the counter is clicked, on a window whose pixels are not
 * those of the interface
 * Then the radio plays its own music
 */
TEST_F(ShopMusicFixture, AClickOnTheRadioChangesTheMusic) {
  click(RADIO_MIDDLE);

  EXPECT_EQ(ambience(), Ambience::ShopRadio);
}

/**
 * Given the shop, its music playing
 * When the counter is clicked beside the radio
 * Then the music of the shop goes on
 */
TEST_F(ShopMusicFixture, AClickBesideTheRadioLeavesTheMusic) {
  click(BESIDE_THE_RADIO);

  EXPECT_EQ(ambience(), Ambience::Shop);
}

/**
 * Given the shop, its music playing
 * When R is pressed and released
 * Then the radio plays its own music
 */
TEST_F(ShopMusicFixture, RTunesTheRadio) {
  press(Key::R);

  EXPECT_EQ(ambience(), Ambience::ShopRadio);
}

/**
 * Given the radio playing its own music
 * When R is pressed and released again
 * Then the music of the shop comes back
 */
TEST_F(ShopMusicFixture, ASecondRBringsTheMusicOfTheShopBack) {
  press(Key::R);

  press(Key::R);

  EXPECT_EQ(ambience(), Ambience::Shop);
}

/**
 * Given the shop, its music playing
 * When R is held, its press repeated before any release
 * Then the radio is tuned once: it plays its own music
 */
TEST_F(ShopMusicFixture, AHeldRTunesTheRadioOnce) {
  hold(Key::R);

  hold(Key::R);

  EXPECT_EQ(ambience(), Ambience::ShopRadio);
}

/**
 * Given the shop opened over the map, its radio tuned
 * When the shop closes
 * Then the map gets its space ambience back
 */
TEST_F(ShopMusicFixture, ClosingTheShopGivesTheMapItsSpaceBack) {
  press(Key::R);

  close();

  EXPECT_EQ(ambience(), Ambience::Space);
}
