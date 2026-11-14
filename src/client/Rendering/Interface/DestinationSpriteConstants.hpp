#pragma once

#include "EntitySpriteConstants.hpp"
#include "SpriteFrame.hpp"

namespace rtype::client {

/**
 * @brief Asset id of the sheet of the destination screen: the planet of
 * SERVER, from Pixel Planets by Master484 (CC0) with an ink ring added, then
 * the yellow basket of SHOP, from Pixel Mart by ghostpixxells (CC0),
 * SPRITE_SPACING apart. HANGAR keeps the house of the sector map.
 */
constexpr const char* DESTINATION_SHEET_ID = "sprites/destinations.png";

constexpr game::SpriteFrame DESTINATION_PLANET_FRAME{
    .left = 0, .top = 0, .width = 30, .height = 30};
constexpr game::SpriteFrame DESTINATION_BASKET_FRAME{
    .left = DESTINATION_PLANET_FRAME.width + SPRITE_SPACING,
    .top = 0,
    .width = 30,
    .height = 31};

}  // namespace rtype::client
