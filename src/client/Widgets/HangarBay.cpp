#include "HangarBay.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <initializer_list>
#include <optional>
#include <string_view>
#include "CutCornerBox.hpp"
#include "EntitySpriteConstants.hpp"
#include "EntitySprites.hpp"
#include "HangarBayConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "ImagePlacement.hpp"
#include "InterfaceColorConstants.hpp"
#include "MotionPreference.hpp"
#include "SpriteFrame.hpp"
#include "SunkBox.hpp"
#include "TextStyle.hpp"
#include "TextureArea.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"

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
              .word = READY_TEXT,
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

/** @returns The frame of the ship of a color: its engines running while the
 * pilot is ready and the interface may move, its first frame otherwise. */
game::SpriteFrame shipFrameOf(int color, BayReadiness readiness,
                              BayMotion motion) {
  const bool enginesRun = motion.preference == MotionPreference::Full &&
                          readiness == BayReadiness::Ready;
  const game::SpriteFrame frame =
      enginesRun ? PLAYER_SHIP_ANIMATION.frameAt(motion.elapsed)
                       .value_or(PLAYER_SHIP_FRAME)
                 : PLAYER_SHIP_FRAME;
  return inPlayerColorRow(frame, color);
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
  CutCornerBox({bay.topLeft, HANGAR_BAY_SIZE},
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
void paintColorBand(IInterfaceCanvas& canvas, sf::Vector2i topLeft, int color) {
  canvas.fill(onBay(topLeft, BAY_COLOR_BAND),
              PLAYER_COLORS[static_cast<std::size_t>(color)].ship);
}

/** @brief Draws a ship frame BAY_SHIP_SCALE times its size, centered on the
 * bay and cut to it. */
void paintShip(IInterfaceCanvas& canvas, sf::Vector2i topLeft,
               game::SpriteFrame frame) {
  const int shipWidth = frame.width * BAY_SHIP_SCALE;
  const sf::Vector2i shipTopLeft(
      topLeft.x + ((HANGAR_BAY_SIZE.x - shipWidth) / SIDES_PER_AXIS),
      topLeft.y + BAY_SHIP_TOP);
  canvas.drawImage(
      TextureArea{.assetId = ENTITY_SHEET_ID, .rectangle = rectangleOf(frame)},
      ImagePlacement{.topLeft = shipTopLeft, .scale = BAY_SHIP_SCALE},
      {topLeft, HANGAR_BAY_SIZE});
}

/** @brief Writes the nickname of a pilot, centered on its row. */
void paintNickname(IInterfaceCanvas& canvas, sf::Vector2i topLeft,
                   std::string_view nickname) {
  canvas.write(nickname, middleAnchor(topLeft, BAY_NICKNAME_TOP),
               nicknameRowStyle(DIALOG_MESSAGE_COLOR));
}

/** @brief Writes OPEN BAY, dim, on the row of the nickname. */
void paintOpenBayLabel(IInterfaceCanvas& canvas, sf::Vector2i topLeft) {
  canvas.write(OPEN_BAY_TEXT, middleAnchor(topLeft, BAY_NICKNAME_TOP),
               nicknameRowStyle(FIELD_TEXT_COLOR));
}

/** @brief Paints the cell of a pilot: a sunk box, its fill, its word on the
 * middle row. */
void paintCell(IInterfaceCanvas& canvas, sf::Vector2i topLeft,
               BayReadiness readiness) {
  const sf::IntRect cell = onBay(topLeft, BAY_STATE_CELL);
  const CellLook look = cellLookOf(readiness);
  SunkBox(cell, SunkBox::Sides::Shaded).paint(canvas);
  if (look.fill.has_value()) {
    canvas.fill(insideOf(cell), *look.fill);
  }
  canvas.write(look.word,
               {centeredTextLeft(cell, look.word), centeredTextTop(cell)},
               {.font = TextFont::Regular, .color = look.text});
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
  paintColorBand(canvas, bay.topLeft, bay.color);
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

void paintHangarBay(IInterfaceCanvas& canvas, const HangarBay& bay,
                    BayMotion motion) {
  paintFrame(canvas, bay);
  paintPlayerNumber(canvas, bay);
  if (bay.pilot.has_value()) {
    paintPilot(canvas, bay, *bay.pilot);
    paintShip(canvas, bay.topLeft,
              shipFrameOf(bay.color, bay.pilot->readiness, motion));
  } else {
    paintOpenBay(canvas, bay.topLeft, motion);
  }
}

}  // namespace rtype::client
