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
