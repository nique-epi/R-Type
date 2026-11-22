#pragma once

#include <array>
#include "EntitySpriteConstants.hpp"
#include "SheetRow.hpp"
#include "ShipLookConstants.hpp"
#include "ShipLookSprite.hpp"
#include "SpriteFrame.hpp"

namespace rtype::client {

/**
 * @brief Asset id of the sheet of the accessories of the catalog: one cell
 * SHIP_LOOK_CELL large per accessory, in the order of SHIP_LOOK_ITEM_NAMES,
 * SPRITE_SPACING apart, each drawing against the top-left corner of its cell.
 * The hulls are not on it: they have their own sheet, SHIP_HULL_SHEET_ID.
 */
constexpr const char* SHIP_LOOK_SHEET_ID = "sprites/looks.png";
constexpr game::SpriteFrame SHIP_LOOK_CELL{
    .left = 0, .top = 0, .width = 16, .height = 16};
constexpr std::array SHIP_LOOK_CELLS =
    framesInRow<identifiers::SHIP_LOOK_ACCESSORY_COUNT>(SHIP_LOOK_CELL,
                                                        SPRITE_SPACING);

/** @brief The hats sit their brim on the top fin of the ship: centred on
 * column 16, their last row on row 2. The shades cover the canopy, columns 17
 * to 25 of rows 6 to 8; the mustache hangs under the nose. */
constexpr ShipLookSprite TOP_HAT_SPRITE{
    .label = {.english = "TOP HAT", .french = "HAUT-DE-FORME"},
    .size = {14, 10},
    .mount = {9, -7}};
constexpr ShipLookSprite PARTY_HAT_SPRITE{
    .label = {.english = "PARTY HAT", .french = "CHAPEAU DE FÊTE"},
    .size = {12, 11},
    .mount = {10, -8}};
constexpr ShipLookSprite CROWN_SPRITE{
    .label = {.english = "CROWN", .french = "COURONNE"},
    .size = {12, 8},
    .mount = {10, -5}};
constexpr ShipLookSprite PROPELLER_CAP_SPRITE{
    .label = {.english = "PROPELLER CAP", .french = "CASQUETTE HÉLICE"},
    .size = {12, 8},
    .mount = {10, -5}};
constexpr ShipLookSprite COWBOY_HAT_SPRITE{
    .label = {.english = "COWBOY HAT", .french = "CHAPEAU COW-BOY"},
    .size = {14, 8},
    .mount = {9, -5}};
constexpr ShipLookSprite VIKING_HELMET_SPRITE{
    .label = {.english = "VIKING HELMET", .french = "CASQUE VIKING"},
    .size = {14, 9},
    .mount = {9, -6}};
constexpr ShipLookSprite WIZARD_HAT_SPRITE{
    .label = {.english = "WIZARD HAT", .french = "CHAPEAU SORCIER"},
    .size = {12, 12},
    .mount = {10, -9}};
constexpr ShipLookSprite CHEF_TOQUE_SPRITE{
    .label = {.english = "CHEF TOQUE", .french = "TOQUE DE CHEF"},
    .size = {12, 9},
    .mount = {10, -6}};
constexpr ShipLookSprite COOL_SHADES_SPRITE{
    .label = {.english = "COOL SHADES", .french = "LUNETTES CLASSE"},
    .size = {10, 3},
    .mount = {17, 5}};
constexpr ShipLookSprite MUSTACHE_SPRITE{
    .label = {.english = "MUSTACHE", .french = "MOUSTACHE"},
    .size = {12, 5},
    .mount = {20, 9}};

/** @brief The second set of hats sits like the first, centred on column 16;
 * the 3D glasses and the monocle cover the canopy; the long beard hangs
 * under the nose like the mustache. */
constexpr ShipLookSprite CHAPKA_SPRITE{
    .label = {.english = "CHAPKA", .french = "CHAPKA"},
    .size = {14, 9},
    .mount = {9, -4}};
constexpr ShipLookSprite TRICORN_SPRITE{
    .label = {.english = "TRICORN", .french = "TRICORNE"},
    .size = {14, 8},
    .mount = {9, -5}};
constexpr ShipLookSprite SANTA_HAT_SPRITE{
    .label = {.english = "SANTA HAT", .french = "BONNET DE NOËL"},
    .size = {13, 9},
    .mount = {10, -6}};
constexpr ShipLookSprite HALO_SPRITE{
    .label = {.english = "HALO", .french = "AURÉOLE"},
    .size = {12, 6},
    .mount = {10, -5}};
constexpr ShipLookSprite DEVIL_HORNS_SPRITE{
    .label = {.english = "DEVIL HORNS", .french = "CORNES DU DIABLE"},
    .size = {13, 7},
    .mount = {10, -1}};
constexpr ShipLookSprite SOMBRERO_SPRITE{
    .label = {.english = "SOMBRERO", .french = "SOMBRERO"},
    .size = {16, 7},
    .mount = {8, -4}};
constexpr ShipLookSprite TRAFFIC_CONE_SPRITE{
    .label = {.english = "TRAFFIC CONE", .french = "CÔNE DE CHANTIER"},
    .size = {10, 9},
    .mount = {11, -6}};
constexpr ShipLookSprite FEZ_SPRITE{
    .label = {.english = "FEZ", .french = "FEZ"},
    .size = {12, 8},
    .mount = {11, -5}};
constexpr ShipLookSprite GRAD_CAP_SPRITE{
    .label = {.english = "GRAD CAP", .french = "TOQUE DE DIPLÔMÉ"},
    .size = {14, 9},
    .mount = {9, -6}};
constexpr ShipLookSprite ANTENNAE_SPRITE{
    .label = {.english = "ANTENNAE", .french = "ANTENNES"},
    .size = {12, 8},
    .mount = {10, -5}};
constexpr ShipLookSprite KABUTO_SPRITE{
    .label = {.english = "KABUTO", .french = "KABUTO"},
    .size = {12, 10},
    .mount = {10, -5}};
constexpr ShipLookSprite MOHAWK_SPRITE{
    .label = {.english = "MOHAWK", .french = "CRÊTE IROQUOISE"},
    .size = {12, 7},
    .mount = {10, -3}};
constexpr ShipLookSprite GLASSES_3D_SPRITE{
    .label = {.english = "3D GLASSES", .french = "LUNETTES 3D"},
    .size = {11, 4},
    .mount = {17, 5}};
constexpr ShipLookSprite MONOCLE_SPRITE{
    .label = {.english = "MONOCLE", .french = "MONOCLE"},
    .size = {6, 8},
    .mount = {20, 5}};
constexpr ShipLookSprite LONG_BEARD_SPRITE{
    .label = {.english = "LONG BEARD", .french = "LONGUE BARBE"},
    .size = {12, 9},
    .mount = {20, 9}};

constexpr std::array SHIP_LOOK_SPRITES{
    TOP_HAT_SPRITE,       PARTY_HAT_SPRITE,    CROWN_SPRITE,
    PROPELLER_CAP_SPRITE, COWBOY_HAT_SPRITE,   VIKING_HELMET_SPRITE,
    WIZARD_HAT_SPRITE,    CHEF_TOQUE_SPRITE,   COOL_SHADES_SPRITE,
    MUSTACHE_SPRITE,      CHAPKA_SPRITE,       TRICORN_SPRITE,
    SANTA_HAT_SPRITE,     HALO_SPRITE,         DEVIL_HORNS_SPRITE,
    SOMBRERO_SPRITE,      TRAFFIC_CONE_SPRITE, FEZ_SPRITE,
    GRAD_CAP_SPRITE,      ANTENNAE_SPRITE,     KABUTO_SPRITE,
    MOHAWK_SPRITE,        GLASSES_3D_SPRITE,   MONOCLE_SPRITE,
    LONG_BEARD_SPRITE};
static_assert(SHIP_LOOK_SPRITES.size() ==
                  identifiers::SHIP_LOOK_ACCESSORY_COUNT,
              "every accessory of the catalog has its drawing");

/** @brief The largest scale an icon of the locker is drawn at. */
constexpr int MAXIMUM_LOOK_ICON_SCALE = 3;

}  // namespace rtype::client
