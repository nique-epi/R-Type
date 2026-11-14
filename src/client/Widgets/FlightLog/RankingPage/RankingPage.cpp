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
#include "FlightLogPages.hpp"
#include "FlightLogSheetConstants.hpp"
#include "FlightLogTextConstants.hpp"
#include "FlightLogTimeline.hpp"
#include "FlightLogTimingConstants.hpp"
#include "HudSpriteConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InkTag.hpp"
#include "InterfaceConstants.hpp"
#include "MotionPreference.hpp"
#include "Outline.hpp"
#include "PaddedScore.hpp"
#include "PilotColor.hpp"
#include "PilotName.hpp"
#include "PlayerShading.hpp"
#include "RankingLayout.hpp"
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
  RankingLayout layout;
  sf::Vector2i topLeft;
  engine::Duration intoRow{};
  MotionPreference motion{};
  int leaderLeft = LEADER_LEFT;
  std::optional<ShadeSwap> markerRecolor = std::nullopt;
};

/** @returns How wide the ink behind YOU is. */
int youTagWidthOf(const IInterfaceCanvas& canvas) {
  return inkTagWidth(canvas, LOCAL_PLAYER_TAG);
}

/** @returns Where the leader of the row of @p pilot starts: after its marker,
 * or after YOU for this client. */
int leaderLeftOf(const IInterfaceCanvas& canvas, const FinalPilot& pilot) {
  if (!pilot.isLocalPlayer) {
    return LEADER_LEFT;
  }
  return rankingLayoutOf(canvas.textMetrics()).youTagOffset.x +
         youTagWidthOf(canvas) + LEADER_AFTER_TAG;
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
  const RankingLayout layout = rankingLayoutOf(canvas.textMetrics());
  canvas.fill(sf::IntRect(page + layout.ruleTopLeft,
                          {pixelsOf(layout.lineWidth, drawn), 1}),
              PAPER_LABEL_COLOR);
}

/** @brief YOU on ink, in the row of @p place. */
void paintYouTag(IInterfaceCanvas& canvas, const RowPlace& place) {
  paintInkTag(canvas,
              InkTag{.word = LOCAL_PLAYER_TAG,
                     .topLeft = place.topLeft + place.layout.youTagOffset,
                     .wordColor = YOU_TAG_TEXT_COLOR});
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
      ImagePlacement{.topLeft = place.topLeft + place.layout.markerOffset,
                     .recolor = place.markerRecolor},
      INTERFACE_AREA);
  if (pilot.isLocalPlayer) {
    paintYouTag(canvas, place);
  }
}

/** @brief The dotted leader, the score counting up, and the dashed separator
 * under the row. */
void paintRowScore(IInterfaceCanvas& canvas, const FinalPilot& pilot,
                   const RowPlace& place) {
  const float leader =
      progressBetween(place.intoRow, LEADER_DELAY, LEADER_TIME);
  const RankingLayout& layout = place.layout;
  const int leaderWidth =
      pixelsOf(layout.leaderRight - place.leaderLeft, leader);
  if (leaderWidth > 0) {
    Outline(sf::IntRect(place.topLeft +
                            sf::Vector2i(place.leaderLeft, layout.leaderRow),
                        {leaderWidth, 1}),
            Outline::Line::Dashed)
        .paint(canvas, PAPER_FAINT_COLOR);
  }
  if (place.intoRow >= LEADER_DELAY) {
    const float counted = easeOutCubic(
        progressBetween(place.intoRow, LEADER_DELAY, ROW_COUNT_TIME));
    paintPaddedScore(canvas, {.value = countedScore(pilot.score, counted),
                              .end = place.topLeft + layout.scoreEnd,
                              .digitColor = PAPER_INK,
                              .zeroColor = PAPER_FAINT_COLOR});
  }
  Outline(sf::IntRect(place.topLeft + sf::Vector2i(layout.ruleTopLeft.x,
                                                   layout.separatorRow),
                      {layout.lineWidth, 1}),
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
  canvas.drawImage(
      TextureArea{.assetId = FLIGHT_LOG_SHEET_ID,
                  .rectangle = rectangleOf(MEDAL_FRAME)},
      ImagePlacement{.topLeft = place.topLeft + place.layout.medalOffset -
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

/** @returns Where the row @p row of the page of @p layout, whose top-left
 * corner is @p page, is written, and how long ago it started; its leader
 * starts at LEADER_LEFT. */
RowPlace rowPlaceOf(const FlightLogLook& look, const RowPlace& page,
                    std::size_t row) {
  return RowPlace{
      .layout = page.layout,
      .topLeft = page.topLeft + sf::Vector2i(0, page.layout.firstRowTop +
                                                    (static_cast<int>(row) *
                                                     page.layout.rowPitch)),
      .intoRow = look.sinceStart - rowStartOf(look.schedule, row),
      .motion = look.motion};
}

}  // namespace

void paintRankingPage(IInterfaceCanvas& canvas, const FlightLogLook& look,
                      sf::Vector2i page) {
  paintHeading(canvas, look.sinceStart - look.schedule.opened, page);
  const std::size_t rows = rowsStartedOf(look);
  const RowPlace pagePlace{.layout = rankingLayoutOf(canvas.textMetrics()),
                           .topLeft = page};
  for (std::size_t row = 0; row < rows; ++row) {
    const FinalPilot& pilot = look.pilots[row];
    RowPlace place = rowPlaceOf(look, pagePlace, row);
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
