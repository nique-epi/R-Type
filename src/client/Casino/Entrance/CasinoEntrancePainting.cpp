#include "CasinoEntrancePainting.hpp"
#include <SFML/Graphics/Rect.hpp>
#include "CasinoEntranceConstants.hpp"
#include "CasinoPainting.hpp"
#include "CasinoPaintingConstants.hpp"
#include "CasinoSpriteConstants.hpp"
#include "EntitySprites.hpp"
#include "EntranceView.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "TextureArea.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @brief Paints the panel of the butler, CASINO_SPEECH_AREA, and his name on
 * its tag; what he says is written in it by his SpokenLine. */
void paintButlerPanel(IInterfaceCanvas& canvas) {
  const sf::IntRect area = CASINO_SPEECH_AREA;
  paintGoldPanel(canvas, area);
  paintNameTag(canvas,
               {area.position.x + CASINO_SPEECH_TAG_LEFT,
                area.position.y - (CASINO_TAG_HEIGHT / SIDES_PER_AXIS)},
               BUTLER_NAME);
}

}  // namespace

void paintFront(IInterfaceCanvas& canvas, const EntranceView& view) {
  canvas.drawImage(
      TextureArea{.assetId = view.front, .rectangle = INTERFACE_AREA},
      ImagePlacement{.topLeft = {0, 0}}, INTERFACE_AREA);
}

void paintGreeter(IInterfaceCanvas& canvas, const EntranceView& view) {
  canvas.drawImage(TextureArea{.assetId = BUTLER_SHEET_ID,
                               .rectangle = rectangleOf(view.butler)},
                   ImagePlacement{.topLeft = CASINO_BUTLER_TOP_LEFT},
                   INTERFACE_AREA);
  paintButlerPanel(canvas);
}

}  // namespace rtype::client
