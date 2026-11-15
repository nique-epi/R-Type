#include "RoulettePainting.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include "CasinoSpriteConstants.hpp"
#include "ClothPainting.hpp"
#include "DrawnNumbersPainting.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "RouletteBandPainting.hpp"
#include "RouletteScreenConstants.hpp"
#include "RouletteView.hpp"
#include "TableRosterPainting.hpp"
#include "TextureArea.hpp"
#include "WheelPicture.hpp"

namespace rtype::client {

namespace {

/** @brief Copies @p picture, its top-left corner at @p topLeft. */
void paintPicture(IInterfaceCanvas& canvas, const TextureArea& picture,
                  sf::Vector2i topLeft) {
  canvas.drawImage(picture, ImagePlacement{.topLeft = topLeft}, INTERFACE_AREA);
}

/** @returns The rectangle of frame @p frame of the turns of the rotor. */
sf::IntRect rotorFrameOf(int frame) {
  return {{(frame % WHEEL_ROTOR_TURN_COLUMNS) * WHEEL_FRAME_SIZE.x,
           (frame / WHEEL_ROTOR_TURN_COLUMNS) * WHEEL_FRAME_SIZE.y},
          WHEEL_FRAME_SIZE};
}

/** @brief Paints the wheel as @p wheel shows it: its bowl, its rotor, then
 * its ball. */
void paintWheel(IInterfaceCanvas& canvas, const WheelPicture& wheel) {
  paintPicture(canvas,
               TextureArea{.assetId = WHEEL_BOWL_ID,
                           .rectangle = {{0, 0}, WHEEL_FRAME_SIZE}},
               WHEEL_TOP_LEFT);
  paintPicture(canvas,
               TextureArea{.assetId = WHEEL_ROTOR_TURN_ID,
                           .rectangle = rotorFrameOf(wheel.rotorFrame)},
               WHEEL_TOP_LEFT);
  if (wheel.ball.has_value()) {
    paintPicture(canvas,
                 TextureArea{.assetId = WHEEL_BALL_ID,
                             .rectangle = {{0, 0}, WHEEL_BALL_SIZE}},
                 WHEEL_TOP_LEFT + *wheel.ball);
  }
}

/** @brief Paints the room, the croupier behind the table, then the table. */
void paintRoom(IInterfaceCanvas& canvas, bool croupierInvites) {
  paintPicture(
      canvas,
      TextureArea{.assetId = ROULETTE_ROOM_ID, .rectangle = INTERFACE_AREA},
      {0, 0});
  paintPicture(
      canvas,
      TextureArea{.assetId = BUTLER_SHEET_ID,
                  .rectangle = croupierInvites ? CROUPIER_INVITING_FRAME
                                               : CROUPIER_WAITING_FRAME},
      CROUPIER_TOP_LEFT);
  paintPicture(canvas,
               TextureArea{.assetId = ROULETTE_TABLE_ID,
                           .rectangle = {{0, 0},
                                         {INTERFACE_AREA.size.x,
                                          ROULETTE_TABLE_HEIGHT}}},
               ROULETTE_TABLE_TOP_LEFT);
}

}  // namespace

void paintRoulette(IInterfaceCanvas& canvas, const RouletteView& view) {
  paintRoom(canvas, view.croupierInvites);
  paintWheel(canvas, view.wheel);
  if (view.drawn.has_value()) {
    paintDrawnNumber(canvas, *view.drawn);
  }
  paintClothBets(canvas, view.bets);
  if (view.cursor.has_value()) {
    paintClothCursor(canvas, *view.cursor);
  }
  if (view.drawn.has_value()) {
    paintResultPlaque(canvas, *view.drawn);
  }
  paintLastNumbers(canvas, view.lastNumbers);
  paintTableRoster(canvas, view.roster);
  paintRouletteBand(canvas, view.band);
}

}  // namespace rtype::client
