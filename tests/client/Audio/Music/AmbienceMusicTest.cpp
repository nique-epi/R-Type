#include <gtest/gtest.h>
#include "Ambience.hpp"
#include "AmbienceMusic.hpp"

using rtype::client::Ambience;
using rtype::client::replacesTheTheme;

/**
 * Given the ambiences that color a screen under the theme: none, space and
 * the hangar
 * When each is asked whether it replaces the theme
 * Then none does, and the theme plays on under them
 */
TEST(AmbienceMusic, TheAmbiencesUnderTheThemeLeaveItPlaying) {
  EXPECT_FALSE(replacesTheTheme(Ambience::None));
  EXPECT_FALSE(replacesTheTheme(Ambience::Space));
  EXPECT_FALSE(replacesTheTheme(Ambience::Hangar));
}

/**
 * Given the musics of the casino: before its doors, in its hall, at its
 * tables
 * When each is asked whether it replaces the theme
 * Then each does, so the theme pauses while it plays
 */
TEST(AmbienceMusic, TheMusicsOfTheCasinoReplaceTheTheme) {
  EXPECT_TRUE(replacesTheTheme(Ambience::CasinoFront));
  EXPECT_TRUE(replacesTheTheme(Ambience::CasinoHall));
  EXPECT_TRUE(replacesTheTheme(Ambience::CasinoTables));
}

/**
 * Given the musics of the shop and of its radio
 * When each is asked whether it replaces the theme
 * Then each does, as before the casino had its own
 */
TEST(AmbienceMusic, TheMusicsOfTheShopReplaceTheTheme) {
  EXPECT_TRUE(replacesTheTheme(Ambience::Shop));
  EXPECT_TRUE(replacesTheTheme(Ambience::ShopRadio));
  EXPECT_TRUE(replacesTheTheme(Ambience::ShopDance));
}
