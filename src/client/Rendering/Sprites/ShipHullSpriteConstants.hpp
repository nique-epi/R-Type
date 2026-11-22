#pragma once

#include <array>
#include <cstddef>
#include "EntitySpriteConstants.hpp"
#include "SheetRow.hpp"
#include "ShipLookConstants.hpp"
#include "SpriteFrame.hpp"
#include "Translation.hpp"

namespace rtype::client {

/**
 * @brief Asset id of the sheet of the hulls a pilot may fly in place of the
 * standard ship: one block per hull, in the order of the hulls of
 * SHIP_LOOK_ITEM_NAMES, each block PLAYER_COLOR_COUNT rows, one per pilot
 * color, of SPRITE_FRAME_COUNT frames the size of PLAYER_SHIP_FRAME,
 * SPRITE_SPACING apart both ways. Every hull keeps its top fin at column 16,
 * so what the ship wears sits where it does on the standard one.
 */
constexpr const char* SHIP_HULL_SHEET_ID = "sprites/hulls.png";
constexpr game::SpriteFrame SHIP_HULL_FRAME{.left = 0,
                                            .top = 0,
                                            .width = PLAYER_SHIP_FRAME.width,
                                            .height = PLAYER_SHIP_FRAME.height};
constexpr std::size_t SHIP_HULL_ROW_COUNT =
    identifiers::SHIP_HULL_COUNT * static_cast<std::size_t>(PLAYER_COLOR_COUNT);
constexpr std::array SHIP_HULL_ROWS =
    rowsOfFrames<SHIP_HULL_ROW_COUNT, SPRITE_FRAME_COUNT>(SHIP_HULL_FRAME,
                                                          SPRITE_SPACING);

/** @brief The name each hull is shown under, in the order of the hulls of
 * SHIP_LOOK_ITEM_NAMES. */
constexpr std::array<Translation, identifiers::SHIP_HULL_COUNT>
    SHIP_HULL_LABELS{
        Translation{.english = "WAR SHARK", .french = "REQUIN DE GUERRE"},
        Translation{.english = "VOSTOK", .french = "VOSTOK"},
        Translation{.english = "BYDO HUSK", .french = "CARCASSE BYDO"},
        Translation{.english = "GOLDEN ACE", .french = "AS EN OR"},
        Translation{.english = "GOLDEN BYDO", .french = "BYDO EN OR"}};

/** @brief The pilot color a hull shows in a catalogue, where no pilot
 * flies it yet: the first. */
constexpr int HULL_ITEM_COLOR = 0;

}  // namespace rtype::client
