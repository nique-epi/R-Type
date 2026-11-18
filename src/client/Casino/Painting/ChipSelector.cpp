#include "ChipSelector.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <optional>
#include "BoxCorners.hpp"
#include "CasinoSpriteConstants.hpp"
#include "ChipRow.hpp"
#include "ChipSelectorConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "KeyCap.hpp"
#include "TextureArea.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @returns Where the chip of @p place lies at rest in a row whose first chip
 * lies at @p firstChip. */
sf::Vector2i restingTopLeft(sf::Vector2i firstChip, std::size_t place) {
  return {firstChip.x + (static_cast<int>(place) * CHIP_SPACING), firstChip.y};
}

/** @brief Paints the chip of @p place of @p row, raised and in corners when
 * it is the chosen one, and the keycap of its key under it. */
void paintChip(IInterfaceCanvas& canvas, const ChipRow& row,
               std::size_t place) {
  const sf::Vector2i topLeft = chipTopLeft(row, place);
  canvas.drawImage(
      TextureArea{
          .assetId = CHIP_SELECTOR_SHEET_ID,
          .rectangle = {{static_cast<int>(place) * CHIP_SELECTOR_STRIDE, 0},
                        CHIP_SELECTOR_FRAME_SIZE}},
      ImagePlacement{.topLeft = topLeft}, INTERFACE_AREA);
  if (place == row.chosen) {
    const sf::Vector2i margin(CHOSEN_CHIP_CORNERS_MARGIN,
                              CHOSEN_CHIP_CORNERS_MARGIN);
    paintBoxCorners(canvas,
                    {topLeft - margin,
                     CHIP_SELECTOR_FRAME_SIZE + (margin * SIDES_PER_AXIS)},
                    HUD_CYAN);
  }
  KeyCap(CHIP_KEY_NAMES.at(place), KeyCap::Edge::Plain)
      .paint(canvas, {topLeft.x + CHIP_KEY_CAP_INSET,
                      row.firstChip.y + CHIP_KEY_CAP_DROP});
}

}  // namespace

sf::Vector2i chipTopLeft(const ChipRow& row, std::size_t place) {
  const sf::Vector2i resting = restingTopLeft(row.firstChip, place);
  return {resting.x, resting.y - (place == row.chosen ? CHOSEN_CHIP_LIFT : 0)};
}

std::optional<std::size_t> chipAt(const ChipRow& row, sf::Vector2i pixel) {
  for (std::size_t place = 0; place < CHIP_VALUE_COUNT; ++place) {
    if (sf::IntRect(restingTopLeft(row.firstChip, place),
                    CHIP_SELECTOR_FRAME_SIZE)
            .contains(pixel)) {
      return place;
    }
  }
  return std::nullopt;
}

std::optional<std::size_t> chipOfKey(sf::Keyboard::Key key) {
  for (std::size_t place = 0; place < CHIP_KEYS.size(); ++place) {
    if (CHIP_KEYS.at(place) == key) {
      return place;
    }
  }
  return std::nullopt;
}

void paintChipRow(IInterfaceCanvas& canvas, const ChipRow& row) {
  for (std::size_t place = 0; place < CHIP_VALUE_COUNT; ++place) {
    paintChip(canvas, row, place);
  }
}

}  // namespace rtype::client
