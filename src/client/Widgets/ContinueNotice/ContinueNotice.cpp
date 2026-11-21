#include "ContinueNotice.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <cstdint>
#include <optional>
#include <string>
#include "ContinueNoticeConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

namespace {

/** @brief Paints the band of the notice whose line starts on row @p top, and
 * its two rules. */
void paintBand(IInterfaceCanvas& canvas, int top) {
  const sf::IntRect band(
      {CONTINUE_NOTICE_AREA.position.x, top - CONTINUE_NOTICE_PADDING},
      {CONTINUE_NOTICE_AREA.size.x,
       TEXT_LINE_HEIGHT + (2 * CONTINUE_NOTICE_PADDING)});
  canvas.fill(band, CONTINUE_NOTICE_BAND_COLOR);
  canvas.fill(sf::IntRect(band.position, {band.size.x, 1}), WARNING_ORANGE);
  canvas.fill(sf::IntRect({band.position.x, band.position.y + band.size.y - 1},
                          {band.size.x, 1}),
              WARNING_ORANGE);
}

}  // namespace

void ContinueNotice::follow(std::uint8_t continuesLeft, engine::Duration now) {
  if (known_.has_value() && continuesLeft < *known_) {
    spentAt_ = now;
    shown_ = continuesLeft;
  }
  known_ = continuesLeft;
}

std::optional<std::uint8_t> ContinueNotice::shownAt(
    engine::Duration now) const {
  if (!spentAt_.has_value() || now - *spentAt_ >= CONTINUE_NOTICE_TIME) {
    return std::nullopt;
  }
  return shown_;
}

void ContinueNotice::paint(IInterfaceCanvas& canvas,
                           std::uint8_t continuesLeft) {
  const std::string line = continueNoticeText(continuesLeft);
  const int top = centeredTextTop(CONTINUE_NOTICE_AREA);
  paintBand(canvas, top);
  canvas.write(line, {centeredTextLeft(CONTINUE_NOTICE_AREA, line), top},
               TextStyle{.font = TextFont::Regular,
                         .color = WARNING_ORANGE,
                         .outlined = true,
                         .alignment = TextAlignment::Left});
}

std::string continueNoticeText(std::uint8_t continuesLeft) {
  std::string line{CONTINUE_NOTICE_OPENING};
  if (continuesLeft == 0) {
    return line + std::string{NO_CONTINUE_LEFT_TEXT};
  }
  if (continuesLeft == 1) {
    return line + std::string{ONE_CONTINUE_LEFT_TEXT};
  }
  return line + std::to_string(continuesLeft) +
         std::string{CONTINUES_LEFT_TEXT};
}

}  // namespace rtype::client
