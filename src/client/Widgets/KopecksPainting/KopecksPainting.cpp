#include "KopecksPainting.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <optional>
#include <string>
#include "CutCornerBox.hpp"
#include "EntitySprites.hpp"
#include "HudSpriteConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "Kopecks.hpp"
#include "KopecksLabel.hpp"
#include "KopecksLabelConstants.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

namespace {

/** @returns The digits of @p label as written: its prefix, then its amount,
 * thousands apart. */
std::string writtenAmountOf(const KopecksLabel& label) {
  return std::string{label.prefix} + kopecksText(label.amount);
}

/** @brief Writes @p written where @p label places its amount, and the coin
 * left of it. */
void paintCoinAndText(IInterfaceCanvas& canvas, const std::string& written,
                      const KopecksLabel& label) {
  const int width =
      KOPECK_FRAME.width + KOPECKS_COIN_GAP + regularTextWidth(written);
  canvas.write(written, label.topRight,
               {.font = TextFont::Regular,
                .color = KOPECKS_COLOR,
                .outlined = label.outlined,
                .alignment = TextAlignment::Right});
  canvas.drawImage(
      TextureArea{.assetId = HUD_SHEET_ID,
                  .rectangle = rectangleOf(KOPECK_FRAME)},
      ImagePlacement{.topLeft = {label.topRight.x - width, label.topRight.y}},
      INTERFACE_AREA);
}

}  // namespace

int kopecksLabelWidth(const KopecksLabel& label) {
  return KOPECK_FRAME.width + KOPECKS_COIN_GAP +
         regularTextWidth(writtenAmountOf(label));
}

void paintKopecks(IInterfaceCanvas& canvas, const KopecksLabel& label) {
  paintCoinAndText(canvas, writtenAmountOf(label), label);
}

void paintUnknownKopecks(IInterfaceCanvas& canvas, const KopecksLabel& label) {
  paintCoinAndText(canvas, std::string{UNKNOWN_KOPECKS_TEXT}, label);
}

void paintKopecksBalance(IInterfaceCanvas& canvas, sf::Vector2i topRight,
                         std::optional<std::uint32_t> balance) {
  CutCornerBox(sf::IntRect({topRight.x - KOPECKS_FRAME_SIZE.x, topRight.y},
                           KOPECKS_FRAME_SIZE),
               {.border = KOPECKS_FRAME_BORDER_COLOR,
                .inside = KOPECKS_FRAME_INSIDE_COLOR})
      .paint(canvas);
  const sf::Vector2i amountEnd(topRight.x - KOPECKS_FRAME_PADDING.x,
                               topRight.y + KOPECKS_FRAME_PADDING.y);
  const KopecksLabel label{.topRight = amountEnd,
                           .amount = balance.value_or(0),
                           .prefix = {},
                           .outlined = false};
  if (balance.has_value()) {
    paintKopecks(canvas, label);
  } else {
    paintUnknownKopecks(canvas, label);
  }
  canvas.write(
      KOPECKS_CAPTION,
      {amountEnd.x, amountEnd.y + KOPECKS_LABEL_HEIGHT + KOPECKS_CAPTION_GAP},
      {.font = TextFont::Small,
       .color = KOPECKS_CAPTION_COLOR,
       .alignment = TextAlignment::Right});
}

}  // namespace rtype::client
