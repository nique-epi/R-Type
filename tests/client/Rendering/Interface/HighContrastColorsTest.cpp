#include <gtest/gtest.h>
#include <SFML/Graphics/Color.hpp>
#include <algorithm>
#include <cstddef>
#include "HighContrastColorConstants.hpp"
#include "HighContrastColors.hpp"
#include "HighContrastColorsFixtures.hpp"
#include "HighContrastColorsTestConstants.hpp"

using rtype::client::HIGH_CONTRAST_BLACK;
using rtype::client::highContrastOf;
using rtype::client::testing::contrastRatioOf;
using rtype::client::testing::HIGH_CONTRAST_RATIO;
using rtype::client::testing::highContrastRatioOf;
using rtype::client::testing::TextOnBackground;
using rtype::client::testing::TEXTS_ON_THEIR_BACKGROUNDS;
using rtype::client::testing::TRANSLUCENT_SKY;
using rtype::client::testing::UNTOUCHED_COLOR;

/**
 * Given every text and edge of the menus and the status band on its
 * background
 * When each pair is drawn in high contrast
 * Then each reads at 7:1 or more, the ratio of a high contrast mode
 */
TEST(HighContrastColors, EveryTextReachesSevenToOne) {
  for (std::size_t index = 0; index < TEXTS_ON_THEIR_BACKGROUNDS.size();
       ++index) {
    SCOPED_TRACE(index);
    EXPECT_GE(highContrastRatioOf(TEXTS_ON_THEIR_BACKGROUNDS[index]),
              HIGH_CONTRAST_RATIO);
  }
}

/**
 * Given the same pairs in the colors of the game
 * When they are measured without high contrast
 * Then one of them at least reads under 7:1, so the mode changes something
 */
TEST(HighContrastColors, TheGameColorsMissSevenToOne) {
  EXPECT_TRUE(std::ranges::any_of(
      TEXTS_ON_THEIR_BACKGROUNDS, [](TextOnBackground pair) {
        return contrastRatioOf(pair.text, pair.background) <
               HIGH_CONTRAST_RATIO;
      }));
}

/**
 * Given a color the high contrast table does not name
 * When it is drawn in high contrast
 * Then it is drawn as it is
 */
TEST(HighContrastColors, AnUnnamedColorIsKept) {
  EXPECT_EQ(highContrastOf(UNTOUCHED_COLOR), UNTOUCHED_COLOR);
}

/**
 * Given the translucent sky laid under a dialog
 * When it is drawn in high contrast
 * Then it turns black and keeps its opacity
 */
TEST(HighContrastColors, AReplacedColorKeepsItsOpacity) {
  const sf::Color shown = highContrastOf(TRANSLUCENT_SKY);

  EXPECT_EQ(shown, sf::Color(HIGH_CONTRAST_BLACK.r, HIGH_CONTRAST_BLACK.g,
                             HIGH_CONTRAST_BLACK.b, TRANSLUCENT_SKY.a));
}
