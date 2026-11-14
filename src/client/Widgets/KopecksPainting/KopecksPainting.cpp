#include "KopecksPainting.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
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
#include "TextMeasures.hpp"
#include "TextMetrics.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"

namespace rtype::client {

namespace {

/** @returns The digits of @p label as written: its prefix, then its amount,
 * thousands apart. */
std::string writtenAmountOf(const KopecksLabel& label) {
  return std::string{label.prefix} + kopecksText(label.amount);
}

}  // namespace

int kopecksLabelWidth(const KopecksLabel& label, TextMetrics metrics) {
  return KOPECK_FRAME.width + KOPECKS_COIN_GAP +
         regularWidthOf(metrics, writtenAmountOf(label));
}

void paintKopecks(IInterfaceCanvas& canvas, const KopecksLabel& label) {
  const std::string written = writtenAmountOf(label);
  const int width = KOPECK_FRAME.width + KOPECKS_COIN_GAP +
                    regularWidthOf(canvas.textMetrics(), written);
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

void paintKopecksBalance(IInterfaceCanvas& canvas, sf::Vector2i topRight,
                         std::uint32_t balance) {
  CutCornerBox(sf::IntRect({topRight.x - KOPECKS_FRAME_SIZE.x, topRight.y},
                           KOPECKS_FRAME_SIZE),
               {.border = KOPECKS_FRAME_BORDER_COLOR,
                .inside = KOPECKS_FRAME_INSIDE_COLOR})
      .paint(canvas);
  const sf::Vector2i amountEnd(topRight.x - KOPECKS_FRAME_PADDING.x,
                               topRight.y + KOPECKS_FRAME_PADDING.y);
  paintKopecks(canvas, KopecksLabel{.topRight = amountEnd,
                                    .amount = balance,
                                    .prefix = {},
                                    .outlined = false});
  canvas.write(
      KOPECKS_CAPTION,
      {amountEnd.x, amountEnd.y + KOPECKS_LABEL_HEIGHT + KOPECKS_CAPTION_GAP},
      {.font = TextFont::Small,
       .color = KOPECKS_CAPTION_COLOR,
       .alignment = TextAlignment::Right});
}

}  // namespace rtype::client
