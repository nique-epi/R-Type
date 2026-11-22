#include "HallPainter.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <span>
#include <utility>
#include "BackgroundColorConstants.hpp"
#include "CasinoHallConstants.hpp"
#include "CasinoLighting.hpp"
#include "CasinoPainting.hpp"
#include "CasinoPaintingConstants.hpp"
#include "CasinoSpriteConstants.hpp"
#include "DurationConversion.hpp"
#include "HallMoment.hpp"
#include "HallPicture.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "MotionPreference.hpp"
#include "RenderingConstants.hpp"
#include "Spectator.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"
#include "TimeConstants.hpp"
#include "TwinkleGroup.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @brief Paints the cone of the spotlight over the ring, one row at a time,
 * its light breathing. */
void paintSpotlight(IInterfaceCanvas& canvas, engine::Duration since) {
  const float alpha =
      HALL_SPOT_ALPHA +
      (std::sin(engine::inSeconds(since) * HALL_SPOT_PACE) * HALL_SPOT_BREATH);
  const sf::Color light(CASINO_WARM_CREAM.r, CASINO_WARM_CREAM.g,
                        CASINO_WARM_CREAM.b, static_cast<std::uint8_t>(alpha));
  const int rows = HALL_SPOT_FLOOR - HALL_SPOT_TOP_MIDDLE.y;
  const int widening = HALL_SPOT_FLOOR_HALF_WIDTH - HALL_SPOT_TOP_HALF_WIDTH;
  for (int row = 0; row < rows; ++row) {
    const int half = HALL_SPOT_TOP_HALF_WIDTH + (widening * row / rows);
    canvas.fill({{HALL_SPOT_TOP_MIDDLE.x - half, HALL_SPOT_TOP_MIDDLE.y + row},
                 {half * SIDES_PER_AXIS, 1}},
                light);
  }
}

/** @brief Paints the bulbs out at this step of the chase, the crystals that
 * dim now, and the reel window of the slot machines lit now. */
void paintLights(IInterfaceCanvas& canvas, const HallPicture& picture,
                 engine::Duration since) {
  canvas.plot(picture.bulbs.unlitAt(-chaseStepAt(since)));
  for (const TwinkleGroup& crystals : picture.crystals) {
    if (isDimmedAt(crystals, since)) {
      canvas.plot(crystals.pixels);
    }
  }
  const auto lit = static_cast<std::size_t>(
      (since / HALL_SLOT_STEP) %
      static_cast<engine::Duration::rep>(HALL_SLOT_CYCLE_STEPS));
  if (lit < HALL_SLOT_WINDOWS.size()) {
    canvas.fill(HALL_SLOT_WINDOWS.at(lit), HALL_LIT_SLOT);
  }
}

/** @returns The frame the boxers show @p moment: on guard, the first, with
 * less motion. */
int boxerFrameAt(HallMoment moment) {
  if (moment.motion == MotionPreference::Reduced) {
    return 0;
  }
  return static_cast<int>((moment.since / HALL_BOXER_FRAME_TIME) %
                          HALL_BOXER_FRAMES);
}

/** @brief Paints the boxers at @p moment, and the spark of the punch on the
 * frames it lands. */
void paintBoxers(IInterfaceCanvas& canvas, HallMoment moment) {
  const int frame = boxerFrameAt(moment);
  canvas.drawImage(
      TextureArea{.assetId = HALL_BOXERS_SHEET_ID,
                  .rectangle = {{frame * HALL_BOXER_FRAME_SIZE.x, 0},
                                HALL_BOXER_FRAME_SIZE}},
      ImagePlacement{.topLeft = HALL_BOXERS_TOP_LEFT}, INTERFACE_AREA);
  if (std::ranges::find(HALL_PUNCH_FRAMES, frame) == HALL_PUNCH_FRAMES.end()) {
    return;
  }
  const sf::Vector2i spark = HALL_BOXERS_TOP_LEFT + HALL_SPARK_OFFSET;
  canvas.fill({spark, {1, 1}}, CASINO_FLASH_WHITE);
  for (const sf::Vector2i offset : HALL_SPARK_GOLD_OFFSETS) {
    canvas.fill({spark + offset, {1, 1}}, STAR_GOLD);
  }
}

/** @returns How many pixels a spectator stands up at @p since: 0, 1 or 2. */
std::size_t hopOf(const Spectator& spectator, engine::Duration since) {
  const float beat =
      std::sin((engine::inSeconds(since) * spectator.speed) + spectator.phase);
  if (beat > HALL_HOP_TWO_PIXELS) {
    return HALL_HIGHEST_HOP;
  }
  return beat > HALL_HOP_ONE_PIXEL ? 1 : 0;
}

/** @brief Paints each spectator raised by its hop; one at rest is the
 * picture itself. */
void paintCrowd(IInterfaceCanvas& canvas, std::span<const Spectator> crowd,
                engine::Duration since) {
  for (const Spectator& spectator : crowd) {
    const std::size_t hop = hopOf(spectator, since);
    if (hop > 0) {
      canvas.plot(spectator.raised.at(hop - 1));
    }
  }
}

/** @brief Paints the camera flash of the window of time @p since falls in,
 * when that window holds one: a white point and its four arms, fading. */
void paintFlash(IInterfaceCanvas& canvas, engine::Duration since) {
  const auto window = static_cast<std::uint32_t>(since / HALL_FLASH_TIME);
  const std::uint32_t draw = scrambledOf(window);
  if (draw % HALL_FLASH_ODDS != 0) {
    return;
  }
  const sf::Vector2i flash(
      HALL_FLASH_AREA.position.x +
          static_cast<int>(scrambledOf(draw) %
                           static_cast<std::uint32_t>(HALL_FLASH_AREA.size.x)),
      HALL_FLASH_AREA.position.y +
          static_cast<int>(draw %
                           static_cast<std::uint32_t>(HALL_FLASH_AREA.size.y)));
  const float left = 1.0F - (engine::inSeconds(since % HALL_FLASH_TIME) /
                             engine::inSeconds(HALL_FLASH_TIME));
  canvas.fill({flash, {1, 1}}, withShareOfLight(CASINO_FLASH_WHITE, left));
  const sf::Color arms = withShareOfLight(CASINO_FLASH_WHITE, left * HALF);
  canvas.fill({{flash.x - 1, flash.y}, {HALL_FLASH_SPAN, 1}}, arms);
  canvas.fill({{flash.x, flash.y - 1}, {1, HALL_FLASH_SPAN}}, arms);
}

/** @brief Paints the two chevrons that push toward the roulette, nudged a
 * pixel every other HALL_CHEVRON_STEP. */
void paintChevrons(IInterfaceCanvas& canvas, engine::Duration since) {
  const int nudge =
      static_cast<int>((since / HALL_CHEVRON_STEP) % HALL_CHEVRON_NUDGE_STEPS);
  for (int chevron = 0; chevron < HALL_CHEVRON_COUNT; ++chevron) {
    const sf::Color color = chevron == 0 ? CASINO_LIGHT_GOLD : STAR_GOLD;
    const int left =
        HALL_CHEVRONS_TOP_LEFT.x + nudge + (chevron * HALL_CHEVRON_GAP);
    for (int row = 0; row < HALL_CHEVRON_ROWS; ++row) {
      canvas.fill({{left + row, HALL_CHEVRONS_TOP_LEFT.y + row},
                   {1, (HALL_CHEVRON_ROWS * SIDES_PER_AXIS) - 1 -
                           (row * SIDES_PER_AXIS)}},
                  color);
    }
  }
}

/** @brief Paints the title of the hall on its cartouche. */
void paintTitle(IInterfaceCanvas& canvas) {
  canvas.fill(HALL_TITLE_AREA, CASINO_PANEL_INSIDE_COLOR);
  paintEdges(canvas, HALL_TITLE_AREA, CASINO_GOLD);
  canvas.write(HALL_TITLE.in(canvas.language()), HALL_TITLE_TOP_MIDDLE,
               TextStyle{.font = TextFont::Regular,
                         .color = STAR_GOLD,
                         .alignment = TextAlignment::Center});
}

/** @brief Paints what moves in the hall, all of it but the boxers. */
void paintLife(IInterfaceCanvas& canvas, const HallPicture& picture,
               engine::Duration since) {
  paintCrowd(canvas, picture.spectators, since);
  paintFlash(canvas, since);
  paintChevrons(canvas, since);
}

}  // namespace

HallPainter::HallPainter(HallPicture picture) : picture_(std::move(picture)) {}

void HallPainter::paint(IInterfaceCanvas& canvas, HallMoment moment) {
  canvas.drawImage(
      TextureArea{.assetId = HALL_PICTURE_ID, .rectangle = INTERFACE_AREA},
      ImagePlacement{.topLeft = {0, 0}}, INTERFACE_AREA);
  const bool moving = moment.motion == MotionPreference::Full;
  if (moving) {
    paintSpotlight(canvas, moment.since);
    swingBeams(beamPixels_, HALL_CEILING_BEAMS, moment.since);
    canvas.plot(beamPixels_);
    paintLights(canvas, picture_, moment.since);
  }
  paintBoxers(canvas, moment);
  canvas.plot(picture_.frontRopes);
  if (moving) {
    paintLife(canvas, picture_, moment.since);
  }
  paintTitle(canvas);
}

}  // namespace rtype::client
