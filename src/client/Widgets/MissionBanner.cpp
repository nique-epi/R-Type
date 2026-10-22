#include "MissionBanner.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstddef>
#include "IInterfaceCanvas.hpp"
#include "InterfaceColors.hpp"
#include "InterfaceConstants.hpp"
#include "MissionBannerConstants.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

namespace {

/** @brief Paints one rule of the banner, centered on the line, on that
 * row. */
void paintRule(IInterfaceCanvas& canvas, int row, int halfWidth) {
  const int middle = MISSION_BANNER_AREA.position.x +
                     (MISSION_BANNER_AREA.size.x / SIDES_PER_AXIS);
  canvas.fill(
      sf::IntRect({middle - halfWidth, row}, {SIDES_PER_AXIS * halfWidth, 1}),
      EDGE_BLUE);
}

}  // namespace

MissionBannerLook missionBannerAt(engine::Duration sinceStart) {
  const engine::Duration intoBanner = sinceStart - MISSION_BANNER_DELAY;
  const engine::Duration writing =
      MISSION_BANNER_LETTER_TIME *
      static_cast<engine::Duration::rep>(MISSION_BANNER_TEXT.size());
  if (intoBanner < engine::Duration::zero() ||
      intoBanner >= writing + MISSION_BANNER_HOLD) {
    return {};
  }
  const std::size_t letters = std::min(
      MISSION_BANNER_TEXT.size(),
      static_cast<std::size_t>(intoBanner / MISSION_BANNER_LETTER_TIME) + 1);
  return MissionBannerLook{
      .letters = letters,
      .ruleHalfWidth = static_cast<int>(
          (static_cast<std::size_t>(MISSION_BANNER_RULE_HALF_WIDTH) * letters) /
          MISSION_BANNER_TEXT.size())};
}

void paintMissionBanner(IInterfaceCanvas& canvas, MissionBannerLook look) {
  if (look.letters == 0) {
    return;
  }
  const int top = centeredTextTop(MISSION_BANNER_AREA);
  paintRule(canvas, top - MISSION_BANNER_RULE_GAP, look.ruleHalfWidth);
  paintRule(canvas, top + TEXT_LINE_HEIGHT + MISSION_BANNER_RULE_GAP - 1,
            look.ruleHalfWidth);
  canvas.write(
      MISSION_BANNER_TEXT.substr(0, look.letters),
      {centeredTextLeft(MISSION_BANNER_AREA, MISSION_BANNER_TEXT), top},
      TextStyle{.font = TextFont::Regular,
                .color = HUD_CYAN,
                .outlined = true,
                .alignment = TextAlignment::Left});
}

}  // namespace rtype::client
