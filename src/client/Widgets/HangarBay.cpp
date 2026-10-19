#include "HangarBay.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <initializer_list>
#include <optional>
#include <string>
#include "CutCornerBox.hpp"
#include "EntitySpriteConstants.hpp"
#include "EntitySprites.hpp"
#include "HangarBayConstants.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceColors.hpp"
#include "MotionPreference.hpp"
#include "SpriteFrame.hpp"
#include "SunkBox.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"
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

/** @returns The left of a line centered on the middle column of a bay. */
int middleOf(sf::Vector2i topLeft) {
  return topLeft.x + (HANGAR_BAY_SIZE.x / SIDES_PER_AXIS);
}

/** @returns The frame of the ship of a pilot, in its color row: its engines
 * running while it is ready and the interface may move, its first frame
 * otherwise. */
game::SpriteFrame shipFrameOf(const HangarBay& bay, const BayPilot& pilot,
                              engine::Duration elapsed,
                              MotionPreference motion) {
  game::SpriteFrame frame = PLAYER_SHIP_FRAME;
  if (motion == MotionPreference::Full &&
      pilot.readiness == BayReadiness::Ready) {
    frame = PLAYER_SHIP_ANIMATION.frameAt(elapsed).value_or(PLAYER_SHIP_FRAME);
  }
  return inPlayerColorRow(frame, bay.color);
}

/** @brief Paints the cell of a pilot: a sunk box, its fill, its word on the
 * middle row. */
void paintCell(IInterfaceCanvas& canvas, sf::Vector2i topLeft,
               BayReadiness readiness) {
  const sf::IntRect cell(topLeft + BAY_STATE_CELL.position,
                         BAY_STATE_CELL.size);
  SunkBox(cell, SunkBox::Sides::Shaded).paint(canvas);
  const CellLook look = cellLookOf(readiness);
  if (look.fill.has_value()) {
    const sf::Vector2i border(BORDER_THICKNESS, BORDER_THICKNESS);
    canvas.fill({cell.position + border, cell.size - (border * SIDES_PER_AXIS)},
                *look.fill);
  }
  canvas.write(look.word,
               {centeredTextLeft(cell, look.word), centeredTextTop(cell)},
               {.font = TextFont::Regular, .color = look.text});
}

/** @brief Paints the tag of the local player, the band, the ship, the
 * nickname and the cell of a pilot. */
void paintPilot(IInterfaceCanvas& canvas, sf::Vector2i topLeft,
                const HangarBay& bay, const BayPilot& pilot,
                engine::Duration elapsed, MotionPreference motion) {
  if (pilot.isLocalPlayer) {
    canvas.write(LOCAL_PLAYER_TAG,
                 {topLeft.x + HANGAR_BAY_SIZE.x - BAY_NUMBER_OFFSET.x,
                  topLeft.y + BAY_NUMBER_OFFSET.y},
                 {.font = TextFont::Regular,
                  .color = PAGE_TEXT_COLOR,
                  .alignment = TextAlignment::Right});
  }
  canvas.fill({topLeft + BAY_COLOR_BAND.position, BAY_COLOR_BAND.size},
              PLAYER_SHIP_COLORS[static_cast<std::size_t>(bay.color)]);
  const game::SpriteFrame frame = shipFrameOf(bay, pilot, elapsed, motion);
  const int shipWidth = frame.width * BAY_SHIP_SCALE;
  canvas.drawImage(
      ENTITY_SHEET_ID, rectangleOf(frame),
      {topLeft.x + ((HANGAR_BAY_SIZE.x - shipWidth) / SIDES_PER_AXIS),
       topLeft.y + BAY_SHIP_TOP},
      BAY_SHIP_SCALE, {topLeft, HANGAR_BAY_SIZE});
  canvas.write(pilot.nickname,
               {middleOf(topLeft), topLeft.y + BAY_NICKNAME_TOP},
               {.font = TextFont::Small,
                .color = DIALOG_MESSAGE_COLOR,
                .alignment = TextAlignment::Center});
  paintCell(canvas, topLeft, pilot.readiness);
}

/** @brief Paints the clamps and the beacons of an open bay, one beacon lit
 * while the interface may move. */
void paintBeacons(IInterfaceCanvas& canvas, sf::Vector2i topLeft,
                  engine::Duration elapsed, MotionPreference motion) {
  for (const int column : {LEFT_CLAMP_COLUMN, RIGHT_CLAMP_COLUMN}) {
    canvas.fill({{topLeft.x + column, topLeft.y + CLAMP_TOP},
                 {BORDER_THICKNESS, CLAMP_HEIGHT}},
                CLAMP_COLOR);
  }
  for (const int row :
       {CLAMP_TOP, CLAMP_TOP + CLAMP_HEIGHT - BORDER_THICKNESS}) {
    canvas.fill({{topLeft.x + LEFT_CLAMP_COLUMN, topLeft.y + row},
                 {CLAMP_ARM_LENGTH, BORDER_THICKNESS}},
                CLAMP_COLOR);
    canvas.fill(
        {{topLeft.x + RIGHT_CLAMP_COLUMN - CLAMP_ARM_LENGTH + BORDER_THICKNESS,
          topLeft.y + row},
         {CLAMP_ARM_LENGTH, BORDER_THICKNESS}},
        CLAMP_COLOR);
  }
  const std::optional<int> lit =
      motion == MotionPreference::Full
          ? std::optional<int>(
                static_cast<int>((elapsed / BEACON_STEP) % BEACON_COUNT))
          : std::nullopt;
  for (int beacon = 0; beacon < BEACON_COUNT; ++beacon) {
    canvas.fill({{topLeft.x + FIRST_BEACON.x + (beacon * BEACON_SPACING),
                  topLeft.y + FIRST_BEACON.y},
                 {BEACON_SIZE, BEACON_SIZE}},
                beacon == lit ? BEACON_LIT_COLOR : BEACON_OFF_COLOR);
  }
}

}  // namespace

void paintHangarBay(IInterfaceCanvas& canvas, sf::Vector2i topLeft,
                    const HangarBay& bay, engine::Duration elapsed,
                    MotionPreference motion) {
  const bool open = !bay.pilot.has_value();
  CutCornerBox({topLeft, HANGAR_BAY_SIZE},
               {.border = open ? DISABLED_BAR_BORDER_COLOR : BAR_BORDER_COLOR,
                .inside = PANEL_INSIDE_COLOR})
      .paint(canvas);
  canvas.write(PLAYER_NUMBER_PREFIX + std::to_string(bay.playerNumber),
               topLeft + BAY_NUMBER_OFFSET,
               {.font = TextFont::Regular,
                .color = open ? FIELD_TEXT_COLOR : PAGE_TEXT_COLOR});
  if (bay.pilot.has_value()) {
    paintPilot(canvas, topLeft, bay, *bay.pilot, elapsed, motion);
    return;
  }
  paintBeacons(canvas, topLeft, elapsed, motion);
  canvas.write(OPEN_BAY_TEXT, {middleOf(topLeft), topLeft.y + BAY_NICKNAME_TOP},
               {.font = TextFont::Small,
                .color = FIELD_TEXT_COLOR,
                .alignment = TextAlignment::Center});
}

}  // namespace rtype::client
