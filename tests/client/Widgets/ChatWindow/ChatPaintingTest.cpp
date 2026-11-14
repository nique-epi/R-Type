#include <gtest/gtest.h>
#include "ChatPainting.hpp"
#include "ChatWindowTestConstants.hpp"
#include "PlayerSettings.hpp"
#include "TextSize.hpp"

using rtype::client::chatLettersOf;
using rtype::client::PlayerSettings;
using rtype::client::TextSize;
using rtype::client::testing::LETTERS_AT_THE_LARGEST_SIZE;

/**
 * Given the letters of a line of the chat left as they are at first launch
 * When the text is at the size the fonts are drawn at
 * Then a line holds the letters chosen
 */
TEST(ChatPainting, AtTheNormalSizeALineHoldsTheLettersChosen) {
  const PlayerSettings settings;

  EXPECT_EQ(chatLettersOf(settings), settings.chatLetters);
}

/**
 * Given the most letters a line of the chat may hold
 * When the text is at the largest size
 * Then a line holds only the letters that fit between the margins
 */
TEST(ChatPainting, AtTheLargestSizeALineHoldsWhatFits) {
  PlayerSettings settings;
  settings.textSize = TextSize::Largest;

  EXPECT_EQ(chatLettersOf(settings), LETTERS_AT_THE_LARGEST_SIZE);
}
