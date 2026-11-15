#include "ClothPainting.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "BackgroundColorConstants.hpp"
#include "BoxCorners.hpp"
#include "CasinoPainting.hpp"
#include "CasinoSpriteConstants.hpp"
#include "ChipSelectorConstants.hpp"
#include "ClothBets.hpp"
#include "ClothGeometry.hpp"
#include "ClothPlace.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "PocketColors.hpp"
#include "PocketShades.hpp"
#include "RouletteBets.hpp"
#include "RouletteScreenConstants.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The places in CHIP_VALUES of the chips @p amount is made of, the
 * largest first, CLOTH_STACK_CHIPS at most. */
std::vector<std::size_t> chipsOfStack(std::uint32_t amount) {
  std::vector<std::size_t> chips;
  std::uint32_t left = amount;
  for (std::size_t place = CHIP_VALUE_COUNT; place-- > 0;) {
    while (left >= CHIP_VALUES.at(place) && chips.size() < CLOTH_STACK_CHIPS) {
      chips.push_back(place);
      left -= CHIP_VALUES.at(place);
    }
  }
  return chips;
}

/** @brief Paints the stack of @p bet on its spot. */
void paintStack(IInterfaceCanvas& canvas, const network::RouletteBet& bet) {
  const sf::Vector2i bottom =
      spotOf(placeOf(bet)) - (CLOTH_CHIP_FRAME_SIZE / SIDES_PER_AXIS);
  int lift = 0;
  for (const std::size_t chip : chipsOfStack(bet.amount)) {
    canvas.drawImage(
        TextureArea{
            .assetId = CASINO_CHIPS_SHEET_ID,
            .rectangle = {{static_cast<int>(chip) * CLOTH_CHIP_STRIDE, 0},
                          CLOTH_CHIP_FRAME_SIZE}},
        ImagePlacement{.topLeft = {bottom.x, bottom.y - lift}}, INTERFACE_AREA);
    lift += CLOTH_CHIP_LIFT;
  }
}

/** @returns @p box, CLOTH_CORNERS_MARGIN larger on every side. */
sf::IntRect outsetOf(sf::IntRect box) {
  return insetOf(box, -CLOTH_CORNERS_MARGIN);
}

}  // namespace

void paintClothBets(IInterfaceCanvas& canvas,
                    const std::vector<network::RouletteBet>& bets) {
  for (const network::RouletteBet& bet : bets) {
    paintStack(canvas, bet);
  }
}

void paintClothCursor(IInterfaceCanvas& canvas, ClothPlace place) {
  paintBoxCorners(canvas, outsetOf(areaOf(place)), HUD_CYAN);
}

void paintDrawnNumber(IInterfaceCanvas& canvas, std::uint8_t number) {
  const sf::IntRect cell = numberCellOf(number);
  const sf::Vector2i inside = cell.size - sf::Vector2i(1, 1);
  const PocketShades shades = pocketShadesOf(number);
  canvas.fill({cell.position + sf::Vector2i(1, 1), inside - sf::Vector2i(1, 1)},
              shades.base);
  canvas.fill({cell.position + sf::Vector2i(1, 1), {inside.x - 1, 1}},
              shades.light);
  canvas.fill({{cell.position.x + 1, cell.position.y + inside.y - 1},
               {inside.x - 1, 1}},
              shades.dark);
  canvas.write(std::to_string(number),
               {cell.position.x + ((inside.x + 1) / SIDES_PER_AXIS) + 1,
                cell.position.y +
                    ((inside.y - CLOTH_NUMBER_HEIGHT) / SIDES_PER_AXIS) + 1},
               TextStyle{.font = TextFont::Regular,
                         .color = STAR_GOLD,
                         .alignment = TextAlignment::Center});
  paintBoxCorners(canvas, outsetOf(cell), STAR_GOLD);
}

}  // namespace rtype::client
