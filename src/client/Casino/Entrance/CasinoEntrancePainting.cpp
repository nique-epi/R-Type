#include "CasinoEntrancePainting.hpp"
#include "CasinoEntranceConstants.hpp"
#include "CasinoPainting.hpp"
#include "CasinoSpriteConstants.hpp"
#include "EntitySprites.hpp"
#include "EntranceView.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "TextureArea.hpp"

namespace rtype::client {

void paintEntrance(IInterfaceCanvas& canvas, const EntranceView& view) {
  canvas.drawImage(
      TextureArea{.assetId = view.front, .rectangle = INTERFACE_AREA},
      ImagePlacement{.topLeft = {0, 0}}, INTERFACE_AREA);
  canvas.drawImage(
      TextureArea{.assetId = BUTLER_SHEET_ID,
                  .rectangle = rectangleOf(view.butler)},
      ImagePlacement{.topLeft = {view.butlerLeft, CASINO_BUTLER_TOP}},
      INTERFACE_AREA);
  if (view.hall.has_value()) {
    paintGoldPanel(canvas, HALL_PANEL_AREA);
    paintNameTag(canvas, HALL_TAG_TOP_LEFT, HALL_TAG);
    paintCasinoBalances(canvas, *view.hall);
  }
  paintButlerSpeech(canvas, view.line);
}

}  // namespace rtype::client
