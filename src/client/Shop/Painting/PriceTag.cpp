#include "PriceTag.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include "ColoredPixel.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceColorConstants.hpp"
#include "PriceTagConstants.hpp"

namespace rtype::client {

namespace {

/** @brief Adds the ink of @p digit, a character '0' to '9', from its
 * top-left corner. */
void addDigit(std::vector<ColoredPixel>& pixels, char digit,
              sf::Vector2i topLeft) {
  const std::string_view rows =
      PRICE_DIGIT_PIXELS.at(static_cast<std::size_t>(digit - '0'));
  for (std::size_t place = 0; place < rows.size(); ++place) {
    if (rows[place] != PRICE_DIGIT_INK) {
      continue;
    }
    const auto column = static_cast<int>(place) % PRICE_DIGIT_WIDTH;
    const auto row = static_cast<int>(place) / PRICE_DIGIT_WIDTH;
    pixels.push_back(
        {.position = topLeft + sf::Vector2i(column, row), .color = INK});
  }
}

}  // namespace

int priceTagWidth(std::uint32_t price) {
  const auto digits = static_cast<int>(std::to_string(price).size());
  return PRICE_TAG_DIGITS_LEFT + (digits * PRICE_DIGIT_ADVANCE) +
         PRICE_TAG_END_MARGIN;
}

void paintPriceTag(IInterfaceCanvas& canvas, sf::Vector2i topRight,
                   std::uint32_t price) {
  const int width = priceTagWidth(price);
  const sf::Vector2i topLeft(topRight.x - width, topRight.y);
  canvas.fill({topLeft, {width, PRICE_TAG_HEIGHT}}, PRICE_TAG_PAPER_COLOR);
  canvas.fill({{topLeft.x, topLeft.y + PRICE_TAG_HEIGHT - 1}, {width, 1}},
              PRICE_TAG_SHADE_COLOR);
  canvas.fill({topLeft + sf::Vector2i(PRICE_TAG_COIN_LEFT, PRICE_TAG_COIN_TOP),
               {PRICE_TAG_COIN_WIDTH, PRICE_TAG_COIN_HEIGHT}},
              PRICE_TAG_COIN_COLOR);
  std::vector<ColoredPixel> ink;
  sf::Vector2i digitTopLeft =
      topLeft + sf::Vector2i(PRICE_TAG_DIGITS_LEFT, PRICE_TAG_DIGITS_TOP);
  for (const char digit : std::to_string(price)) {
    addDigit(ink, digit, digitTopLeft);
    digitTopLeft.x += PRICE_DIGIT_ADVANCE;
  }
  canvas.plot(ink);
}

}  // namespace rtype::client
