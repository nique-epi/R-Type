#include "FlightLogPages.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include "FlightLogLayoutConstants.hpp"
#include "InterfaceConstants.hpp"
#include "MissionLogLayout.hpp"
#include "RankingLayout.hpp"
#include "StatusBandConstants.hpp"
#include "TextMeasures.hpp"
#include "TextMetrics.hpp"
#include "TextStyle.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The top of the score of the team on the wide page: under its
 * label, never above the row it was drawn on. */
int teamScoreTopOf(int teamLabelTop, int lineHeight) {
  return std::max(TEAM_SCORE_END.y,
                  teamLabelTop + lineHeight + WIDE_PAGE_LINE_GAP);
}

/** @returns The box of the stamp on the wide page: centered on it, under
 * MISSION LOG, never above the row it was drawn on. */
sf::IntRect wideStampBoxOf(int lineHeight) {
  return {{(WIDE_PAGE_WIDTH - STAMP_BOX.size.x) / SIDES_PER_AXIS,
           std::max(STAMP_BOX.position.y,
                    MISSION_LOG_TOP_LEFT.y + lineHeight + WIDE_PAGE_LINE_GAP)},
          STAMP_BOX.size};
}

/** @returns The ranking as the book was drawn at the size the fonts are
 * drawn at, on its page of PAGE_SIZE. */
RankingLayout drawnRankingLayout() {
  return {.ruleTopLeft = RANKING_RULE_TOP_LEFT,
          .lineWidth = RANKING_LINE_WIDTH,
          .firstRowTop = FIRST_ROW_TOP,
          .rowPitch = RANKING_ROW_PITCH,
          .markerOffset = PILOT_MARKER_OFFSET,
          .youTagOffset = YOU_TAG_OFFSET,
          .leaderRight = LEADER_RIGHT,
          .leaderRow = LEADER_ROW,
          .scoreEnd = ROW_SCORE_END,
          .separatorRow = ROW_SEPARATOR_ROW,
          .medalOffset = MEDAL_OFFSET};
}

}  // namespace

MissionLogLayout missionLogLayoutOf(TextMetrics metrics) {
  if (metrics == TextMetrics{}) {
    return {.width = PAGE_SIZE.x,
            .stampBox = STAMP_BOX,
            .summaryTop = DOWN_LINE_TOP,
            .teamLabelTop = TEAM_LABEL_TOP,
            .teamScoreEnd = TEAM_SCORE_END,
            .polaroidTop = POLAROID_TOP,
            .caption = POLAROID_CAPTION,
            .captionAlignment = TextAlignment::Left};
  }
  const int lineHeight = lineHeightOf(metrics);
  const sf::IntRect stamp = wideStampBoxOf(lineHeight);
  const int summaryTop = std::max(
      DOWN_LINE_TOP, stamp.position.y + stamp.size.y + WIDE_PAGE_PART_GAP);
  const int teamLabelTop =
      std::max(TEAM_LABEL_TOP, summaryTop + lineHeight + WIDE_PAGE_PART_GAP);
  const int scoreTop = teamScoreTopOf(teamLabelTop, lineHeight);
  return {
      .width = WIDE_PAGE_WIDTH,
      .stampBox = stamp,
      .summaryTop = summaryTop,
      .teamLabelTop = teamLabelTop,
      .teamScoreEnd = {(WIDE_PAGE_WIDTH / SIDES_PER_AXIS) +
                           (TEAM_SCORE_END.x - (PAGE_SIZE.x / SIDES_PER_AXIS)),
                       scoreTop},
      .polaroidTop =
          std::max(POLAROID_TOP, scoreTop + (TEXT_LINE_HEIGHT * TITLE_SCALE) +
                                     (SIDES_PER_AXIS * WIDE_PAGE_PART_GAP)),
      .caption = {POLAROID_SIZE.x / SIDES_PER_AXIS,
                  POLAROID_SIZE.y + WIDE_PAGE_LINE_GAP},
      .captionAlignment = TextAlignment::Center};
}

RankingLayout rankingLayoutOf(TextMetrics metrics) {
  if (metrics == TextMetrics{}) {
    return drawnRankingLayout();
  }
  const int lineHeight = lineHeightOf(metrics);
  const sf::Vector2i rule(
      RANKING_RULE_TOP_LEFT.x,
      std::max(RANKING_RULE_TOP_LEFT.y,
               RANKING_HEADING_TOP_LEFT.y + lineHeight + WIDE_PAGE_LINE_GAP));
  const int lineWidth = WIDE_PAGE_WIDTH - (SIDES_PER_AXIS * rule.x);
  const int secondLine = PILOT_NAME_OFFSET.y + lineHeight + 1;
  const sf::Vector2i scoreEnd(rule.x + lineWidth, secondLine);
  const int separatorRow = secondLine + lineHeight + 1;
  return {.ruleTopLeft = rule,
          .lineWidth = lineWidth,
          .firstRowTop = rule.y + (FIRST_ROW_TOP - RANKING_RULE_TOP_LEFT.y),
          .rowPitch = separatorRow + WIDE_RANKING_ROW_GAP,
          .markerOffset = {PILOT_MARKER_OFFSET.x, secondLine},
          .youTagOffset = {YOU_TAG_OFFSET.x, secondLine - 1},
          .leaderRight = scoreEnd.x - (STATUS_SCORE_DIGITS * lineHeight) -
                         LEADER_SCORE_GAP,
          .leaderRow = secondLine +
                       ((lineHeight * LEADER_ROW_IN_LINE) / TEXT_LINE_HEIGHT),
          .scoreEnd = scoreEnd,
          .separatorRow = separatorRow,
          .medalOffset = {scoreEnd.x - (ROW_SCORE_END.x - MEDAL_OFFSET.x),
                          MEDAL_OFFSET.y}};
}

}  // namespace rtype::client
