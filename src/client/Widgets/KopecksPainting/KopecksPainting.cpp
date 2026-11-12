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

}  // namespace

int kopecksLabelWidth(const KopecksLabel& label) {
  return KOPECK_FRAME.width + KOPECKS_COIN_GAP +
         regularTextWidth(writtenAmountOf(label));
}

void paintKopecks(IInterfaceCanvas& canvas, const KopecksLabel& label) {
  canvas.write(writtenAmountOf(label), label.topRight,
               {.font = TextFont::Regular,
                .color = KOPECKS_COLOR,
                .outlined = label.outlined,
                .alignment = TextAlignment::Right});
  canvas.drawImage(
      TextureArea{.assetId = HUD_SHEET_ID,
                  .rectangle = rectangleOf(KOPECK_FRAME)},
      ImagePlacement{.topLeft = {label.topRight.x - kopecksLabelWidth(label),
                                 label.topRight.y}},
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
  paintKopecks(canvas, KopecksLabel{.topRight = amountEnd, .amount = balance});
  canvas.write(
      KOPECKS_CAPTION,
      {amountEnd.x, amountEnd.y + KOPECKS_LABEL_HEIGHT + KOPECKS_CAPTION_GAP},
      {.font = TextFont::Small,
       .color = KOPECKS_CAPTION_COLOR,
       .alignment = TextAlignment::Right});
}

}  // namespace rtype::client
