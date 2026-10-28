#include "ScreenOff.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include "Easing.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceConstants.hpp"
#include "NoSignalConstants.hpp"
#include "ScreenOffLook.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

ScreenOffLook screenOffAt(engine::Duration sinceGivingUp) {
  const float collapsed = easeOutQuad(progressBetween(
      sinceGivingUp, engine::Duration::zero(), SCREEN_OFF_COLLAPSE));
  const int rows =
      INTERFACE_AREA.size.y - pixelsOf(INTERFACE_AREA.size.y, collapsed);
  if (rows > SCREEN_OFF_LINE_ROWS) {
    return {.pictureRows = rows, .lineHalfWidth = 0};
  }
  if (sinceGivingUp >= SCREEN_OFF_COLLAPSE + SCREEN_OFF_CLOSE) {
    return {};
  }
  const float closed = easeOutQuad(
      progressBetween(sinceGivingUp, SCREEN_OFF_COLLAPSE, SCREEN_OFF_CLOSE));
  const int halfWidth = INTERFACE_AREA.size.x / SIDES_PER_AXIS;
  return {
      .pictureRows = 0,
      .lineHalfWidth = std::max(1, halfWidth - pixelsOf(halfWidth, closed))};
}

void paintScreenOff(IInterfaceCanvas& canvas, const ScreenOffLook& look) {
  canvas.fill(INTERFACE_AREA, SCREEN_OFF_DARK_COLOR);
  if (look.lineHalfWidth <= 0) {
    return;
  }
  const sf::Vector2i middle = INTERFACE_AREA.size / SIDES_PER_AXIS;
  canvas.fill(
      {{middle.x - look.lineHalfWidth,
        middle.y - (SCREEN_OFF_LINE_THICKNESS / SIDES_PER_AXIS)},
       {SIDES_PER_AXIS * look.lineHalfWidth, SCREEN_OFF_LINE_THICKNESS}},
      SCREEN_OFF_LINE_COLOR);
}

}  // namespace rtype::client
