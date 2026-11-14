#include "TauntLabel.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "BookPose.hpp"
#include "Easing.hpp"
#include "EntitySprites.hpp"
#include "FlightLogColorConstants.hpp"
#include "FlightLogLayoutConstants.hpp"
#include "FlightLogLook.hpp"
#include "FlightLogSheetConstants.hpp"
#include "FlightLogTimeline.hpp"
#include "FlightLogTimingConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "Outline.hpp"
#include "TextMeasures.hpp"
#include "TextMetrics.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"
#include "Typing.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"
#include "WrappedLines.hpp"

namespace rtype::client {

namespace {

/** @brief Writes the lines of the taunt centered on the label, as many of
 * their letters as are typed. */
void writeTaunt(IInterfaceCanvas& canvas, const FlightLogLook& look,
                sf::Vector2i label) {
  std::size_t letters =
      lettersTypedAt(look.sinceStart - TAUNT_START, TAUNT_LETTER_TIME,
                     letterCountOf(look.taunt));
  const int lines = static_cast<int>(look.taunt.size());
  const int block =
      (lines * TAUNT_LINE_PITCH) - (TAUNT_LINE_PITCH - TEXT_LINE_HEIGHT);
  int top = label.y + ((LABEL_SIZE.y - block) / SIDES_PER_AXIS);
  for (const std::string& line : look.taunt) {
    const std::size_t lineLetters = letterCountOf(std::string_view{line});
    canvas.write(typedPrefix(line, letters),
                 {centeredTextLeft(sf::IntRect(label, LABEL_SIZE), line,
                                   canvas.textMetrics()),
                  top},
                 {.font = TextFont::Regular, .color = PAPER_INK});
    letters -= std::min(letters, lineLetters);
    top += TAUNT_LINE_PITCH;
  }
}

/** @returns The lines of the taunt as the label writes them above the size
 * the fonts are drawn at: its words in small print, cut to the inside of the
 * label. */
std::vector<std::string> largeTauntLinesOf(const IInterfaceCanvas& canvas,
                                           const FlightLogLook& look) {
  std::string joined;
  for (const std::string& line : look.taunt) {
    joined += (joined.empty() ? "" : std::string(1, WORD_SEPARATOR)) + line;
  }
  return wrappedLinesOf(
      joined,
      {.width = LABEL_SIZE.x - (SIDES_PER_AXIS * LABEL_TEXT_MARGIN),
       .font = TextFont::Small},
      canvas);
}

/** @brief Paints the label above the size the fonts are drawn at: paper as
 * wide as the label drawn and as tall as its lines need, its edge, its tape,
 * and the lines typed, centered on it. */
void paintLargeLabel(IInterfaceCanvas& canvas, const FlightLogLook& look,
                     sf::Vector2i label) {
  const std::vector<std::string> lines = largeTauntLinesOf(canvas, look);
  const int pitch = lineHeightOf(canvas.textMetrics()) + LABEL_LINE_GAP;
  const sf::IntRect paper(
      label,
      {LABEL_SIZE.x,
       std::max(LABEL_SIZE.y, (static_cast<int>(lines.size()) * pitch) +
                                  (SIDES_PER_AXIS * LABEL_TEXT_MARGIN))});
  canvas.fill(paper, PAPER_COLOR);
  Outline(paper, Outline::Line::Solid).paint(canvas, PAPER_FAINT_COLOR);
  for (const sf::IntRect tape : LABEL_TAPES) {
    canvas.fill(sf::IntRect(label + tape.position, tape.size), TAPE_COLOR);
  }
  std::size_t letters =
      lettersTypedAt(look.sinceStart - TAUNT_START, TAUNT_LETTER_TIME,
                     letterCountOf(look.taunt));
  int top = label.y + LABEL_TEXT_MARGIN;
  for (const std::string& line : lines) {
    canvas.write(typedPrefix(line, letters),
                 {label.x + (LABEL_SIZE.x / SIDES_PER_AXIS), top},
                 {.font = TextFont::Small,
                  .color = PAPER_INK,
                  .alignment = TextAlignment::Center});
    letters -= std::min(letters, letterCountOf(std::string_view{line}));
    top += pitch;
  }
}

/** @returns Where the label lies on the cover of the book in @p pose, falling
 * once the cover starts to turn; nothing once it fell. */
std::optional<sf::Vector2i> labelTopLeftOf(const FlightLogLook& look,
                                           BookPose pose) {
  const engine::Duration fallStart = look.schedule.openStart;
  if (look.sinceStart >= fallStart + LABEL_FALL_TIME) {
    return std::nullopt;
  }
  const int fall =
      pixelsOf(LABEL_FALL, easeInCubic(progressBetween(
                               look.sinceStart, fallStart, LABEL_FALL_TIME)));
  return sf::Vector2i(pose.hinge, pose.top + fall) + LABEL_ON_COVER;
}

}  // namespace

void paintTauntLabel(IInterfaceCanvas& canvas, const FlightLogLook& look,
                     BookPose pose) {
  const std::optional<sf::Vector2i> label = labelTopLeftOf(look, pose);
  if (!label.has_value()) {
    return;
  }
  if (canvas.textMetrics() != TextMetrics{}) {
    paintLargeLabel(canvas, look, *label);
    return;
  }
  canvas.drawImage(TextureArea{.assetId = FLIGHT_LOG_SHEET_ID,
                               .rectangle = rectangleOf(TAUNT_LABEL_FRAME)},
                   ImagePlacement{.topLeft = *label}, INTERFACE_AREA);
  for (const sf::IntRect tape : LABEL_TAPES) {
    canvas.fill(sf::IntRect(*label + tape.position, tape.size), TAPE_COLOR);
  }
  writeTaunt(canvas, look, *label);
}

}  // namespace rtype::client
