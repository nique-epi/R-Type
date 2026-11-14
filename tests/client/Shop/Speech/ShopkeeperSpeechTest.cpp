#include <gtest/gtest.h>
#include <array>
#include <cstddef>
#include <span>
#include <string_view>
#include "MotionPreference.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ShopkeeperLineConstants.hpp"
#include "ShopkeeperSpeech.hpp"
#include "ShopkeeperSpeechConstants.hpp"
#include "ShopkeeperSpeechTestConstants.hpp"
#include "SpeechLines.hpp"
#include "SpokenLineConstants.hpp"
#include "WidgetConstants.hpp"

using rtype::client::MotionPreference;
using rtype::client::SHOPKEEPER_ALREADY_YOURS_LINE;
using rtype::client::SHOPKEEPER_BORDER_RADIO_LINES;
using rtype::client::SHOPKEEPER_BOTTLES_LINES;
using rtype::client::SHOPKEEPER_BOUGHT_LINES;
using rtype::client::SHOPKEEPER_BUBBLE_WIDTH;
using rtype::client::SHOPKEEPER_GREETINGS;
using rtype::client::SHOPKEEPER_MAX_LINES;
using rtype::client::SHOPKEEPER_NOT_FOR_SALE_LINE;
using rtype::client::SHOPKEEPER_RADIO_HINT;
using rtype::client::SHOPKEEPER_SHOP_MUSIC_LINE;
using rtype::client::SHOPKEEPER_TOO_EXPENSIVE_LINES;
using rtype::client::ShopkeeperSpeech;
using rtype::client::SIDES_PER_AXIS;
using rtype::client::speechLinesOf;
using rtype::client::SPOKEN_BUBBLE_PADDING;
using rtype::client::testing::FIRST_GREETING_WORD;
using rtype::client::testing::FOUR_ITEMS;
using rtype::client::testing::NO_ITEM;
using rtype::client::testing::SECOND_GREETING_WORD;

/**
 * Given every line the shopkeeper says
 * When each is cut to the text of his bubble
 * Then none takes more lines than his bubble holds
 */
TEST(ShopkeeperSpeechTest, EveryLineFitsHisBubble) {
  const int textWidth =
      SHOPKEEPER_BUBBLE_WIDTH - (SIDES_PER_AXIS * SPOKEN_BUBBLE_PADDING);
  const std::array<std::span<const std::string_view>, 5> lists{
      SHOPKEEPER_GREETINGS, SHOPKEEPER_BOUGHT_LINES,
      SHOPKEEPER_TOO_EXPENSIVE_LINES, SHOPKEEPER_BOTTLES_LINES,
      SHOPKEEPER_BORDER_RADIO_LINES};
  const std::array<std::string_view, 4> singles{
      SHOPKEEPER_RADIO_HINT, SHOPKEEPER_ALREADY_YOURS_LINE,
      SHOPKEEPER_NOT_FOR_SALE_LINE, SHOPKEEPER_SHOP_MUSIC_LINE};
  for (const std::span<const std::string_view> list : lists) {
    for (const std::string_view line : list) {
      EXPECT_LE(speechLinesOf(line, {}, textWidth).size(),
                static_cast<std::size_t>(SHOPKEEPER_MAX_LINES))
          << line;
    }
  }
  for (const std::string_view line : singles) {
    EXPECT_LE(speechLinesOf(line, {}, textWidth).size(),
              static_cast<std::size_t>(SHOPKEEPER_MAX_LINES))
        << line;
  }
}

/**
 * Given a shopkeeper who shows his lines whole
 * When he greets a player who owns no item, then one who owns four
 * Then he says his first greeting, then his second
 */
TEST(ShopkeeperSpeechTest, TheGreetingFollowsTheItemsOwned) {
  RecordingSoundPlayer sounds;
  ShopkeeperSpeech first(sounds, MotionPreference::Reduced);
  ShopkeeperSpeech second(sounds, MotionPreference::Reduced);
  InterfaceCanvasRecord firstPainting;
  RecordingInterfaceCanvas firstCanvas(firstPainting);
  InterfaceCanvasRecord secondPainting;
  RecordingInterfaceCanvas secondCanvas(secondPainting);

  first.greet(NO_ITEM);
  first.paint(firstCanvas);
  second.greet(FOUR_ITEMS);
  second.paint(secondCanvas);

  EXPECT_NE(writingOf(firstPainting, FIRST_GREETING_WORD), nullptr);
  EXPECT_NE(writingOf(secondPainting, SECOND_GREETING_WORD), nullptr);
}
