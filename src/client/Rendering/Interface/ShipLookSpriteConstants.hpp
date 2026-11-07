#pragma once

#include <array>
#include "EntitySpriteConstants.hpp"
#include "SheetRow.hpp"
#include "ShipLookConstants.hpp"
#include "ShipLookSprite.hpp"
#include "SpriteFrame.hpp"

namespace rtype::client {

/**
 * @brief Asset id of the sheet of the items of the locker: one cell
 * SHIP_LOOK_CELL large per item, in the order of SHIP_LOOK_ITEM_NAMES,
 * SPRITE_SPACING apart, each drawing against the top-left corner of its cell.
 */
constexpr const char* SHIP_LOOK_SHEET_ID = "sprites/looks.png";
constexpr game::SpriteFrame SHIP_LOOK_CELL{
    .left = 0, .top = 0, .width = 16, .height = 16};
constexpr std::array SHIP_LOOK_CELLS =
    framesInRow<identifiers::SHIP_LOOK_ITEM_COUNT>(SHIP_LOOK_CELL,
                                                   SPRITE_SPACING);

/** @brief The hats sit their brim on the top fin of the ship: centred on
 * column 16, their last row on row 2. The shades cover the canopy, columns 17
 * to 25 of rows 6 to 8; the mustache hangs under the nose. */
constexpr ShipLookSprite TOP_HAT_SPRITE{
    .label = "TOP HAT", .size = {14, 10}, .mount = {9, -7}};
constexpr ShipLookSprite PARTY_HAT_SPRITE{
    .label = "PARTY HAT", .size = {12, 11}, .mount = {10, -8}};
constexpr ShipLookSprite CROWN_SPRITE{
    .label = "CROWN", .size = {12, 8}, .mount = {10, -5}};
constexpr ShipLookSprite PROPELLER_CAP_SPRITE{
    .label = "PROPELLER CAP", .size = {12, 8}, .mount = {10, -5}};
constexpr ShipLookSprite COWBOY_HAT_SPRITE{
    .label = "COWBOY HAT", .size = {14, 8}, .mount = {9, -5}};
constexpr ShipLookSprite VIKING_HELMET_SPRITE{
    .label = "VIKING HELMET", .size = {14, 9}, .mount = {9, -6}};
constexpr ShipLookSprite WIZARD_HAT_SPRITE{
    .label = "WIZARD HAT", .size = {12, 12}, .mount = {10, -9}};
constexpr ShipLookSprite CHEF_TOQUE_SPRITE{
    .label = "CHEF TOQUE", .size = {12, 9}, .mount = {10, -6}};
constexpr ShipLookSprite COOL_SHADES_SPRITE{
    .label = "COOL SHADES", .size = {10, 3}, .mount = {17, 5}};
constexpr ShipLookSprite MUSTACHE_SPRITE{
    .label = "MUSTACHE", .size = {12, 5}, .mount = {20, 9}};

constexpr std::array SHIP_LOOK_SPRITES{TOP_HAT_SPRITE,     PARTY_HAT_SPRITE,
                                       CROWN_SPRITE,       PROPELLER_CAP_SPRITE,
                                       COWBOY_HAT_SPRITE,  VIKING_HELMET_SPRITE,
                                       WIZARD_HAT_SPRITE,  CHEF_TOQUE_SPRITE,
                                       COOL_SHADES_SPRITE, MUSTACHE_SPRITE};
static_assert(SHIP_LOOK_SPRITES.size() == identifiers::SHIP_LOOK_ITEM_COUNT,
              "every item of the locker has its drawing");

/** @brief The largest scale an icon of the locker is drawn at. */
constexpr int MAXIMUM_LOOK_ICON_SCALE = 3;

}  // namespace rtype::client
