#include "DrawnNumbersPainting.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "BackgroundColorConstants.hpp"
#include "CasinoPainting.hpp"
#include "CasinoPaintingConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "PocketColors.hpp"
#include "PocketShades.hpp"
#include "RouletteColorConstants.hpp"
#include "RouletteScreenConstants.hpp"
#include "TextStyle.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The outer and inner shades of a box of @p number: one step
 * darker for the zero, whose green is lighter. */
PocketShades boxShadesOf(std::uint8_t number) {
  const PocketShades shades = pocketShadesOf(number);
  if (number == 0) {
    return {.shadow = shades.shadow,
            .dark = shades.shadow,
            .base = shades.dark,
            .light = shades.base};
  }
  return shades;
}

/** @brief Paints the box of @p number at @p place of the numbers drawn. */
void paintLastNumber(IInterfaceCanvas& canvas, std::uint8_t number,
                     std::size_t place) {
  const sf::IntRect box(
      {LAST_FIRST_BOX.x + (static_cast<int>(place) * LAST_NUMBER_STEP),
       LAST_FIRST_BOX.y},
      LAST_BOX_SIZE);
  canvas.fill(box, boxShadesOf(number).base);
  paintEdges(canvas, box, place == 0 ? STAR_GOLD : CASINO_BRONZE);
  canvas.write(std::to_string(number),
               {box.position.x + (box.size.x / SIDES_PER_AXIS) + 1,
                box.position.y + LAST_NUMBER_TOP},
               TextStyle{.font = TextFont::Small,
                         .color = STAR_WHITE,
                         .alignment = TextAlignment::Center});
}

}  // namespace

void paintResultPlaque(IInterfaceCanvas& canvas, std::uint8_t number) {
  const PocketShades shades = boxShadesOf(number);
  const sf::IntRect area = RESULT_PLAQUE_AREA;
  canvas.fill(area, shades.dark);
  canvas.fill(insetOf(area, RESULT_PLAQUE_INNER_EDGE), shades.base);
  paintEdges(canvas, area, CASINO_LIGHT_GOLD);
  paintEdges(canvas, insetOf(area, 1), CASINO_DARK_GOLD);
  canvas.fill({area.position, {1, 1}}, CASINO_PALE_GOLD);
  canvas.write(std::to_string(number),
               {area.position.x + (area.size.x / SIDES_PER_AXIS) + 1,
                area.position.y + RESULT_NUMBER_TOP},
               TextStyle{.font = TextFont::Regular,
                         .color = STAR_WHITE,
                         .outlined = true,
                         .alignment = TextAlignment::Center,
                         .scale = RESULT_NUMBER_SCALE});
  canvas.write(pocketColorWordOf(number), RESULT_COLOR_WORD,
               TextStyle{.font = TextFont::Regular, .color = STAR_GOLD});
  canvas.write(WINS_WORD, RESULT_WINS_WORD,
               TextStyle{.font = TextFont::Small, .color = PALE_BLUE});
}

void paintLastNumbers(IInterfaceCanvas& canvas,
                      const std::vector<std::uint8_t>& numbers) {
  if (numbers.empty()) {
    return;
  }
  canvas.write(LAST_CAPTION_TEXT, LAST_CAPTION,
               TextStyle{.font = TextFont::Small, .color = PALE_BLUE});
  for (std::size_t place = 0; place < numbers.size(); ++place) {
    paintLastNumber(canvas, numbers.at(place), place);
  }
}

}  // namespace rtype::client
