#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include "EntitySpriteConstants.hpp"
#include "SheetRow.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Asset id of the sheet of the map of the servers: a planet per server
 * the house of the hangar and the kiosk of the shop, at native size. Each glyph
 * holds one row, and frames and rows are SPRITE_SPACING pixels apart, as in the
 * entity sheet.
 */
constexpr const char* SECTOR_MAP_SHEET_ID = "sprites/sector_map.png";

/** @brief Sides of the glyphs, in pixels of the sheet: a planet grows with
 * the pilots of its server, and the house of the hangar, which the
 * destination screen draws. */
constexpr int SMALL_PLANET_SIZE = 5;
constexpr int MEDIUM_PLANET_SIZE = 7;
constexpr int LARGE_PLANET_SIZE = 9;
constexpr int HANGAR_HOUSE_SIZE = 9;

/**
 * @brief The first frame of each row of the sheet, from the top: a planet
 * with no pilot, with a few, with a crowd, a slow server (a large planet in
 * an orange ring), a server of another version (dark and still), and the
 * house of the hangar.
 */
constexpr game::SpriteFrame SMALL_PLANET_FRAME{.left = 0,
                                               .top = 0,
                                               .width = SMALL_PLANET_SIZE,
                                               .height = SMALL_PLANET_SIZE};
constexpr game::SpriteFrame MEDIUM_PLANET_FRAME{
    .left = 0,
    .top = bottomOf(SMALL_PLANET_FRAME) + SPRITE_SPACING,
    .width = MEDIUM_PLANET_SIZE,
    .height = MEDIUM_PLANET_SIZE};
constexpr game::SpriteFrame LARGE_PLANET_FRAME{
    .left = 0,
    .top = bottomOf(MEDIUM_PLANET_FRAME) + SPRITE_SPACING,
    .width = LARGE_PLANET_SIZE,
    .height = LARGE_PLANET_SIZE};
constexpr game::SpriteFrame SLOW_PLANET_FRAME =
    frameUnder(LARGE_PLANET_FRAME, SPRITE_SPACING);
constexpr game::SpriteFrame OTHER_VERSION_PLANET_FRAME{
    .left = 0,
    .top = bottomOf(SLOW_PLANET_FRAME) + SPRITE_SPACING,
    .width = MEDIUM_PLANET_SIZE,
    .height = MEDIUM_PLANET_SIZE};
constexpr game::SpriteFrame HANGAR_HOUSE_FRAME{
    .left = 0,
    .top = bottomOf(OTHER_VERSION_PLANET_FRAME) + SPRITE_SPACING,
    .width = HANGAR_HOUSE_SIZE,
    .height = HANGAR_HOUSE_SIZE};

/**
 * @brief A planet turns: a light band runs across it over
 * PLANET_TURN_FRAME_COUNT frames, each shown PLANET_TURN_FRAME_TIME, so a
 * whole turn lasts 1.2 s. A server of another version stands still, on its
 * one frame.
 */
constexpr std::size_t PLANET_TURN_FRAME_COUNT = 8;
constexpr engine::Duration PLANET_TURN_FRAME_TIME =
    std::chrono::milliseconds(150);

/** @brief The frames of each planet, in the order they show. */
constexpr std::array SMALL_PLANET_FRAMES =
    framesInRow<PLANET_TURN_FRAME_COUNT>(SMALL_PLANET_FRAME, SPRITE_SPACING);
constexpr std::array MEDIUM_PLANET_FRAMES =
    framesInRow<PLANET_TURN_FRAME_COUNT>(MEDIUM_PLANET_FRAME, SPRITE_SPACING);
constexpr std::array LARGE_PLANET_FRAMES =
    framesInRow<PLANET_TURN_FRAME_COUNT>(LARGE_PLANET_FRAME, SPRITE_SPACING);
constexpr std::array SLOW_PLANET_FRAMES =
    framesInRow<PLANET_TURN_FRAME_COUNT>(SLOW_PLANET_FRAME, SPRITE_SPACING);
constexpr std::array OTHER_VERSION_PLANET_FRAMES{OTHER_VERSION_PLANET_FRAME};

}  // namespace rtype::client
