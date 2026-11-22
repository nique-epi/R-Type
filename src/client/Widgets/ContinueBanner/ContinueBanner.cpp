#include "ContinueBanner.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <optional>
#include <string>
#include "ContinueBannerConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceConstants.hpp"
#include "Language.hpp"
#include "MissionBannerConstants.hpp"
#include "TextMark.hpp"
#include "TextMarks.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

namespace {

/** @brief A line of the banner: its text, its font and its color. */
struct BannerLine {
  std::string text;
  TextFont font{};
  sf::Color color;
};

/** @returns The column the banner is centered on. */
int middleColumn() { return MISSION_BANNER_AREA.getCenter().x; }

/** @returns The row of the first line of the banner: its two lines and the
 * gap between them are centered in MISSION_BANNER_AREA. */
int firstLineTop() {
  const int height =
      (CONTINUE_BANNER_LINES * TEXT_LINE_HEIGHT) + CONTINUE_BANNER_LINE_GAP;
  return MISSION_BANNER_AREA.position.y +
         ((MISSION_BANNER_AREA.size.y - height) / SIDES_PER_AXIS);
}

/** @returns The second line of the banner for @p continuesLeft, in
 * @p language. */
BannerLine secondLineOf(std::uint8_t continuesLeft, Language language) {
  if (continuesLeft == 0) {
    return BannerLine{.text = std::string(NO_CONTINUE_LEFT_TEXT.in(language)),
                      .font = TextFont::Small,
                      .color = NO_CONTINUE_LEFT_COLOR};
  }
  const std::string count = std::to_string(continuesLeft);
  return BannerLine{
      .text = withMark(std::string(CONTINUES_LEFT.in(language)),
                       TextMark{.mark = CONTINUE_COUNT_MARK, .value = count}),
      .font = TextFont::Small,
      .color = CONTINUES_LEFT_COLOR};
}

/** @brief Paints one rule of the banner, centered, on @p row. */
void paintRule(IInterfaceCanvas& canvas, int row) {
  canvas.fill(
      centeredRule(MISSION_BANNER_AREA, row, MISSION_BANNER_RULE_HALF_WIDTH),
      CONTINUE_BANNER_RULE_COLOR);
}

/** @brief Writes @p line centered on the banner, outlined, from @p top. */
void writeLine(IInterfaceCanvas& canvas, const BannerLine& line, int top) {
  canvas.write(line.text, {middleColumn(), top},
               TextStyle{.font = line.font,
                         .color = line.color,
                         .outlined = true,
                         .alignment = TextAlignment::Center});
}

}  // namespace

void ContinueBanner::follow(std::uint8_t continuesLeft) {
  if (told_.has_value() && continuesLeft < *told_) {
    shownFor_ = engine::Duration::zero();
  }
  told_ = continuesLeft;
}

void ContinueBanner::advance(engine::Duration elapsed) {
  if (!shownFor_.has_value()) {
    return;
  }
  *shownFor_ += elapsed;
  if (*shownFor_ >= CONTINUE_BANNER_TIME) {
    shownFor_.reset();
  }
}

std::optional<std::uint8_t> ContinueBanner::shownContinues() const {
  return shownFor_.has_value() ? told_ : std::nullopt;
}

void ContinueBanner::paint(IInterfaceCanvas& canvas) const {
  const std::optional<std::uint8_t> continuesLeft = shownContinues();
  if (!continuesLeft.has_value()) {
    return;
  }
  const int top = firstLineTop();
  const int secondTop = top + TEXT_LINE_HEIGHT + CONTINUE_BANNER_LINE_GAP;
  paintRule(canvas, top - MISSION_BANNER_RULE_GAP);
  paintRule(canvas, secondTop + TEXT_LINE_HEIGHT + MISSION_BANNER_RULE_GAP - 1);
  writeLine(canvas,
            BannerLine{.text = std::string(
                           CONTINUE_BANNER_TITLE.in(canvas.language())),
                       .font = TextFont::Regular,
                       .color = CONTINUE_BANNER_TITLE_COLOR},
            top);
  writeLine(canvas, secondLineOf(*continuesLeft, canvas.language()), secondTop);
}

}  // namespace rtype::client
