#include "MissionBanner.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <string_view>
#include "IInterfaceCanvas.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "Language.hpp"
#include "MissionBannerConstants.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"
#include "Typing.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

namespace {

/** @brief Paints one rule of the banner, centered on the line, on that
 * row. */
void paintRule(IInterfaceCanvas& canvas, int row, int halfWidth) {
  canvas.fill(centeredRule(MISSION_BANNER_AREA, row, halfWidth), EDGE_BLUE);
}

}  // namespace

MissionBannerLook missionBannerAt(engine::Duration sinceStart,
                                  Language language) {
  const std::size_t length = letterCountOf(MISSION_BANNER_TEXT.in(language));
  const engine::Duration intoBanner = sinceStart - MISSION_BANNER_DELAY;
  const engine::Duration writing =
      typingTimeOf(MISSION_BANNER_LETTER_TIME, length);
  if (intoBanner < engine::Duration::zero() ||
      intoBanner >= writing + MISSION_BANNER_HOLD) {
    return {};
  }
  const std::size_t letters =
      lettersTypedAt(intoBanner, MISSION_BANNER_LETTER_TIME, length);
  return MissionBannerLook{
      .letters = letters,
      .ruleHalfWidth = static_cast<int>(
          (static_cast<std::size_t>(MISSION_BANNER_RULE_HALF_WIDTH) * letters) /
          length)};
}

void paintMissionBanner(IInterfaceCanvas& canvas, MissionBannerLook look) {
  if (look.letters == 0) {
    return;
  }
  const int top = centeredTextTop(MISSION_BANNER_AREA);
  paintRule(canvas, top - MISSION_BANNER_RULE_GAP, look.ruleHalfWidth);
  paintRule(canvas, top + TEXT_LINE_HEIGHT + MISSION_BANNER_RULE_GAP - 1,
            look.ruleHalfWidth);
  const std::string_view text = MISSION_BANNER_TEXT.in(canvas.language());
  canvas.write(typedPrefix(text, look.letters),
               {centeredTextLeft(MISSION_BANNER_AREA, text), top},
               TextStyle{.font = TextFont::Regular,
                         .color = HUD_CYAN,
                         .outlined = true,
                         .alignment = TextAlignment::Left});
}

}  // namespace rtype::client
