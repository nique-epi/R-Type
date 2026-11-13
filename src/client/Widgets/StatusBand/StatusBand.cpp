#include "StatusBand.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Utf.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "CutCornerBox.hpp"
#include "EntitySpriteConstants.hpp"
#include "EntitySprites.hpp"
#include "FlashPreference.hpp"
#include "HudSpriteConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceConstants.hpp"
#include "Outline.hpp"
#include "PaddedScore.hpp"
#include "PilotColor.hpp"
#include "PilotDeparture.hpp"
#include "PilotName.hpp"
#include "PilotStatus.hpp"
#include "PlayerPalette.hpp"
#include "PlayerShading.hpp"
#include "ShadeSwap.hpp"
#include "SpriteFrame.hpp"
#include "StatusBandConstants.hpp"
#include "StatusBandLook.hpp"
#include "TextMeasures.hpp"
#include "TextMetrics.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @brief The word of the first row of a bay, in its font and color. */
struct StatusWord {
  const char* text{};
  TextFont font{};
  sf::Color color;
};

/** @brief A row of the same life, count times, from its first top-left
 * corner, its shades swapped as the palette asks. */
struct LifeRow {
  sf::Vector2i topLeft;
  game::SpriteFrame frame{};
  int count{};
  std::optional<ShadeSwap> recolor = std::nullopt;
};

/** @brief The lives of a pilot to draw: where they start, the index of the
 * player color, how many lives the pilot has left, and the swap of their
 * shades. */
struct LifeCount {
  sf::Vector2i topLeft;
  std::size_t color{};
  int lives{};
  std::optional<ShadeSwap> recolor = std::nullopt;
};

/** @brief Where the bay of a pilot lies, and how the palette the player chose
 * paints its color. */
struct PilotBay {
  sf::Vector2i topLeft;
  PlayerShading shading;
};

/** @brief A bay being painted, and the look of the band. */
struct PaintedBay {
  PilotBay bay;
  StatusBandLook look;
};

/** @brief A line of a bay that may not fit: its text, the room it lies in,
 * and how it is written. */
struct BayLine {
  std::string_view text;
  sf::IntRect room;
  TextStyle style;
};

/** @brief The texts a bay shows in turn above the size the fonts are drawn
 * at, kept while their lines are painted. */
struct BayText {
  std::string name;
  std::string score;
  std::string lives;
};

/** @returns The color of a pilot, 0 to PLAYER_COLOR_COUNT - 1: its number
 * less FIRST_PLAYER_NUMBER. */
int colorOf(const PilotStatus& pilot) {
  return pilotColorOfPlayerNumber(pilot.playerNumber);
}

/** @returns The color of a pilot as an index of the tables of player
 * colors; only for a pilot that has a bay. */
std::size_t colorIndexOf(const PilotStatus& pilot) {
  return static_cast<std::size_t>(colorOf(pilot));
}

/** @returns Whether a pilot's number has a bay, one per player color. */
bool hasBay(const PilotStatus& pilot) {
  return colorOf(pilot) >= 0 && colorOf(pilot) < PLAYER_COLOR_COUNT;
}

/** @returns Whether a pilot is gone from the game: out of lives, left,
 * dropped or expelled. */
bool isGone(const PilotStatus& pilot) {
  return pilot.state == PilotState::OutOfLives || hasLeft(pilot);
}

/** @returns The left column of a bay, 0 to PLAYER_COLOR_COUNT - 1 from the
 * left. */
int bayLeft(int bay) {
  return STATUS_BAY_INSET + (bay * (STATUS_BAY_WIDTH + STATUS_BAY_GAP));
}

/** @returns The top-left corner of the bay of a pilot. */
sf::Vector2i bayOf(const PilotStatus& pilot) {
  return {bayLeft(colorOf(pilot)), STATUS_BAND_AREA.position.y};
}

/** @returns The bay of a pilot that has one, in the shades @p palette gives
 * its color. */
PilotBay pilotBayOf(const PilotStatus& pilot, const PlayerPalette& palette) {
  return {.topLeft = bayOf(pilot),
          .shading = shadingOf(palette, colorIndexOf(pilot))};
}

/** @returns The word of a pilot gone from the game: LEFT, SIGNAL LOST,
 * EXPELLED or OUT; nothing for a pilot still flying. */
std::optional<StatusWord> goneWordOf(PilotState state) {
  switch (state) {
    case PilotState::Left:
      return StatusWord{.text = LEFT_GAME_TEXT,
                        .font = TextFont::Small,
                        .color = GONE_TEXT_COLOR};
    case PilotState::SignalLost:
      return StatusWord{.text = SIGNAL_LOST_TEXT,
                        .font = TextFont::Small,
                        .color = GONE_TEXT_COLOR};
    case PilotState::Expelled:
      return StatusWord{.text = EXPELLED_TEXT,
                        .font = TextFont::Small,
                        .color = GONE_TEXT_COLOR};
    case PilotState::OutOfLives:
      return StatusWord{.text = OUT_TEXT,
                        .font = TextFont::Regular,
                        .color = WARNING_TEXT_COLOR};
    case PilotState::Flying:
    case PilotState::Respawning:
      break;
  }
  return std::nullopt;
}

/** @returns The word of a pilot still flying, the first that applies: LAST
 * LIFE, RESPAWN, then YOU; nothing for a teammate with lives to spare. */
std::optional<StatusWord> flyingWordOf(const PilotStatus& pilot) {
  if (pilot.lives == LAST_LIFE_COUNT) {
    return StatusWord{.text = LAST_LIFE_TEXT,
                      .font = TextFont::Small,
                      .color = WARNING_TEXT_COLOR};
  }
  if (pilot.state == PilotState::Respawning) {
    return StatusWord{.text = RESPAWN_TEXT,
                      .font = TextFont::Small,
                      .color = STATUS_LABEL_COLOR};
  }
  if (pilot.isLocalPlayer) {
    return StatusWord{.text = LOCAL_PLAYER_TAG,
                      .font = TextFont::Regular,
                      .color = STATUS_LABEL_COLOR};
  }
  return std::nullopt;
}

/** @returns The word of a pilot: the one of a gone pilot, or of a flying
 * one. */
std::optional<StatusWord> statusWordOf(const PilotStatus& pilot) {
  const std::optional<StatusWord> goneWord = goneWordOf(pilot.state);
  return goneWord.has_value() ? goneWord : flyingWordOf(pilot);
}

/** @returns How many whole characters of the regular font at @p metrics fit
 * @p width, one at least. */
std::size_t charactersIn(int width, TextMetrics metrics) {
  return static_cast<std::size_t>(std::max(1, width / lineHeightOf(metrics)));
}

/** @returns The part of @p text shown at turn @p turn when it is shown
 * @p characters whole characters at a time, the parts in a loop. */
std::string_view partAtTurn(std::string_view text, std::size_t characters,
                            std::size_t turn) {
  const auto count =
      static_cast<std::size_t>(sf::Utf8::count(text.begin(), text.end()));
  const std::size_t parts = (count + characters - 1) / characters;
  std::string_view::const_iterator start = text.begin();
  for (std::size_t skipped = 0; skipped < (turn % parts) * characters;
       ++skipped) {
    start = sf::Utf8::next(start, text.end());
  }
  std::string_view::const_iterator end = start;
  for (std::size_t kept = 0; kept < characters && end != text.end(); ++kept) {
    end = sf::Utf8::next(end, text.end());
  }
  return {start, end};
}

/** @returns The turn a band shows at @p sinceStart, one per
 * STATUS_TURN_DURATION. */
std::size_t turnAt(engine::Duration sinceStart) {
  return static_cast<std::size_t>(sinceStart / STATUS_TURN_DURATION);
}

/** @brief Writes @p line in its room: whole when it fits; otherwise running
 * through the room, two copies STATUS_SCROLL_GAP apart, or, with REDUCE
 * FLASHES, its parts that fit the room in turn. */
void paintBayLine(IInterfaceCanvas& canvas, const BayLine& line,
                  const StatusBandLook& look) {
  const int width = canvas.textWidth(line.text, line.style.font);
  if (width <= line.room.size.x) {
    canvas.write(line.text, line.room.position, line.style);
    return;
  }
  TextStyle clipped = line.style;
  clipped.clip = line.room;
  if (look.flashes == FlashPreference::Reduced) {
    canvas.write(
        partAtTurn(line.text,
                   charactersIn(line.room.size.x, canvas.textMetrics()),
                   turnAt(look.sinceStart)),
        line.room.position, clipped);
    return;
  }
  const int period = width + STATUS_SCROLL_GAP;
  const int offset =
      static_cast<int>((look.sinceStart / STATUS_SCROLL_STEP) % period);
  for (const int copy : {0, period}) {
    canvas.write(line.text, line.room.position + sf::Vector2i(copy - offset, 0),
                 clipped);
  }
}

/** @returns The color of the name of a pilot: the light shade of its color,
 * dimmed once it is gone. */
sf::Color nameColorOf(const PilotStatus& pilot, const PilotBay& bay) {
  return isGone(pilot) ? GONE_TEXT_COLOR : bay.shading.shades.light;
}

/** @returns The room the first row of a bay leaves the name, in pixels: from
 * STATUS_NAME_TOP_LEFT to STATUS_WORD_END, less the word and STATUS_WORD_GAP
 * when there is one. */
int nameRoom(const IInterfaceCanvas& canvas,
             const std::optional<StatusWord>& word) {
  const int row = STATUS_WORD_END.x - STATUS_NAME_TOP_LEFT.x;
  if (!word.has_value()) {
    return row;
  }
  return row - canvas.textWidth(word->text, word->font) - STATUS_WORD_GAP;
}

/** @brief Paints the panel of the band and the rules between its bays. */
void paintPanel(IInterfaceCanvas& canvas) {
  CutCornerBox(STATUS_BAND_AREA, {.border = STATUS_BAND_BORDER_COLOR,
                                  .inside = STATUS_BAND_INSIDE_COLOR})
      .paint(canvas);
  for (int bay = 1; bay < PLAYER_COLOR_COUNT; ++bay) {
    const int ruleLeft = bayLeft(bay) - STATUS_BAY_GAP;
    canvas.fill({{ruleLeft, STATUS_BAND_AREA.position.y + STATUS_RULE_TOP},
                 {BORDER_THICKNESS, STATUS_RULE_HEIGHT}},
                STATUS_RULE_COLOR);
  }
}

/** @brief Paints the chip of a pilot: its color and its marker; a gone
 * pilot's chip is dimmed, its marker kept. */
void paintChip(IInterfaceCanvas& canvas, const PilotStatus& pilot,
               const PilotBay& bay) {
  const sf::IntRect chip(bay.topLeft + STATUS_CHIP_TOP_LEFT, STATUS_CHIP_SIZE);
  canvas.fill(chip, isGone(pilot) ? GONE_CHIP_COLOR : bay.shading.shades.ship);
  canvas.drawImage(
      TextureArea{.assetId = HUD_SHEET_ID,
                  .rectangle = rectangleOf(MARKER_FRAMES[colorIndexOf(pilot)])},
      ImagePlacement{.topLeft = chip.position + STATUS_MARKER_OFFSET,
                     .recolor = bay.shading.recolor},
      chip);
}

/** @brief Writes a word against the right edge of the first row of a bay,
 * SMALL_WORD_DROP lower in small print. */
void paintWord(IInterfaceCanvas& canvas, const StatusWord& word,
               sf::Vector2i bay) {
  const int drop = word.font == TextFont::Small ? SMALL_WORD_DROP : 0;
  canvas.write(word.text, bay + STATUS_WORD_END + sf::Vector2i(0, drop),
               {.font = word.font,
                .color = word.color,
                .alignment = TextAlignment::Right});
}

/** @brief Writes the first row of a bay: the word of the pilot against the
 * right edge, then its name from STATUS_NAME_TOP_LEFT, in the light shade of
 * its color, cut to the room the word leaves. */
void paintTitle(IInterfaceCanvas& canvas, const PilotStatus& pilot,
                const PaintedBay& bay) {
  const std::optional<StatusWord> word = statusWordOf(pilot);
  if (word.has_value()) {
    paintWord(canvas, *word, bay.bay.topLeft);
  }
  const std::string name = pilotNameOf(pilot.nickname, pilot.playerNumber);
  paintBayLine(canvas,
               BayLine{.text = name,
                       .room = {bay.bay.topLeft + STATUS_NAME_TOP_LEFT,
                                {nameRoom(canvas, word), TEXT_LINE_HEIGHT}},
                       .style = {.font = TextFont::Regular,
                                 .color = nameColorOf(pilot, bay.bay)}},
               bay.look);
}

/** @brief Draws the same life count times, STATUS_LIFE_STEP apart. */
void paintLifeRow(IInterfaceCanvas& canvas, const LifeRow& row) {
  for (int life = 0; life < row.count; ++life) {
    canvas.drawImage(
        TextureArea{.assetId = HUD_SHEET_ID,
                    .rectangle = rectangleOf(row.frame)},
        ImagePlacement{
            .topLeft = row.topLeft + sf::Vector2i(life * STATUS_LIFE_STEP, 0),
            .recolor = row.recolor},
        STATUS_BAND_AREA);
  }
}

/** @brief Paints a count of lives from its top-left corner: up to
 * STATUS_LIFE_ICON_LIMIT, a ship of the player color for each life left, then
 * a lost life for each of the others, so the slots stay the same whatever the
 * count; past the limit, one ship and the count. */
void paintLifeCount(IInterfaceCanvas& canvas, const LifeCount& count) {
  const game::SpriteFrame life = LIFE_ROW[count.color];
  if (count.lives > STATUS_LIFE_ICON_LIMIT) {
    paintLifeRow(canvas, {.topLeft = count.topLeft,
                          .frame = life,
                          .count = 1,
                          .recolor = count.recolor});
    canvas.write(LIVES_COUNT_PREFIX + std::to_string(count.lives),
                 count.topLeft + STATUS_LIVES_COUNT_OFFSET,
                 {.font = TextFont::Regular, .color = SCORE_COLOR});
    return;
  }
  paintLifeRow(canvas, {.topLeft = count.topLeft,
                        .frame = life,
                        .count = count.lives,
                        .recolor = count.recolor});
  paintLifeRow(canvas,
               {.topLeft = count.topLeft +
                           sf::Vector2i(count.lives * STATUS_LIFE_STEP, 0),
                .frame = LOST_LIFE_FRAME,
                .count = STATUS_LIFE_ICON_LIMIT - count.lives});
}

/** @brief Paints the left of the second row of a pilot: the unplugged plug
 * once its signal is lost, nothing once it left or was expelled or while its
 * lives are unknown, otherwise its count of lives, every life lost once it has
 * none left. */
void paintLives(IInterfaceCanvas& canvas, const PilotStatus& pilot,
                const PilotBay& bay) {
  const sf::Vector2i topLeft = bay.topLeft + STATUS_LIVES_TOP_LEFT;
  switch (pilot.state) {
    case PilotState::SignalLost:
      canvas.drawSmoothImage(PLUG_ICON_ID, {topLeft, STATUS_ICON_SIZE},
                             GONE_TEXT_COLOR);
      return;
    case PilotState::Left:
    case PilotState::Expelled:
      return;
    case PilotState::Flying:
    case PilotState::Respawning:
    case PilotState::OutOfLives:
      break;
  }
  if (pilot.lives.has_value()) {
    paintLifeCount(canvas, {.topLeft = topLeft,
                            .color = colorIndexOf(pilot),
                            .lives = *pilot.lives,
                            .recolor = bay.shading.recolor});
  }
}

/** @brief Writes the score of a pilot, its last digit at STATUS_SCORE_END,
 * padded to STATUS_SCORE_DIGITS digits (see paintPaddedScore()). */
void paintScore(IInterfaceCanvas& canvas, const PilotStatus& pilot,
                sf::Vector2i bay) {
  paintPaddedScore(canvas,
                   {.value = pilot.score,
                    .end = bay + STATUS_SCORE_END,
                    .digitColor = isGone(pilot) ? GONE_TEXT_COLOR : SCORE_COLOR,
                    .zeroColor = LEADING_ZERO_COLOR});
}

/** @returns The parts a bay shows in turn above the size the fonts are
 * drawn at: the name, the score, then the word, or the count of lives when
 * there is no word and the lives are known. */
std::vector<BayLine> largeBayLinesOf(const PilotStatus& pilot,
                                     const PilotBay& bay, const BayText& text) {
  std::vector<BayLine> lines{
      BayLine{.text = text.name,
              .room = {},
              .style = {.font = TextFont::Regular,
                        .color = nameColorOf(pilot, bay)}},
      BayLine{
          .text = text.score,
          .room = {},
          .style = {.font = TextFont::Regular,
                    .color = isGone(pilot) ? GONE_TEXT_COLOR : SCORE_COLOR}}};
  const std::optional<StatusWord> word = statusWordOf(pilot);
  if (word.has_value()) {
    lines.push_back(
        BayLine{.text = word->text,
                .room = {},
                .style = {.font = word->font, .color = word->color}});
  } else if (pilot.lives.has_value()) {
    lines.push_back(
        BayLine{.text = text.lives,
                .room = {},
                .style = {.font = TextFont::Regular, .color = SCORE_COLOR}});
  }
  return lines;
}

/** @brief Paints the one line of a bay above the size the fonts are drawn
 * at: the part of its turn, from the name's place to the right edge of the
 * word, on the middle row of the band. */
void paintLargeLine(IInterfaceCanvas& canvas, const PilotStatus& pilot,
                    const PaintedBay& bay) {
  const BayText text{
      .name = pilotNameOf(pilot.nickname, pilot.playerNumber),
      .score = std::to_string(std::min(pilot.score, LARGEST_SHOWN_SCORE)),
      .lives = LIVES_COUNT_PREFIX + std::to_string(pilot.lives.value_or(0))};
  std::vector<BayLine> lines = largeBayLinesOf(pilot, bay.bay, text);
  BayLine line = lines[turnAt(bay.look.sinceStart) % lines.size()];
  const int lineHeight = lineHeightOf(canvas.textMetrics());
  line.room = {
      {bay.bay.topLeft.x + STATUS_NAME_TOP_LEFT.x,
       bay.bay.topLeft.y + ((STATUS_BAND_ROWS - lineHeight) / SIDES_PER_AXIS)},
      {STATUS_WORD_END.x - STATUS_NAME_TOP_LEFT.x, lineHeight}};
  paintBayLine(canvas, line, bay.look);
}

/** @brief Paints the bay of a pilot: its frame when it is the local player,
 * its chip, then its name and word, its lives and its score on two rows, or
 * one line of them in turn above the size the fonts are drawn at. */
void paintBay(IInterfaceCanvas& canvas, const PilotStatus& pilot,
              const StatusBandLook& look) {
  const PaintedBay bay{.bay = pilotBayOf(pilot, look.palette.get()),
                       .look = look};
  if (pilot.isLocalPlayer) {
    Outline({bay.bay.topLeft + STATUS_LOCAL_FRAME.position,
             STATUS_LOCAL_FRAME.size},
            Outline::Line::Solid)
        .paint(canvas, LOCAL_BAY_FRAME_COLOR);
  }
  paintChip(canvas, pilot, bay.bay);
  if (canvas.textMetrics() != TextMetrics{}) {
    paintLargeLine(canvas, pilot, bay);
    return;
  }
  paintTitle(canvas, pilot, bay);
  paintLives(canvas, pilot, bay.bay);
  paintScore(canvas, pilot, bay.bay.topLeft);
}

}  // namespace

void paintStatusBand(IInterfaceCanvas& canvas,
                     std::span<const PilotStatus> pilots,
                     const StatusBandLook& look) {
  paintPanel(canvas);
  for (const PilotStatus& pilot : pilots) {
    if (hasBay(pilot)) {
      paintBay(canvas, pilot, look);
    }
  }
}

bool statusBandMoves(std::span<const PilotStatus> pilots,
                     const IInterfaceCanvas& canvas) {
  if (canvas.textMetrics() != TextMetrics{}) {
    return true;
  }
  return std::ranges::any_of(pilots, [&canvas](const PilotStatus& pilot) {
    return hasBay(pilot) &&
           canvas.textWidth(pilotNameOf(pilot.nickname, pilot.playerNumber),
                            TextFont::Regular) >
               nameRoom(canvas, statusWordOf(pilot));
  });
}

}  // namespace rtype::client
