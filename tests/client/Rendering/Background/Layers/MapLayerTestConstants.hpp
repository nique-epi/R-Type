#pragma once

#include <cstddef>
#include <string_view>
#include "LayerFrame.hpp"

/** @brief Two maps of a layer, 300 then 400 pixels wide, the first holding
 * one tile, which begins on its column 32, the second none, and the tileset
 * their triangles are cut from. */
constexpr int FIRST_MAP_WIDTH = 300;
constexpr int SECOND_MAP_WIDTH = 400;
constexpr std::string_view WALL_TILESET = "biomes/fosse/fosse_near.png";
constexpr std::size_t NO_VERTEX = 0;
constexpr int COLUMN_OF_THE_FIRST_TILE = 32;

/** @brief Where the maps show when the layer lies at its origin: the first
 * on 0, the second on 300; the first again on 700 is right of the
 * background, 480 pixels wide. */
constexpr int SECOND_MAP_AT_THE_ORIGIN = 300;

/** @brief Where they show once the layer moved 350 columns to the left: the
 * second map on -50, then the first again on 350. */
constexpr rtype::client::LayerFrame PAST_THE_FIRST_MAP{.column = -350};
constexpr int SECOND_MAP_PARTLY_GONE = -50;
constexpr int FIRST_MAP_AGAIN = 350;
