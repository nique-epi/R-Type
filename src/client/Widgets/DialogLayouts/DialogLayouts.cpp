#include "DialogLayouts.hpp"
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <string>
#include <string_view>
#include <vector>
#include "DialogLayout.hpp"
#include "DialogText.hpp"
#include "InterfaceConstants.hpp"
#include "TextMeasures.hpp"
#include "TextMetrics.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

namespace {

/** @returns @p text cut into lines no wider than @p width in the regular
 * font at @p metrics, each word whole. */
std::vector<std::string> wrappedWordsOf(std::string_view text, int width,
                                        TextMetrics metrics) {
  std::vector<std::string> lines;
  std::size_t start = 0;
  while (start < text.size()) {
    std::size_t end = text.find(WORD_SEPARATOR, start);
    end = end == std::string_view::npos ? text.size() : end;
    const std::string_view word = text.substr(start, end - start);
    const std::string joined =
        lines.empty() ? std::string(word)
                      : lines.back() + WORD_SEPARATOR + std::string(word);
    if (!lines.empty() && regularTextWidth(metrics, joined) <= width) {
      lines.back() = joined;
    } else {
      lines.emplace_back(word);
    }
    start = end + 1;
  }
  return lines;
}

/** @returns How wide the buttons of a dialog are at @p metrics: as drawn, or
 * as wide as the widest label and its padding. */
int dialogButtonWidthOf(const DialogText& text, TextMetrics metrics) {
  int width = DIALOG_BUTTON_SIZE.x;
  for (const std::string_view label : text.barLabels) {
    width = std::max(width, regularTextWidth(metrics, label) +
                                (SIDES_PER_AXIS * BUTTON_TEXT_OFFSET.x));
  }
  return width;
}

/** @returns How wide a dialog is above the size the fonts are drawn at: as
 * wide as its heading, its message or its row of buttons need, from
 * DIALOG_SIZE wide to DIALOG_LARGEST_WIDTH; the widest when it holds rows. */
int grownDialogWidthOf(const DialogText& text, TextMetrics metrics,
                       int buttonWidth) {
  if (text.rowCount > 0) {
    return DIALOG_LARGEST_WIDTH;
  }
  const int bars = static_cast<int>(text.barLabels.size());
  const int needed =
      std::max({regularTextWidth(metrics, text.heading),
                regularTextWidth(metrics, text.message),
                (bars * buttonWidth) + ((bars - 1) * DIALOG_BUTTON_GAP)});
  return std::clamp(needed + (SIDES_PER_AXIS * DIALOG_HEADING_OFFSET.x),
                    DIALOG_SIZE.x, DIALOG_LARGEST_WIDTH);
}

/** @returns The layout of a dialog as dialogs were drawn. */
DialogLayout drawnDialogLayoutOf(const DialogText& text) {
  const int buttonsTop =
      text.rowCount == 0
          ? DIALOG_BUTTONS_TOP
          : DIALOG_ROWS_TOP + (text.rowCount * DIALOG_ROW_PITCH) + ROW_GAP;
  const int height = text.rowCount == 0 ? DIALOG_SIZE.y
                                        : buttonsTop + DIALOG_BUTTON_SIZE.y +
                                              DIALOG_MARGIN_UNDER_BUTTONS;
  return DialogLayout{.size = {DIALOG_SIZE.x, height},
                      .messageLines = {std::string(text.message)},
                      .messageTop = DIALOG_MESSAGE_OFFSET.y,
                      .messagePitch = TEXT_LINE_HEIGHT,
                      .rowsTop = DIALOG_ROWS_TOP,
                      .rowSize = DIALOG_ROW_SIZE,
                      .rowPitch = DIALOG_ROW_PITCH,
                      .buttonsTop = buttonsTop,
                      .buttonSize = DIALOG_BUTTON_SIZE};
}

}  // namespace

DialogLayout dialogLayoutOf(const DialogText& text, TextMetrics metrics) {
  if (metrics == TextMetrics{}) {
    return drawnDialogLayoutOf(text);
  }
  const int lineHeight = lineHeightOf(metrics);
  const int buttonWidth = dialogButtonWidthOf(text, metrics);
  const int width = grownDialogWidthOf(text, metrics, buttonWidth);
  DialogLayout layout{
      .size = {width, 0},
      .messageLines = wrappedWordsOf(
          text.message, width - (SIDES_PER_AXIS * DIALOG_HEADING_OFFSET.x),
          metrics),
      .messageTop = DIALOG_MESSAGE_OFFSET.y - TEXT_LINE_HEIGHT + lineHeight,
      .messagePitch = lineHeight + DESCRIPTION_LINE_GAP,
      .rowsTop = DIALOG_ROWS_TOP - TEXT_LINE_HEIGHT + lineHeight,
      .rowSize = {width - (SIDES_PER_AXIS * DIALOG_ROW_INSET),
                  textBoxHeight(DIALOG_ROW_SIZE.y, metrics)},
      .rowPitch = textBoxHeight(DIALOG_ROW_SIZE.y, metrics) + ROW_GAP,
      .buttonsTop = 0,
      .buttonSize = {buttonWidth,
                     textBoxHeight(DIALOG_BUTTON_SIZE.y, metrics)}};
  const auto lines = static_cast<int>(layout.messageLines.size());
  layout.buttonsTop =
      text.rowCount == 0
          ? layout.messageTop + (lines * layout.messagePitch) +
                (DIALOG_BUTTONS_TOP - DIALOG_MESSAGE_OFFSET.y -
                 TEXT_LINE_HEIGHT)
          : layout.rowsTop + (text.rowCount * layout.rowPitch) + ROW_GAP;
  layout.size.y = layout.buttonsTop + layout.buttonSize.y +
                  (text.rowCount == 0 ? DIALOG_SIZE.y - DIALOG_BUTTONS_TOP -
                                            DIALOG_BUTTON_SIZE.y
                                      : DIALOG_MARGIN_UNDER_BUTTONS);
  return layout;
}

}  // namespace rtype::client
