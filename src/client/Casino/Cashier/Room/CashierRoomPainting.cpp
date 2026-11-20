#include "CashierRoomPainting.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <span>
#include "BackgroundColorConstants.hpp"
#include "CashierConstants.hpp"
#include "CashierRoom.hpp"
#include "CashierRoomConstants.hpp"
#include "CashierRoomMoment.hpp"
#include "CashierWay.hpp"
#include "CasinoLighting.hpp"
#include "CasinoPaintingConstants.hpp"
#include "CasinoSpriteConstants.hpp"
#include "DurationConversion.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "MotionPreference.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns Whether @p since falls on the first of each two steps of
 * @p step. */
bool onEvenStep(engine::Duration since, engine::Duration step) {
  return (since / step) % CASHIER_BLINK_PHASES == 0;
}

/** @brief Writes one way of the rate board, @p row from the top: its name and
 * its rate, lit when it is the way of the panel. */
void paintBoardRow(IInterfaceCanvas& canvas, int row, bool lit) {
  const int top =
      CASHIER_BOARD_FIRST_ROW_TOP + (row * CASHIER_BOARD_ROW_SPACING);
  const char* title = row == 0 ? BUY_CHIPS_TITLE : CASH_OUT_TITLE;
  const char* rate = row == 0 ? BUY_CHIPS_RATE : CASH_OUT_RATE;
  canvas.write(title, {CASHIER_BOARD_MIDDLE, top},
               TextStyle{.font = TextFont::Small,
                         .color = lit ? CASINO_LIGHT_GOLD : CASINO_DARK_GOLD,
                         .alignment = TextAlignment::Center});
  canvas.write(rate, {CASHIER_BOARD_MIDDLE, top + CASHIER_BOARD_RATE_DROP},
               TextStyle{.font = TextFont::Small,
                         .color = lit ? CASINO_PALE_CREAM : PALE_BLUE,
                         .alignment = TextAlignment::Center});
}

/** @brief Paints the rate board: EXCHANGE, then both ways, the way of the
 * panel lit. */
void paintRateBoard(IInterfaceCanvas& canvas, CashierWay way) {
  canvas.write(CASHIER_BOARD_TITLE,
               {CASHIER_BOARD_MIDDLE, CASHIER_BOARD_TITLE_TOP},
               TextStyle{.font = TextFont::Regular,
                         .color = STAR_GOLD,
                         .alignment = TextAlignment::Center});
  paintBoardRow(canvas, 0, way == CashierWay::BuyChips);
  paintBoardRow(canvas, 1, way == CashierWay::CashOut);
}

/** @brief Paints the glint crossing the glass of the chip rack, while it
 * crosses: a band of CASHIER_GLINT_WIDTH, leaning, row by row. */
void paintGlint(IInterfaceCanvas& canvas, engine::Duration since) {
  const engine::Duration into = since % CASHIER_GLINT_PERIOD;
  if (into >= CASHIER_GLINT_TIME) {
    return;
  }
  const sf::IntRect glass = CASHIER_RACK_GLASS;
  const float share =
      engine::inSeconds(into) / engine::inSeconds(CASHIER_GLINT_TIME);
  const int course = glass.size.x + (CASHIER_GLINT_LEAD * CASHIER_GLINT_LEADS);
  const int start = glass.position.x - CASHIER_GLINT_LEAD +
                    static_cast<int>(share * static_cast<float>(course));
  for (int row = 0; row < glass.size.y; ++row) {
    const int left = start - (CASHIER_GLINT_LEAN * row / glass.size.y);
    const sf::IntRect band({left, glass.position.y + row},
                           {CASHIER_GLINT_WIDTH, 1});
    if (const auto inside = band.findIntersection(glass)) {
      canvas.fill(*inside, CASHIER_GLINT_COLOR);
    }
  }
}

/** @brief Paints the coins of the vault that sparkle now: each of
 * CASHIER_SPARKLES, staggered, a coin it draws, brightening then fading. */
void paintSparkles(IInterfaceCanvas& canvas, std::span<const sf::Vector2i> gold,
                   engine::Duration since) {
  if (gold.empty()) {
    return;
  }
  for (std::size_t sparkle = 0; sparkle < CASHIER_SPARKLES; ++sparkle) {
    const engine::Duration shifted =
        since + (CASHIER_SPARKLE_STAGGER * static_cast<int>(sparkle));
    const auto window =
        static_cast<std::uint32_t>(shifted / CASHIER_SPARKLE_TIME);
    const sf::Vector2i coin =
        gold[scrambledOf((window * CASHIER_SPARKLES) +
                         static_cast<std::uint32_t>(sparkle)) %
             gold.size()];
    const float life = engine::inSeconds(shifted % CASHIER_SPARKLE_TIME) /
                       engine::inSeconds(CASHIER_SPARKLE_TIME);
    const float light =
        1.0F - (std::abs(life - CASHIER_SPARKLE_PEAK) / CASHIER_SPARKLE_PEAK);
    canvas.fill({coin, {1, 1}}, withShareOfLight(CASINO_FLASH_WHITE, light));
    const sf::Color arm =
        withShareOfLight(CASINO_WARM_CREAM, light * CASHIER_SPARKLE_ARM_SHARE);
    for (const sf::Vector2i offset : CASHIER_SPARKLE_ARMS) {
      canvas.fill({coin + offset, {1, 1}}, arm);
    }
  }
}

/** @brief Paints what moves in the cage: the bills, the glint, the light of
 * the camera and the sparkling coins. */
void paintRoomLife(IInterfaceCanvas& canvas, const CashierRoom& room,
                   engine::Duration since) {
  if (onEvenStep(since, CASHIER_BILLS_STEP)) {
    canvas.drawImage(
        TextureArea{.assetId = CASHIER_ROOM_ID,
                    .rectangle = CASHIER_BILLS_AREA},
        ImagePlacement{.topLeft = {CASHIER_BILLS_AREA.position.x,
                                   CASHIER_BILLS_AREA.position.y - 1}},
        INTERFACE_AREA);
  }
  paintGlint(canvas, since);
  if (onEvenStep(since, CASHIER_CAMERA_STEP)) {
    canvas.fill({CASHIER_CAMERA_LIGHT, {1, 1}}, CASINO_NEON_RED);
  }
  paintSparkles(canvas, room.gold, since);
}

/** @returns The frame of the teller @p moment: his first with less motion,
 * one of his serving while he serves, one of his counting otherwise. */
sf::IntRect tellerFrameAt(const CashierRoomMoment& moment) {
  int row = 0;
  int frame = 0;
  if (moment.motion == MotionPreference::Full) {
    if (tellerServesAt(moment.sinceServed)) {
      row = 1;
      frame = static_cast<int>(moment.sinceServed.value_or(engine::Duration{}) /
                               TELLER_SERVING_FRAME_TIME);
    } else {
      frame = static_cast<int>((moment.since / TELLER_COUNTING_FRAME_TIME) %
                               TELLER_FRAMES_PER_POSE);
    }
  }
  return {{frame * TELLER_FRAME_SIZE.x, row * TELLER_FRAME_SIZE.y},
          TELLER_FRAME_SIZE};
}

/** @brief Paints the teller, then the counter and the brass of the grille
 * over him. */
void paintTeller(IInterfaceCanvas& canvas, const CashierRoom& room,
                 const CashierRoomMoment& moment) {
  canvas.drawImage(TextureArea{.assetId = TELLER_SHEET_ID,
                               .rectangle = tellerFrameAt(moment)},
                   ImagePlacement{.topLeft = CASHIER_TELLER_TOP_LEFT},
                   INTERFACE_AREA);
  const sf::IntRect counter(
      {CASHIER_TELLER_TOP_LEFT.x, CASHIER_COUNTER_TOP},
      {TELLER_FRAME_SIZE.x, INTERFACE_AREA.size.y - CASHIER_COUNTER_TOP});
  canvas.drawImage(
      TextureArea{.assetId = CASHIER_ROOM_ID, .rectangle = counter},
      ImagePlacement{.topLeft = counter.position}, INTERFACE_AREA);
  canvas.plot(room.grille);
}

}  // namespace

bool tellerServesAt(std::optional<engine::Duration> sinceServed) {
  return sinceServed.has_value() &&
         *sinceServed < TELLER_SERVING_FRAME_TIME * TELLER_FRAMES_PER_POSE;
}

void paintCashierRoom(IInterfaceCanvas& canvas, const CashierRoom& room,
                      const CashierRoomMoment& moment) {
  canvas.drawImage(
      TextureArea{.assetId = CASHIER_ROOM_ID, .rectangle = INTERFACE_AREA},
      ImagePlacement{.topLeft = {0, 0}}, INTERFACE_AREA);
  paintRateBoard(canvas, moment.way);
  if (moment.motion == MotionPreference::Full) {
    paintRoomLife(canvas, room, moment.since);
  }
  paintTeller(canvas, room, moment);
}

}  // namespace rtype::client
