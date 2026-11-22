#include "RouletteBandPainting.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include "BackgroundColorConstants.hpp"
#include "CasinoPaintingConstants.hpp"
#include "ChipRow.hpp"
#include "ChipSelector.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceColorConstants.hpp"
#include "Kopecks.hpp"
#include "RouletteBand.hpp"
#include "RouletteScreenConstants.hpp"
#include "TextStyle.hpp"

namespace rtype::client {

void paintRouletteBand(IInterfaceCanvas& canvas, const RouletteBand& band) {
  const sf::Vector2i topLeft = BAND_AREA.position;
  canvas.fill(BAND_AREA, SKY_COLOR);
  canvas.fill({topLeft, {BAND_AREA.size.x, 1}}, CASINO_GOLD);
  canvas.fill({{topLeft.x, topLeft.y + 1}, {BAND_AREA.size.x, 1}},
              CASINO_DARK_GOLD);
  const TextStyle caption{.font = TextFont::Small, .color = PALE_BLUE};
  canvas.write(BAND_CHIPS_CAPTION_TEXT.in(canvas.language()),
               BAND_CHIPS_CAPTION, caption);
  canvas.write(kopecksText(band.chips), BAND_CHIPS_VALUE,
               TextStyle{.font = TextFont::Regular, .color = STAR_GOLD});
  canvas.write(BAND_STAKE_CAPTION_TEXT.in(canvas.language()),
               BAND_STAKE_CAPTION, caption);
  canvas.write(kopecksText(band.stake), BAND_STAKE_VALUE,
               TextStyle{.font = TextFont::Regular, .color = STAR_WHITE});
  canvas.write(band.status, BAND_STATUS,
               TextStyle{.font = TextFont::Small, .color = HUD_CYAN});
  paintChipRow(
      canvas, ChipRow{.firstChip = BAND_FIRST_CHIP, .chosen = band.chosenChip});
}

}  // namespace rtype::client
