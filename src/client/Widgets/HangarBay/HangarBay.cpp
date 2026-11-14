#include "HangarBay.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "CutCornerBox.hpp"
#include "EntitySpriteConstants.hpp"
#include "HangarBayConstants.hpp"
#include "HangarBayLayout.hpp"
#include "IInterfaceCanvas.hpp"
#include "MotionPreference.hpp"
#include "PilotShipPicture.hpp"
#include "PlayerNumberLabel.hpp"
#include "ShipLookConstants.hpp"
#include "ShipLooks.hpp"
#include "ShipPicture.hpp"
#include "ShipPlacement.hpp"
#include "SpriteFrame.hpp"
#include "SunkBox.hpp"
#include "TextMeasures.hpp"
#include "TextMetrics.hpp"
#include "TextStyle.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"
#include "WrappedLines.hpp"

namespace rtype::client {

namespace {

/** @brief What the cell of a pilot shows: its fill, none for the sunk box
 * alone, its word and the color of the word. */
struct CellLook {
  std::optional<sf::Color> fill;
  const char* word{};
  sf::Color text;
};

/** @returns The look of the cell of a pilot in each state. */
CellLook cellLookOf(BayReadiness readiness) {
  switch (readiness) {
    case BayReadiness::Ready:
      return {.fill = CURRENT_CELL_COLOR,
              .word = READY_TEXT,
              .text = CURRENT_CELL_TEXT_COLOR};
    case BayReadiness::ReadyAsked:
      return {.fill = AWAITED_VALUE_COLOR,
              .word = READY_ASKED_TEXT,
              .text = CURRENT_CELL_TEXT_COLOR};
    case BayReadiness::NotReadyAsked:
      return {.fill = AWAITED_VALUE_COLOR,
              .word = NOT_READY_ASKED_TEXT,
              .text = CURRENT_CELL_TEXT_COLOR};
    case BayReadiness::NotReady:
      break;
  }
  return {.fill = std::nullopt,
          .word = NOT_READY_TEXT,
          .text = OTHER_CELL_TEXT_COLOR};
}

/** @returns A rectangle given from the top-left corner of a bay, placed on
 * the bay at @p topLeft. */
sf::IntRect onBay(sf::Vector2i topLeft, sf::IntRect area) {
  return {topLeft + area.position, area.size};
}

/** @returns The inside of a box, less its border on every side. */
sf::IntRect insideOf(sf::IntRect box) {
  const sf::Vector2i border(BORDER_THICKNESS, BORDER_THICKNESS);
  return {box.position + border, box.size - (border * SIDES_PER_AXIS)};
}

/** @returns The anchor of a line centered on the middle column of a bay, on
 * row @p top of the bay. */
sf::Vector2i middleAnchor(sf::Vector2i topLeft, int top) {
  return {topLeft.x + (HANGAR_BAY_SIZE.x / SIDES_PER_AXIS), topLeft.y + top};
}

/** @returns The style of the row of the nickname: small print, centered, in
 * @p color. */
TextStyle nicknameRowStyle(sf::Color color) {
  return {.font = TextFont::Small,
          .color = color,
          .alignment = TextAlignment::Center};
}

/** @returns The frame of the ship, in the row of the first color: its
 * engines running while the pilot is ready and the interface may move, its
 * first frame otherwise. */
game::SpriteFrame shipFrameOf(BayReadiness readiness, BayMotion motion) {
  const bool enginesRun = motion.preference == MotionPreference::Full &&
                          readiness == BayReadiness::Ready;
  return enginesRun ? PLAYER_SHIP_ANIMATION.frameAt(motion.elapsed)
                          .value_or(PLAYER_SHIP_FRAME)
                    : PLAYER_SHIP_FRAME;
}

/** @returns The beacon lit after motion.elapsed, from left to right for
 * BEACON_STEP each; none while the interface may not move. */
std::optional<int> litBeaconAt(BayMotion motion) {
  if (motion.preference != MotionPreference::Full) {
    return std::nullopt;
  }
  return static_cast<int>((motion.elapsed / BEACON_STEP) % BEACON_COUNT);
}

/** @brief Paints the frame of a bay: lit while a pilot holds it, off while it
 * is open. */
void paintFrame(IInterfaceCanvas& canvas, const HangarBay& bay) {
  const bool open = !bay.pilot.has_value();
  CutCornerBox({bay.topLeft, hangarBayLayoutOf(canvas.textMetrics()).size},
               {.border = open ? DISABLED_BAR_BORDER_COLOR : BAR_BORDER_COLOR,
                .inside = PANEL_INSIDE_COLOR})
      .paint(canvas);
}

/** @brief Writes the player number of a bay in its top-left corner, dim while
 * the bay is open. */
void paintPlayerNumber(IInterfaceCanvas& canvas, const HangarBay& bay) {
  const bool open = !bay.pilot.has_value();
  canvas.write(playerNumberLabel(bay.playerNumber),
               bay.topLeft + BAY_NUMBER_OFFSET,
               {.font = TextFont::Regular,
                .color = open ? FIELD_TEXT_COLOR : PAGE_TEXT_COLOR});
}

/** @brief Writes YOU at the end of the row of the player number. */
void paintLocalPlayerTag(IInterfaceCanvas& canvas, sf::Vector2i topLeft) {
  const sf::Vector2i tagEnd(topLeft.x + HANGAR_BAY_SIZE.x - BAY_NUMBER_OFFSET.x,
                            topLeft.y + BAY_NUMBER_OFFSET.y);
  canvas.write(LOCAL_PLAYER_TAG, tagEnd,
               {.font = TextFont::Regular,
                .color = PAGE_TEXT_COLOR,
                .alignment = TextAlignment::Right});
}

/** @brief Paints the band of the ship color under the row of the number. */
void paintColorBand(IInterfaceCanvas& canvas, const HangarBay& bay) {
  canvas.fill(
      onBay(bay.topLeft, hangarBayLayoutOf(canvas.textMetrics()).colorBand),
      bay.palette.get().at(static_cast<std::size_t>(bay.color)).ship);
}

/** @brief Draws a ship frame as many times its size as the layout says,
 * centered on the bay and cut to it, in the row of its color and the shades
 * of the palette, then what its pilot wears. */
void paintShip(IInterfaceCanvas& canvas, const HangarBay& bay,
               game::SpriteFrame frame) {
  const HangarBayLayout layout = hangarBayLayoutOf(canvas.textMetrics());
  const int shipWidth = frame.width * layout.shipScale;
  const ShipPlacement ship{
      .topLeft = sf::Vector2i(
          bay.topLeft.x + ((layout.size.x - shipWidth) / SIDES_PER_AXIS),
          bay.topLeft.y + layout.shipTop),
      .scale = layout.shipScale,
      .visibleArea = {bay.topLeft, layout.size}};
  paintPilotShip(canvas,
                 PilotShipPicture{.frame = frame,
                                  .color = bay.color,
                                  .hull = bay.pilot.has_value()
                                              ? bay.pilot->look.hull
                                              : identifiers::NO_SHIP_LOOK_ITEM,
                                  .palette = bay.palette},
                 ship);
  if (bay.pilot.has_value()) {
    paintShipLook(canvas, bay.pilot->look, ship);
  }
}

/** @returns @p text as the rows of the nickname hold it: one line as drawn
 * at the size the fonts are drawn at; above it, cut to the inside of the bay
 * in small print. */
std::vector<std::string> nicknameLinesOf(const IInterfaceCanvas& canvas,
                                         std::string_view text) {
  if (hangarBayLayoutOf(canvas.textMetrics()).nicknameLines == 1) {
    return {std::string(text)};
  }
  return wrappedLinesOf(
      text, {.width = BAY_COLOR_BAND.size.x, .font = TextFont::Small}, canvas);
}

/** @brief What the rows of the nickname of a bay write, and in which color. */
struct NicknameRows {
  std::string_view text;
  sf::Color color;
};

/** @brief Writes @p rows on the rows of the nickname, each line centered. */
void paintNicknameRows(IInterfaceCanvas& canvas, sf::Vector2i topLeft,
                       NicknameRows rows) {
  int top = hangarBayLayoutOf(canvas.textMetrics()).nicknameTop;
  for (const std::string& line : nicknameLinesOf(canvas, rows.text)) {
    canvas.write(line, middleAnchor(topLeft, top),
                 nicknameRowStyle(rows.color));
    top += lineHeightOf(canvas.textMetrics());
  }
}

/** @brief Writes the nickname of a pilot, centered on its rows. */
void paintNickname(IInterfaceCanvas& canvas, sf::Vector2i topLeft,
                   std::string_view nickname) {
  paintNicknameRows(canvas, topLeft,
                    {.text = nickname, .color = DIALOG_MESSAGE_COLOR});
}

/** @brief Writes OPEN BAY, dim, on the rows of the nickname. */
void paintOpenBayLabel(IInterfaceCanvas& canvas, sf::Vector2i topLeft) {
  paintNicknameRows(canvas, topLeft,
                    {.text = OPEN_BAY_TEXT, .color = FIELD_TEXT_COLOR});
}

/** @brief Writes the word of a cell, centered on it: on its middle row in the
 * regular font as drawn at the size the fonts are drawn at; above it, in
 * small print on as many lines as the cell needs, the lines centered on its
 * height. */
void paintCellWord(IInterfaceCanvas& canvas, sf::IntRect cell,
                   const CellLook& look) {
  const TextMetrics metrics = canvas.textMetrics();
  if (metrics == TextMetrics{}) {
    canvas.write(look.word,
                 {centeredTextLeft(cell, look.word, metrics),
                  centeredTextTop(cell, metrics)},
                 {.font = TextFont::Regular, .color = look.text});
    return;
  }
  const std::vector<std::string> lines = wrappedLinesOf(
      look.word, {.width = insideOf(cell).size.x, .font = TextFont::Small},
      canvas);
  const int lineHeight = lineHeightOf(metrics);
  int top = cell.position.y +
            ((cell.size.y - (static_cast<int>(lines.size()) * lineHeight)) /
             SIDES_PER_AXIS);
  for (const std::string& line : lines) {
    canvas.write(line, {cell.position.x + (cell.size.x / SIDES_PER_AXIS), top},
                 {.font = TextFont::Small,
                  .color = look.text,
                  .alignment = TextAlignment::Center});
    top += lineHeight;
  }
}

/** @brief Paints the cell of a pilot: a sunk box, its fill, its word on the
 * middle row. */
void paintCell(IInterfaceCanvas& canvas, sf::Vector2i topLeft,
               BayReadiness readiness) {
  const sf::IntRect cell =
      onBay(topLeft, hangarBayLayoutOf(canvas.textMetrics()).stateCell);
  const CellLook look = cellLookOf(readiness);
  SunkBox(cell, SunkBox::Sides::Shaded).paint(canvas);
  if (look.fill.has_value()) {
    canvas.fill(insideOf(cell), *look.fill);
  }
  paintCellWord(canvas, cell, look);
}

/** @brief Paints the docking clamps of an open bay: a stroke down each side
 * of the beacons, and an arm at each end of a stroke, pointing inwards. */
void paintClamps(IInterfaceCanvas& canvas, sf::Vector2i topLeft) {
  const int leftArmLeft = LEFT_CLAMP_COLUMN;
  const int rightArmLeft =
      RIGHT_CLAMP_COLUMN + BORDER_THICKNESS - CLAMP_ARM_LENGTH;
  const int bottomArmTop = CLAMP_TOP + CLAMP_HEIGHT - BORDER_THICKNESS;
  for (const int column : {LEFT_CLAMP_COLUMN, RIGHT_CLAMP_COLUMN}) {
    canvas.fill(
        onBay(topLeft, {{column, CLAMP_TOP}, {BORDER_THICKNESS, CLAMP_HEIGHT}}),
        CLAMP_COLOR);
  }
  for (const int armTop : {CLAMP_TOP, bottomArmTop}) {
    for (const int armLeft : {leftArmLeft, rightArmLeft}) {
      canvas.fill(onBay(topLeft, {{armLeft, armTop},
                                  {CLAMP_ARM_LENGTH, BORDER_THICKNESS}}),
                  CLAMP_COLOR);
    }
  }
}

/** @brief Paints the row of beacons of an open bay: @p lit in cyan, the
 * others off. */
void paintBeacons(IInterfaceCanvas& canvas, sf::Vector2i topLeft,
                  std::optional<int> lit) {
  for (int beacon = 0; beacon < BEACON_COUNT; ++beacon) {
    const sf::Vector2i position =
        FIRST_BEACON + sf::Vector2i(beacon * BEACON_SPACING, 0);
    canvas.fill(onBay(topLeft, {position, {BEACON_SIZE, BEACON_SIZE}}),
                beacon == lit ? BEACON_LIT_COLOR : BEACON_OFF_COLOR);
  }
}

/** @brief Paints who holds a bay: YOU for the local player, the band of its
 * color, its nickname and its cell. The ship, which moves, is painted
 * apart. */
void paintPilot(IInterfaceCanvas& canvas, const HangarBay& bay,
                const BayPilot& pilot) {
  if (pilot.isLocalPlayer) {
    paintLocalPlayerTag(canvas, bay.topLeft);
  }
  paintColorBand(canvas, bay);
  paintNickname(canvas, bay.topLeft, pilot.nickname);
  paintCell(canvas, bay.topLeft, pilot.readiness);
}

/** @brief Paints a bay nobody holds: its clamps, its beacons and OPEN BAY. */
void paintOpenBay(IInterfaceCanvas& canvas, sf::Vector2i topLeft,
                  BayMotion motion) {
  paintClamps(canvas, topLeft);
  paintBeacons(canvas, topLeft, litBeaconAt(motion));
  paintOpenBayLabel(canvas, topLeft);
}

}  // namespace

HangarBayLayout hangarBayLayoutOf(TextMetrics metrics) {
  if (metrics == TextMetrics{}) {
    return {.size = HANGAR_BAY_SIZE,
            .colorBand = BAY_COLOR_BAND,
            .shipScale = BAY_SHIP_SCALE,
            .shipTop = BAY_SHIP_TOP,
            .nicknameTop = BAY_NICKNAME_TOP,
            .nicknameLines = 1,
            .stateCell = BAY_STATE_CELL};
  }
  const int lineHeight = lineHeightOf(metrics);
  const sf::IntRect band(
      {BAY_COLOR_BAND.position.x,
       BAY_NUMBER_OFFSET.y + lineHeight + LARGE_BAY_BAND_GAP},
      BAY_COLOR_BAND.size);
  const int shipTop = band.position.y + band.size.y + LARGE_BAY_SHIP_GAP;
  const int nicknameTop =
      std::max(shipTop + (PLAYER_SHIP_FRAME.height * LARGE_BAY_SHIP_SCALE),
               CLAMP_TOP + CLAMP_HEIGHT) +
      LARGE_BAY_PART_GAP;
  const sf::IntRect cell(
      {BAY_STATE_CELL.position.x, nicknameTop +
                                      (LARGE_BAY_NICKNAME_LINES * lineHeight) +
                                      LARGE_BAY_PART_GAP},
      {BAY_STATE_CELL.size.x,
       (LARGE_BAY_CELL_LINES * lineHeight) + LARGE_BAY_CELL_PADDING});
  return {.size = {HANGAR_BAY_SIZE.x,
                   cell.position.y + cell.size.y + BAY_SPACE_UNDER_CELL},
          .colorBand = band,
          .shipScale = LARGE_BAY_SHIP_SCALE,
          .shipTop = shipTop,
          .nicknameTop = nicknameTop,
          .nicknameLines = LARGE_BAY_NICKNAME_LINES,
          .stateCell = cell};
}

void paintHangarBay(IInterfaceCanvas& canvas, const HangarBay& bay,
                    BayMotion motion) {
  paintFrame(canvas, bay);
  paintPlayerNumber(canvas, bay);
  if (bay.pilot.has_value()) {
    paintPilot(canvas, bay, *bay.pilot);
    paintShip(canvas, bay, shipFrameOf(bay.pilot->readiness, motion));
  } else {
    paintOpenBay(canvas, bay.topLeft, motion);
  }
}

}  // namespace rtype::client
