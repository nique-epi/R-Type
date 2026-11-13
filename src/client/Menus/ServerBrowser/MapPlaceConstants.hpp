#pragma once

#include <array>
#include "HangarScreenIdConstants.hpp"
#include "MapPlace.hpp"
#include "SectorMapSpriteConstants.hpp"
#include "ServerMapScreenConstants.hpp"
#include "ShopScreenIdConstants.hpp"

namespace rtype::client {

/** @brief The hint of the bubble of a place, which names its key between
 * its start and its end. */
constexpr const char* MAP_PLACE_HINT_START = "CLICK OR ";
constexpr const char* MAP_PLACE_HINT_END = ": OPEN";

/** @brief The hangar, drawn as a house: it stands on the whole map close to
 * the planet, where no server stands (servers start past the reach of the
 * planet and its ring, 120 pixels from its centre), and its bubble opens
 * where the bubble of a server does. */
constexpr MapPlace HANGAR_PLACE{.point = {84, 64},
                                .frame = HANGAR_HOUSE_FRAME,
                                .label = HANGAR_LABEL,
                                .detail = "LOCKER AND SHIP",
                                .key = HANGAR_KEY,
                                .keyName = HANGAR_KEY_NAME,
                                .screen = HANGAR_SCREEN,
                                .bubbleOffset = BUBBLE_OFFSET};

/** @brief The shop, drawn as a kiosk, left of the hangar and of every
 * server; its bubble opens downwards, under the house of the hangar, so it
 * hides neither. */
constexpr MapPlace SHOP_PLACE{.point = {36, 62},
                              .frame = SHOP_KIOSK_FRAME,
                              .label = SHOP_LABEL,
                              .detail = "HATS AND SHIPS",
                              .key = SHOP_KEY,
                              .keyName = SHOP_KEY_NAME,
                              .screen = SHOP_SCREEN,
                              .bubbleOffset = {14, 8}};

/** @brief The places of the whole map. */
constexpr std::array MAP_PLACES{HANGAR_PLACE, SHOP_PLACE};

}  // namespace rtype::client
