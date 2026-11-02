#include "SilenceWarning.hpp"
#include <SFML/System/Vector2.hpp>
#include "EntitySprites.hpp"
#include "GameNoticeConstants.hpp"
#include "HudSpriteConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

void SilenceWarning::hear() { silence_ = engine::Duration::zero(); }

void SilenceWarning::advance(engine::Duration elapsed) { silence_ += elapsed; }

void SilenceWarning::stop() { stopped_ = true; }

bool SilenceWarning::isShown() const {
  return !stopped_ && silence_ >= SILENCE_WARNING_DELAY;
}

void SilenceWarning::paint(IInterfaceCanvas& canvas) const {
  if (!isShown()) {
    return;
  }
  canvas.drawImage(TextureArea{.assetId = HUD_SHEET_ID,
                               .rectangle = rectangleOf(WEAK_SIGNAL_FRAME)},
                   ImagePlacement{.topLeft = WEAK_SIGNAL_TOP_LEFT},
                   INTERFACE_AREA);
  canvas.write(SILENCE_WARNING_TEXT,
               WEAK_SIGNAL_TOP_LEFT +
                   sf::Vector2i(-SILENCE_WARNING_GAP, SILENCE_WARNING_DROP),
               {.font = TextFont::Small,
                .color = SILENCE_WARNING_COLOR,
                .outlined = true,
                .alignment = TextAlignment::Right});
}

}  // namespace rtype::client
