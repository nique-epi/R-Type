#include "RankingPage.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include <string>
#include "Easing.hpp"
#include "EntitySprites.hpp"
#include "FinalPilot.hpp"
#include "FlightLogColorConstants.hpp"
#include "FlightLogLayoutConstants.hpp"
#include "FlightLogLook.hpp"
#include "FlightLogSheetConstants.hpp"
#include "FlightLogTextConstants.hpp"
#include "FlightLogTimeline.hpp"
#include "FlightLogTimingConstants.hpp"
#include "HudSpriteConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "MotionPreference.hpp"
#include "Outline.hpp"
#include "PaddedScore.hpp"
#include "PilotColor.hpp"
#include "PilotName.hpp"
#include "PlayerShading.hpp"
#include "ShadeSwap.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"
#include "TimeConstants.hpp"
#include "Typing.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @brief A row being written: its top-left corner, how long ago it started,
 * whether things may move, where its dotted leader starts, and the shades its
 * marker swaps for those of the palette. */
struct RowPlace {
  sf::Vector2i topLeft;
  engine::Duration intoRow{};
  MotionPreference motion{};
  int leaderLeft = LEADER_LEFT;
  std::optional<ShadeSwap> markerRecolor = std::nullopt;
};

/** @returns How wide the ink behind YOU is. */
int youTagWidthOf(const IInterfaceCanvas& canvas) {
  return canvas.textWidth(LOCAL_PLAYER_TAG, TextFont::Small) + YOU_TAG_PADDING;
}

/** @returns Where the leader of the row of @p pilot starts: after its marker,
 * or after YOU for this client. */
int leaderLeftOf(const IInterfaceCanvas& canvas, const FinalPilot& pilot) {
  if (!pilot.isLocalPlayer) {
    return LEADER_LEFT;
  }
  return YOU_TAG_OFFSET.x + youTagWidthOf(canvas) + LEADER_AFTER_TAG;
}

void paintHeading(IInterfaceCanvas& canvas, engine::Duration sinceOpen,
                  sf::Vector2i page) {
  canvas.write(
      typedPrefix(RANKING_TEXT,
                  lettersTypedAt(sinceOpen - RANKING_DELAY, RANKING_LETTER_TIME,
                                 RANKING_TEXT.size())),
      page + RANKING_HEADING_TOP_LEFT,
      {.font = TextFont::Regular, .color = PAPER_INK});
  const engine::Duration ruleStart =
      RANKING_DELAY + typingTimeOf(RANKING_LETTER_TIME, RANKING_TEXT.size());
  const float drawn =
      easeOutCubic(progressBetween(sinceOpen, ruleStart, RULE_DRAW_TIME));
  canvas.fill(sf::IntRect(page + RANKING_RULE_TOP_LEFT,
                          {pixelsOf(RANKING_LINE_WIDTH, drawn), 1}),
              PAPER_LABEL_COLOR);
}

/** @brief YOU on ink, in the row whose top-left corner is @p rowTopLeft. */
void paintYouTag(IInterfaceCanvas& canvas, sf::Vector2i rowTopLeft) {
  canvas.fill(sf::IntRect(rowTopLeft + YOU_TAG_OFFSET,
                          {youTagWidthOf(canvas), YOU_TAG_HEIGHT}),
              YOU_TAG_COLOR);
  canvas.write(LOCAL_PLAYER_TAG, rowTopLeft + YOU_TAG_OFFSET + YOU_TEXT_OFFSET,
               {.font = TextFont::Small, .color = YOU_TAG_TEXT_COLOR});
}

/** @brief The rank, slamming in at one scale more for DIGIT_SLAM_TIME, the
 * name typed, the marker, and YOU on ink for this client. */
void paintRowName(IInterfaceCanvas& canvas, const FinalPilot& pilot,
                  const RowPlace& place) {
  const int scale =
      place.intoRow < DIGIT_SLAM_TIME && place.motion == MotionPreference::Full
          ? TITLE_SCALE + 1
          : TITLE_SCALE;
  const int shift = (scale - TITLE_SCALE) * SLAM_SHIFT;
  canvas.write(std::to_string(pilot.rank),
               place.topLeft + RANK_DIGIT_OFFSET - sf::Vector2i(shift, shift),
               {.font = TextFont::Regular, .color = PAPER_INK, .scale = scale});
  const std::string name = pilotNameOf(pilot.nickname, pilot.playerNumber);
  canvas.write(
      typedPrefix(name, lettersTypedAt(place.intoRow - NAME_DELAY,
                                       NAME_LETTER_TIME, letterCountOf(name))),
      place.topLeft + PILOT_NAME_OFFSET,
      {.font = TextFont::Regular, .color = PAPER_INK});
  canvas.drawImage(
      TextureArea{
          .assetId = HUD_SHEET_ID,
          .rectangle = rectangleOf(MARKER_FRAMES.at(static_cast<std::size_t>(
              pilotColorOfPlayerNumber(pilot.playerNumber))))},
      ImagePlacement{.topLeft = place.topLeft + PILOT_MARKER_OFFSET,
                     .recolor = place.markerRecolor},
      INTERFACE_AREA);
  if (pilot.isLocalPlayer) {
    paintYouTag(canvas, place.topLeft);
  }
}

/** @brief The dotted leader, the score counting up, and the dashed separator
 * under the row. */
void paintRowScore(IInterfaceCanvas& canvas, const FinalPilot& pilot,
                   const RowPlace& place) {
  const float leader =
      progressBetween(place.intoRow, LEADER_DELAY, LEADER_TIME);
  const int leaderWidth = pixelsOf(LEADER_RIGHT - place.leaderLeft, leader);
  if (leaderWidth > 0) {
    Outline(
        sf::IntRect(place.topLeft + sf::Vector2i(place.leaderLeft, LEADER_ROW),
                    {leaderWidth, 1}),
        Outline::Line::Dashed)
        .paint(canvas, PAPER_FAINT_COLOR);
  }
  if (place.intoRow >= LEADER_DELAY) {
    const float counted = easeOutCubic(
        progressBetween(place.intoRow, LEADER_DELAY, ROW_COUNT_TIME));
    paintPaddedScore(canvas, {.value = countedScore(pilot.score, counted),
                              .end = place.topLeft + ROW_SCORE_END,
                              .digitColor = PAPER_INK,
                              .zeroColor = PAPER_FAINT_COLOR});
  }
  Outline(sf::IntRect(place.topLeft + sf::Vector2i(RANKING_RULE_TOP_LEFT.x,
                                                   ROW_SEPARATOR_ROW),
                      {RANKING_LINE_WIDTH, 1}),
          Outline::Line::Dashed)
      .paint(canvas, PAPER_FAINT_COLOR);
}

/** @brief The medal of a first pilot, once its row is written, dropping in
 * with an overshoot unless motion is reduced. */
void paintMedal(IInterfaceCanvas& canvas, const RowPlace& place) {
  const float landed =
      place.motion == MotionPreference::Reduced
          ? 1.0F
          : easeOutBack(progressBetween(place.intoRow, ROW_TIME, MEDAL_TIME));
  const int drop = pixelsOf(MEDAL_DROP, 1.0F - landed);
  canvas.drawImage(TextureArea{.assetId = FLIGHT_LOG_SHEET_ID,
                               .rectangle = rectangleOf(MEDAL_FRAME)},
                   ImagePlacement{.topLeft = place.topLeft + MEDAL_OFFSET -
                                             sf::Vector2i(0, drop)},
                   INTERFACE_AREA);
}

/** @brief Writes a row: its name, its score, and the medal of a first pilot
 * once the row is written. */
void paintRow(IInterfaceCanvas& canvas, const FinalPilot& pilot,
              const RowPlace& place) {
  paintRowName(canvas, pilot, place);
  paintRowScore(canvas, pilot, place);
  if (pilot.rank == MEDAL_RANK && place.intoRow >= ROW_TIME) {
    paintMedal(canvas, place);
  }
}

/** @returns How many rows have started to be written, best first. */
std::size_t rowsStartedOf(const FlightLogLook& look) {
  std::size_t rows = 0;
  while (rows < look.pilots.size() &&
         look.sinceStart >= rowStartOf(look.schedule, rows)) {
    ++rows;
  }
  return rows;
}

/** @returns Where the row @p row of @p page is written, and how long ago it
 * started; its leader starts at LEADER_LEFT. */
RowPlace rowPlaceOf(const FlightLogLook& look, sf::Vector2i page,
                    std::size_t row) {
  return RowPlace{
      .topLeft = page + sf::Vector2i(0, FIRST_ROW_TOP + (static_cast<int>(row) *
                                                         RANKING_ROW_PITCH)),
      .intoRow = look.sinceStart - rowStartOf(look.schedule, row),
      .motion = look.motion};
}

}  // namespace

void paintRankingPage(IInterfaceCanvas& canvas, const FlightLogLook& look,
                      sf::Vector2i page) {
  paintHeading(canvas, look.sinceStart - look.schedule.opened, page);
  const std::size_t rows = rowsStartedOf(look);
  for (std::size_t row = 0; row < rows; ++row) {
    const FinalPilot& pilot = look.pilots[row];
    RowPlace place = rowPlaceOf(look, page, row);
    place.leaderLeft = leaderLeftOf(canvas, pilot);
    place.markerRecolor =
        shadingOf(look.palette,
                  static_cast<std::size_t>(
                      pilotColorOfPlayerNumber(pilot.playerNumber)))
            .recolor;
    paintRow(canvas, pilot, place);
  }
}

}  // namespace rtype::client
