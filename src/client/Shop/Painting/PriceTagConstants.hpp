#pragma once

#include <SFML/Graphics/Color.hpp>
#include <array>
#include <string_view>

namespace rtype::client {

/** @brief A price tag: a cream label PRICE_TAG_HEIGHT tall, its bottom row
 * shaded, a small gold coin at PRICE_TAG_COIN_LEFT and PRICE_TAG_COIN_TOP
 * inside it, then the price in ink from PRICE_TAG_DIGITS_LEFT and
 * PRICE_TAG_DIGITS_TOP, each digit PRICE_DIGIT_WIDTH wide and as tall as the
 * rows of PRICE_DIGIT_PIXELS, PRICE_DIGIT_ADVANCE apart, and
 * PRICE_TAG_END_MARGIN after the last. */
constexpr int PRICE_TAG_HEIGHT = 7;
constexpr int PRICE_TAG_DIGITS_LEFT = 4;
constexpr int PRICE_TAG_DIGITS_TOP = 1;
constexpr int PRICE_TAG_END_MARGIN = 3;
constexpr int PRICE_DIGIT_WIDTH = 3;
constexpr int PRICE_DIGIT_ADVANCE = PRICE_DIGIT_WIDTH + 1;
constexpr int PRICE_TAG_COIN_LEFT = 1;
constexpr int PRICE_TAG_COIN_TOP = 2;
constexpr int PRICE_TAG_COIN_WIDTH = 2;
constexpr int PRICE_TAG_COIN_HEIGHT = 3;
constexpr sf::Color PRICE_TAG_PAPER_COLOR(232, 220, 188);
constexpr sf::Color PRICE_TAG_SHADE_COLOR(184, 168, 136);
constexpr sf::Color PRICE_TAG_COIN_COLOR(255, 197, 49);

/** @brief The pixels of each digit, 0 to 9, row by row from its top-left
 * corner: 'x' for ink. */
constexpr char PRICE_DIGIT_INK = 'x';
constexpr std::array<std::string_view, 10> PRICE_DIGIT_PIXELS{
    "xxxx.xx.xx.xxxx", ".x.xx..x..x.xxx", "xxx..xxxxx..xxx", "xxx..xxxx..xxxx",
    "x.xx.xxxx..x..x", "xxxx..xxx..xxxx", "xxxx..xxxx.xxxx", "xxx..x..x..x..x",
    "xxxx.xxxxx.xxxx", "xxxx.xxxx..xxxx"};

}  // namespace rtype::client
