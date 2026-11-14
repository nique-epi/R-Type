#include "ChatPainting.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "ChatChannelConstants.hpp"
#include "ChatEntry.hpp"
#include "ChatView.hpp"
#include "ChatWindowConstants.hpp"
#include "Easing.hpp"
#include "FieldText.hpp"
#include "FieldTextPainting.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "PilotColor.hpp"
#include "PilotName.hpp"
#include "PlayerPalette.hpp"
#include "PlayerSettings.hpp"
#include "SunkBox.hpp"
#include "TextMeasures.hpp"
#include "TextMetrics.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"
#include "Typing.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

namespace {

/** @brief One line of the chat on screen: the name that starts it, if any,
 * the text after it, their colors, and the age of its entry. */
struct ChatRow {
  std::string head;
  std::string body;
  sf::Color headColor;
  sf::Color bodyColor;
  engine::Duration age{};
};

/** @brief Where a line is written, and the alpha of its letters. */
struct RowPlace {
  sf::Vector2i topLeft;
  std::uint8_t alpha = OPAQUE_ALPHA;
};

/** @returns @p text cut into lines of @p letters letters at most: at the last
 * space a line holds, inside a word when it holds none. */
std::vector<std::string_view> wrapped(std::string_view text,
                                      std::size_t letters) {
  std::vector<std::string_view> lines;
  while (letterCountOf(text) > letters) {
    const std::string_view full = typedPrefix(text, letters);
    const std::size_t space = full.rfind(' ');
    const std::size_t cut =
        space == std::string_view::npos || space == 0 ? full.size() : space;
    lines.push_back(text.substr(0, cut));
    text.remove_prefix(cut);
    if (!text.empty() && text.front() == ' ') {
      text.remove_prefix(1);
    }
  }
  lines.push_back(text);
  return lines;
}

/** @returns What a line of @p entry starts with: the name of its pilot and
 * CHAT_NAME_SEPARATOR, or nothing for a line the client writes. */
std::string headOf(const ChatEntry& entry) {
  if (entry.playerNumber == CLIENT_WRITER) {
    return {};
  }
  return pilotNameOf(entry.nickname, entry.playerNumber) +
         std::string{CHAT_NAME_SEPARATOR};
}

/** @returns The light shade of the pilot of @p entry, CHAT_NOTICE_COLOR when
 * no pilot color has its number. */
sf::Color nameColorOf(const ChatEntry& entry, const PlayerPalette& palette) {
  const int color = pilotColorOfPlayerNumber(entry.playerNumber);
  return isPilotColor(color) ? palette.at(static_cast<std::size_t>(color)).light
                             : CHAT_NOTICE_COLOR;
}

/** @returns The whole line of @p entry: its head, then its text. */
std::string lineOf(const ChatEntry& entry) {
  return headOf(entry) + entry.text;
}

/** @brief Adds the lines of @p entry on screen to @p rows, its name split off
 * the first. */
void addRowsOf(const ChatEntry& entry, const ChatView& view,
               std::vector<ChatRow>& rows) {
  const std::string head = headOf(entry);
  const std::string line = head + entry.text;
  const sf::Color name = nameColorOf(entry, view.palette);
  const sf::Color body =
      entry.playerNumber == CLIENT_WRITER ? CHAT_NOTICE_COLOR : CHAT_TEXT_COLOR;
  const std::size_t first = rows.size();
  for (const std::string_view piece :
       wrapped(line, static_cast<std::size_t>(chatLettersOf(view.settings)))) {
    rows.push_back(ChatRow{.head = {},
                           .body = std::string{piece},
                           .headColor = name,
                           .bodyColor = body,
                           .age = entry.age});
  }
  ChatRow& named = rows[first];
  const std::size_t headShown = std::min(head.size(), named.body.size());
  named.head = named.body.substr(0, headShown);
  named.body.erase(0, headShown);
}

/** @returns The lines of every entry the view shows: all of them while the
 * chat is open, those that still show while it is closed. */
std::vector<ChatRow> rowsOf(const ChatView& view) {
  std::vector<ChatRow> rows;
  for (const ChatEntry& entry : view.entries) {
    if (view.draft.has_value() || isShownClosed(entry, view.settings)) {
      addRowsOf(entry, view, rows);
    }
  }
  return rows;
}

/** @returns The text size the chat is written at, as @p settings choose it. */
TextMetrics metricsOf(const PlayerSettings& settings) {
  return textMetricsOf(settings.textSize);
}

/** @returns From one line of the chat to the next at the text size of
 * @p metrics: a line and its outline, as CHAT_LINE_PITCH is as drawn. */
int linePitchOf(TextMetrics metrics) {
  return textBoxHeight(CHAT_LINE_PITCH, metrics);
}

/** @returns The top row of the line @p fromBottom lines above the last one,
 * whose outline ends on @p bottom, at the text size of @p metrics. */
int rowTop(int bottom, int fromBottom, TextMetrics metrics) {
  return bottom - lineHeightOf(metrics) - BORDER_THICKNESS -
         (fromBottom * linePitchOf(metrics));
}

/** @returns The alpha of a line @p age old: opaque, then fading to nothing
 * over the last CHAT_FADE_TIME of chatSeconds. */
std::uint8_t alphaOf(engine::Duration age, const PlayerSettings& settings) {
  const float faded = progressBetween(
      age, std::chrono::seconds(settings.chatSeconds) - CHAT_FADE_TIME,
      CHAT_FADE_TIME);
  return static_cast<std::uint8_t>(static_cast<float>(OPAQUE_ALPHA) *
                                   (1.0F - faded));
}

/** @returns @p color at @p alpha. */
sf::Color withAlpha(sf::Color color, std::uint8_t alpha) {
  color.a = alpha;
  return color;
}

void paintRow(IInterfaceCanvas& canvas, const ChatRow& row, RowPlace place) {
  if (!row.head.empty()) {
    canvas.write(row.head, place.topLeft,
                 {.font = TextFont::Regular,
                  .color = withAlpha(row.headColor, place.alpha),
                  .outlined = true});
  }
  canvas.write(
      row.body,
      place.topLeft +
          sf::Vector2i(regularWidthOf(canvas.textMetrics(), row.head), 0),
      {.font = TextFont::Regular,
       .color = withAlpha(row.bodyColor, place.alpha),
       .outlined = true});
}

void paintClosed(IInterfaceCanvas& canvas, const ChatView& view) {
  const std::vector<ChatRow> rows = rowsOf(view);
  const PlayerSettings& settings = view.settings;
  const int count = static_cast<int>(rows.size());
  const int shown = std::min(count, settings.chatClosedLines);
  for (int fromBottom = 0; fromBottom < shown; ++fromBottom) {
    const ChatRow& row = rows[static_cast<std::size_t>(count - 1 - fromBottom)];
    paintRow(canvas, row,
             {.topLeft = {CHAT_LEFT, rowTop(view.bottom, fromBottom,
                                            canvas.textMetrics())},
              .alpha = alphaOf(row.age, settings)});
  }
}

/** @returns The text of the line that says @p lines lines lie above. */
std::string moreLinesText(int lines) {
  return std::string{MORE_LINES_START} + std::to_string(lines) +
         std::string{MORE_LINES_END};
}

/** @returns How many lines of the open chat show over @p bottom at the text
 * size of @p metrics: chatOpenLines, or as many as fit under the safe margin
 * with the line that says how many lie above. */
int openLinesOf(const PlayerSettings& settings, int bottom,
                TextMetrics metrics) {
  const int fit = ((bottom - SAFE_MARGIN) / linePitchOf(metrics)) - 1;
  return std::max(1, std::min(settings.chatOpenLines, fit));
}

/** @brief Paints the lines of the open chat over @p bottom, the last
 * chatOpenLines once scrolled, as many as fit, and how many lie above them. */
void paintHistory(IInterfaceCanvas& canvas, const ChatView& view, int bottom) {
  const std::vector<ChatRow> rows = rowsOf(view);
  const TextMetrics metrics = canvas.textMetrics();
  const int end = std::max(0, static_cast<int>(rows.size()) - view.scroll);
  const int start =
      std::max(0, end - openLinesOf(view.settings, bottom, metrics));
  for (int index = start; index < end; ++index) {
    paintRow(canvas, rows[static_cast<std::size_t>(index)],
             {.topLeft = {CHAT_LEFT, rowTop(bottom, end - 1 - index, metrics)},
              .alpha = OPAQUE_ALPHA});
  }
  if (start > 0) {
    canvas.write(
        moreLinesText(start), {CHAT_LEFT, rowTop(bottom, end - start, metrics)},
        {.font = TextFont::Small, .color = CHAT_HINT_COLOR, .outlined = true});
  }
}

/** @returns The box of the field: chatLetters letters, its caret and its
 * padding wide, its bottom on the bottom of the view. */
sf::IntRect fieldAreaOf(const ChatView& view) {
  const TextMetrics metrics = metricsOf(view.settings);
  const int width = (chatLettersOf(view.settings) * lineHeightOf(metrics)) +
                    (SIDES_PER_AXIS * FIELD_TEXT_PADDING) + FIELD_CARET_SIZE.x;
  const int height = textBoxHeight(CHAT_FIELD_HEIGHT, metrics);
  return {{CHAT_LEFT, view.bottom - height}, {width, height}};
}

/** @returns The last @p letters letters of @p text, all of it when shorter. */
std::string_view tailOf(std::string_view text, std::size_t letters) {
  const std::size_t count = letterCountOf(text);
  return count <= letters
             ? text
             : text.substr(typedPrefix(text, count - letters).size());
}

/** @brief Writes SAY over the field, and the keys at its right when they fit
 * beside it. */
void paintLabel(IInterfaceCanvas& canvas, sf::IntRect field, int top) {
  const TextStyle label{
      .font = TextFont::Regular, .color = CHAT_LABEL_COLOR, .outlined = true};
  canvas.write(CHAT_FIELD_LABEL, {CHAT_LEFT, top}, label);
  const int right = field.position.x + field.size.x;
  if (CHAT_LEFT + regularWidthOf(canvas.textMetrics(), CHAT_FIELD_LABEL) +
          CHAT_HINT_GAP + canvas.textWidth(CHAT_FIELD_HINT, TextFont::Small) <=
      right) {
    canvas.write(CHAT_FIELD_HINT, {right, top},
                 {.font = TextFont::Small,
                  .color = CHAT_HINT_COLOR,
                  .outlined = true,
                  .alignment = TextAlignment::Right});
  }
}

void paintOpen(IInterfaceCanvas& canvas, const ChatView& view) {
  const std::string_view draft = view.draft.value_or(std::string_view{});
  canvas.fill(INTERFACE_AREA,
              withAlpha(DIM_LAYER_COLOR,
                        static_cast<std::uint8_t>(
                            OPAQUE_ALPHA * view.settings.get().chatBackground /
                            WHOLE_PERCENT)));
  const sf::IntRect field = fieldAreaOf(view);
  SunkBox(field, SunkBox::Sides::Lit).paint(canvas);
  paintFieldText(
      canvas,
      FieldText{.box = field,
                .text = tailOf(draft, static_cast<std::size_t>(
                                          chatLettersOf(view.settings))),
                .color = SELECTED_FIELD_TEXT_COLOR,
                .caret = true});
  const int labelTop =
      field.position.y - FIELD_LABEL_GAP - lineHeightOf(canvas.textMetrics());
  paintLabel(canvas, field, labelTop);
  paintHistory(canvas, view, labelTop - FIELD_LABEL_GAP);
}

}  // namespace

int chatLettersOf(const PlayerSettings& settings) {
  const int room = INTERFACE_AREA.size.x - (SIDES_PER_AXIS * CHAT_LEFT) -
                   (SIDES_PER_AXIS * FIELD_TEXT_PADDING) - FIELD_CARET_SIZE.x;
  return std::min(settings.chatLetters,
                  room / lineHeightOf(metricsOf(settings)));
}

void paintChat(IInterfaceCanvas& canvas, const ChatView& view) {
  if (!view.settings.get().chatShown) {
    return;
  }
  if (view.draft.has_value()) {
    paintOpen(canvas, view);
    return;
  }
  paintClosed(canvas, view);
}

int chatLineCount(std::span<const ChatEntry> entries, int letters) {
  std::size_t lines = 0;
  for (const ChatEntry& entry : entries) {
    lines += wrapped(lineOf(entry), static_cast<std::size_t>(letters)).size();
  }
  return static_cast<int>(lines);
}

bool isShownClosed(const ChatEntry& entry, const PlayerSettings& settings) {
  return entry.age < std::chrono::seconds(settings.chatSeconds);
}

}  // namespace rtype::client
