#include "MissionLogPage.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <random>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "BarRowWidth.hpp"
#include "Easing.hpp"
#include "EntitySpriteConstants.hpp"
#include "EntitySprites.hpp"
#include "FinalPilot.hpp"
#include "FinalRanking.hpp"
#include "FlightLogColorConstants.hpp"
#include "FlightLogLayoutConstants.hpp"
#include "FlightLogLook.hpp"
#include "FlightLogTextConstants.hpp"
#include "FlightLogTimingConstants.hpp"
#include "HudSpriteConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "MotionPreference.hpp"
#include "Outline.hpp"
#include "PaddedScore.hpp"
#include "PilotColor.hpp"
#include "PlayerNumberLabel.hpp"
#include "PlayerShading.hpp"
#include "ShadeSwap.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"
#include "TimeConstants.hpp"
#include "Typing.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

namespace {

/** @returns The scale the stamp is written at @p age after it lands: one
 * frame each of STAMP_SLAM_SCALES, then TITLE_SCALE; at once with reduced
 * motion. */
int stampScaleAt(engine::Duration age, MotionPreference motion) {
  const auto step = static_cast<std::size_t>(age / STAMP_SLAM_FRAME_TIME);
  if (motion == MotionPreference::Reduced || step >= STAMP_SLAM_SCALES.size()) {
    return TITLE_SCALE;
  }
  return STAMP_SLAM_SCALES[step];
}

/** @returns The pixels of the stamp the ink missed, STAMP_HOLE_COUNT of them
 * inside STAMP_BOX, drawn from @p seed. */
std::vector<sf::Vector2i> stampHolesOf(std::uint32_t seed) {
  std::mt19937 random(seed);
  std::uniform_int_distribution<int> column(0, STAMP_BOX.size.x - 1);
  std::uniform_int_distribution<int> row(0, STAMP_BOX.size.y - 1);
  std::vector<sf::Vector2i> holes;
  holes.reserve(STAMP_HOLE_COUNT);
  for (int hole = 0; hole < STAMP_HOLE_COUNT; ++hole) {
    holes.emplace_back(column(random), row(random));
  }
  return holes;
}

/** @brief Writes GAME OVER at @p scale, centered in the box of the stamp. */
void writeStamp(IInterfaceCanvas& canvas, sf::IntRect box, int scale) {
  canvas.write(STAMP_TEXT,
               {box.position.x + (box.size.x / SIDES_PER_AXIS),
                box.position.y + ((box.size.y - (TEXT_LINE_HEIGHT * scale)) /
                                  SIDES_PER_AXIS)},
               {.font = TextFont::Regular,
                .color = STAMP_COLOR,
                .alignment = TextAlignment::Center,
                .scale = scale});
}

/** @brief Paints the double frame of the stamp and the holes the ink
 * missed. */
void paintStampFrame(IInterfaceCanvas& canvas, sf::IntRect box) {
  Outline(box, Outline::Line::Solid).paint(canvas, STAMP_COLOR);
  Outline(
      sf::IntRect(box.position + sf::Vector2i(STAMP_FRAME_GAP, STAMP_FRAME_GAP),
                  box.size - sf::Vector2i(SIDES_PER_AXIS * STAMP_FRAME_GAP,
                                          SIDES_PER_AXIS * STAMP_FRAME_GAP)),
      Outline::Line::Solid)
      .paint(canvas, STAMP_COLOR);
  static const std::vector<sf::Vector2i> holes = stampHolesOf(STAMP_HOLES_SEED);
  for (const sf::Vector2i hole : holes) {
    canvas.fill(sf::IntRect(box.position + hole, {1, 1}), PAPER_COLOR);
  }
}

/** @brief The stamp once it landed: GAME OVER slamming in at a larger scale,
 * then at TITLE_SCALE with its frame. */
void paintStamp(IInterfaceCanvas& canvas, const FlightLogLook& look,
                sf::Vector2i page) {
  const engine::Duration age = look.sinceStart - look.schedule.stamped;
  if (age < engine::Duration::zero()) {
    return;
  }
  const int scale = stampScaleAt(age, look.motion);
  const sf::IntRect box(page + STAMP_BOX.position, STAMP_BOX.size);
  writeStamp(canvas, box, scale);
  if (scale == TITLE_SCALE) {
    paintStampFrame(canvas, box);
  }
}

void paintLines(IInterfaceCanvas& canvas, const FlightLogLook& look,
                sf::Vector2i page) {
  const engine::Duration sinceOpen = look.sinceStart - look.schedule.opened;
  canvas.write(typedPrefix(MISSION_LOG_TEXT,
                           lettersTypedAt(sinceOpen - MISSION_LOG_DELAY,
                                          MISSION_LOG_LETTER_TIME,
                                          MISSION_LOG_TEXT.size())),
               page + MISSION_LOG_TOP_LEFT,
               {.font = TextFont::Small, .color = PAPER_LABEL_COLOR});
  canvas.write(typedPrefix(DOWN_TEXT,
                           lettersTypedAt(sinceOpen - DOWN_DELAY,
                                          DOWN_LETTER_TIME, DOWN_TEXT.size())),
               {centeredTextLeft(sf::IntRect(page, PAGE_SIZE), DOWN_TEXT),
                page.y + DOWN_LINE_TOP},
               {.font = TextFont::Regular, .color = PAPER_INK});
}

void paintTeamScore(IInterfaceCanvas& canvas, const FlightLogLook& look,
                    sf::Vector2i page) {
  const engine::Duration sinceOpen = look.sinceStart - look.schedule.opened;
  canvas.write(
      TEAM_SCORE_TEXT,
      {page.x + (PAGE_SIZE.x / SIDES_PER_AXIS), page.y + TEAM_LABEL_TOP},
      {.font = TextFont::Small,
       .color = PAPER_LABEL_COLOR,
       .alignment = TextAlignment::Center});
  const float counted =
      easeOutCubic(progressBetween(sinceOpen, TEAM_DELAY, TEAM_COUNT_TIME));
  paintPaddedScore(canvas,
                   {.value = countedScore(teamScoreOf(look.pilots), counted),
                    .end = page + TEAM_SCORE_END,
                    .digitColor = PAPER_INK,
                    .zeroColor = PAPER_FAINT_COLOR,
                    .scale = TITLE_SCALE});
}

/** @brief Where the polaroid of a pilot lies, and the shades its ship and
 * marker swap for those of the palette. */
struct Polaroid {
  sf::Vector2i topLeft;
  std::optional<ShadeSwap> recolor = std::nullopt;
};

/** @brief Paints the polaroid of a pilot: its frame and window on the sky,
 * its ship, its marker, YOU or its number, and the tape, the ship and the
 * marker in the shades of the palette. */
void paintPolaroid(IInterfaceCanvas& canvas, const FinalPilot& pilot,
                   const Polaroid& polaroid) {
  const sf::Vector2i topLeft = polaroid.topLeft;
  const int color = pilotColorOfPlayerNumber(pilot.playerNumber);
  canvas.fill(sf::IntRect(topLeft, POLAROID_SIZE), PAPER_FAINT_COLOR);
  canvas.fill(
      sf::IntRect(topLeft + sf::Vector2i(1, 1),
                  POLAROID_SIZE - sf::Vector2i(SIDES_PER_AXIS, SIDES_PER_AXIS)),
      POLAROID_COLOR);
  canvas.fill(
      sf::IntRect(topLeft + POLAROID_WINDOW.position, POLAROID_WINDOW.size),
      POLAROID_WINDOW_COLOR);
  for (std::size_t star = 0; star < POLAROID_STARS.size(); ++star) {
    canvas.fill(sf::IntRect(topLeft + POLAROID_STARS[star], {1, 1}),
                POLAROID_STAR_COLORS[star]);
  }
  canvas.drawImage(TextureArea{.assetId = ENTITY_SHEET_ID,
                               .rectangle = rectangleOf(
                                   inPlayerColorRow(PLAYER_SHIP_FRAME, color))},
                   ImagePlacement{.topLeft = topLeft + POLAROID_SHIP,
                                  .recolor = polaroid.recolor},
                   INTERFACE_AREA);
  canvas.drawImage(TextureArea{.assetId = HUD_SHEET_ID,
                               .rectangle = rectangleOf(MARKER_FRAMES.at(
                                   static_cast<std::size_t>(color)))},
                   ImagePlacement{.topLeft = topLeft + POLAROID_MARKER,
                                  .recolor = polaroid.recolor},
                   INTERFACE_AREA);
  canvas.write(pilot.isLocalPlayer ? std::string{LOCAL_PLAYER_TAG}
                                   : playerNumberLabel(pilot.playerNumber),
               topLeft + POLAROID_CAPTION,
               {.font = TextFont::Small, .color = PAPER_INK});
  canvas.fill(sf::IntRect(topLeft + POLAROID_TAPE.position, POLAROID_TAPE.size),
              TAPE_COLOR);
}

/** @returns Where the polaroid in slot @p slot lies on @p page: one polaroid
 * per pilot of @p look, side by side, centered on the page. */
sf::Vector2i polaroidTopLeft(const FlightLogLook& look, sf::Vector2i page,
                             int slot) {
  const int count = static_cast<int>(look.pilots.size());
  const int width = barRowWidth(count, POLAROID_SIZE, POLAROID_GAP);
  return {page.x + ((PAGE_SIZE.x - width) / SIDES_PER_AXIS) +
              (slot * (POLAROID_SIZE.x + POLAROID_GAP)),
          page.y + POLAROID_TOP};
}

/** @returns How many rows above its place a polaroid shown @p sinceShown ago
 * still lies: POLAROID_DROP at first, none once it landed or with reduced
 * motion. */
int polaroidDropAt(engine::Duration sinceShown, MotionPreference motion) {
  if (motion == MotionPreference::Reduced) {
    return 0;
  }
  const float landed =
      easeOutCubic(progressBetween(sinceShown, {}, PHOTO_DROP_TIME));
  return pixelsOf(POLAROID_DROP, 1.0F - landed);
}

/** @returns The pilots in the order of their numbers, the order of the
 * polaroids. */
std::vector<FinalPilot> pilotsByNumber(std::span<const FinalPilot> pilots) {
  std::vector<FinalPilot> numbered(pilots.begin(), pilots.end());
  std::ranges::sort(numbered, {}, &FinalPilot::playerNumber);
  return numbered;
}

/** @returns How long ago the polaroid in slot @p slot started to drop in,
 * PHOTO_STEP after the one before it; negative before that. */
engine::Duration sinceShownOf(const FlightLogLook& look, int slot) {
  return look.sinceStart - look.schedule.opened - PHOTOS_DELAY -
         (PHOTO_STEP * static_cast<engine::Duration::rep>(slot));
}

void paintPolaroids(IInterfaceCanvas& canvas, const FlightLogLook& look,
                    sf::Vector2i page) {
  const std::vector<FinalPilot> numbered = pilotsByNumber(look.pilots);
  for (int slot = 0; std::cmp_less(slot, numbered.size()); ++slot) {
    const engine::Duration sinceShown = sinceShownOf(look, slot);
    if (sinceShown < engine::Duration::zero()) {
      return;
    }
    const FinalPilot& pilot = numbered[static_cast<std::size_t>(slot)];
    paintPolaroid(
        canvas, pilot,
        Polaroid{
            .topLeft = polaroidTopLeft(look, page, slot) -
                       sf::Vector2i(0, polaroidDropAt(sinceShown, look.motion)),
            .recolor = shadingOf(look.palette, static_cast<std::size_t>(
                                                   pilotColorOfPlayerNumber(
                                                       pilot.playerNumber)))
                           .recolor});
  }
}

}  // namespace

void paintMissionLogPage(IInterfaceCanvas& canvas, const FlightLogLook& look,
                         sf::Vector2i page) {
  paintLines(canvas, look, page);
  paintStamp(canvas, look, page);
  if (look.sinceStart - look.schedule.opened >= TEAM_DELAY) {
    paintTeamScore(canvas, look, page);
  }
  paintPolaroids(canvas, look, page);
}

}  // namespace rtype::client
