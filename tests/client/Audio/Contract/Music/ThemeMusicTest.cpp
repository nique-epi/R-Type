#include <gtest/gtest.h>
#include <SFML/Audio/InputSoundFile.hpp>
#include <SFML/System/Time.hpp>
#include <filesystem>
#include "AudioConstants.hpp"
#include "AudioTestConstants.hpp"
#include "ThemeLoopFixtures.hpp"

using rtype::client::THEME_MUSIC_ID;
using rtype::client::testing::THEME_FADE_START;

/**
 * Given the theme of the repository
 * When it is opened, as the music is
 * Then it can be read, and it lasts longer than its loop, from the start of
 * the file to the end of the loop
 */
TEST(ThemeMusic, TheThemeHoldsItsLoop) {
  sf::InputSoundFile theme;

  ASSERT_TRUE(theme.openFromFile(
      std::filesystem::path(RTYPE_SOURCE_ASSETS_FOLDER) / THEME_MUSIC_ID));
  EXPECT_GT(theme.getDuration().asSeconds(), themeLoopEnd());
}

/**
 * Given the part of the theme that loops
 * When its end is compared with the fade out of the file
 * Then it ends before the fade, so the loop never plays it
 */
TEST(ThemeMusic, TheLoopEndsBeforeTheFadeOut) {
  EXPECT_LT(themeLoopEnd(), THEME_FADE_START);
}
