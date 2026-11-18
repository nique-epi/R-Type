#pragma once

#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstddef>
#include <string_view>
#include "BackgroundPace.hpp"

/** @brief A sky of two layers: ten far stars that drift 4.5 pixels per
 * second and warp, then the planet, which drifts 1.8 pixels per second and
 * does not. */
constexpr std::string_view A_SMALL_SKY = R"(
[[layer]]
name = "far stars"
motion = { autoscroll = 4.5, repeat = 488, warps = true }

[layer.particles]
population = 10
area = { left = 0, top = 0, width = 480, height = 270 }
pixel = "deep_blue"

[[layer]]
name = "planet"
motion = { autoscroll = 1.8, repeat = 647 }
image = { file = "backgrounds/space/planet.png", x = 288, y = 43 }
)";
constexpr std::size_t LAYERS_OF_THE_SMALL_SKY = 2;
constexpr std::size_t STAR_LAYER = 0;
constexpr std::size_t PLANET_LAYER_OF_THE_SKY = 1;
constexpr sf::Vector2i WHERE_THE_PLANET_STARTS(288, 43);

/** @brief Where they lie after ten seconds: the planet 18 pixels to the
 * left, the stars 45, or 180 when they warp at four times their speed. A
 * speed read from a file is a float, so a place is right to a thousandth of
 * a pixel. */
constexpr float PLANET_AFTER_TEN_SECONDS = -18.0F;
constexpr float STARS_AFTER_TEN_SECONDS_OF_WARP = -180.0F;
constexpr float ROUNDING_OF_A_PLACE = 0.001F;
constexpr rtype::client::BackgroundPace AT_FOUR_TIMES_THE_SPEED{
    .scrolled = 0.0F, .speedFactor = 4.0F};

/** @brief After one second the planet moved 1.8 pixels: it is painted one
 * column to the left, and lies 0.8 pixel further left. */
constexpr int PLANET_AFTER_ONE_SECOND = -1;
constexpr float SHIFT_OF_THE_PLANET_AFTER_ONE_SECOND = -0.8F;
constexpr float ROUNDING_OF_A_SHIFT = 0.0001F;

/** @brief The streak of the far stars: none up to one and a half times
 * their speed; at forty times, 4.5 pixels per second cover 3 pixels in a
 * tick of a sixtieth of a second. */
constexpr rtype::client::BackgroundPace AT_ONE_AND_A_HALF_TIMES_THE_SPEED{
    .scrolled = 0.0F, .speedFactor = 1.5F};
constexpr rtype::client::BackgroundPace AT_FORTY_TIMES_THE_SPEED{
    .scrolled = 0.0F, .speedFactor = 40.0F};
constexpr int STREAK_AT_FORTY_TIMES_THE_SPEED = 3;

/** @brief The Pit the repository ships: its far walls follow a quarter of
 * the scroll, its middle walls half, its near walls, the entrance, all of
 * it. */
constexpr std::string_view THE_PIT = "fosse";
constexpr std::size_t LAYERS_OF_THE_SHIPPED_PIT = 3;
constexpr std::size_t FAR_WALLS = 0;
constexpr std::size_t MIDDLE_WALLS = 1;
constexpr std::size_t NEAR_WALLS = 2;

/** @brief Where its layers stand once the level scrolled 100 background
 * pixels since it began: 25 and 50 columns to the left, and the entrance,
 * which starts on the right edge of the 480 columns, on 380. */
constexpr rtype::client::BackgroundPace A_HUNDRED_PIXELS_IN{.scrolled = 100.0F};
constexpr int FAR_WALLS_A_HUNDRED_PIXELS_IN = -25;
constexpr int MIDDLE_WALLS_A_HUNDRED_PIXELS_IN = -50;
constexpr int RIGHT_EDGE_OF_THE_BACKGROUND = 480;
constexpr int ENTRANCE_A_HUNDRED_PIXELS_IN = 380;

/** @brief Ten pixels and a half in, the near walls are painted ten columns
 * in and lie on their pixels: they keep the grid of the playfield. */
constexpr rtype::client::BackgroundPace TEN_PIXELS_AND_A_HALF_IN{.scrolled =
                                                                     10.5F};
constexpr int ENTRANCE_TEN_PIXELS_IN = 470;

/** @brief The first tiles of the near walls begin on the column 112 of
 * their first map, so the entrance begins on 592 when the Pit begins. */
constexpr int WHERE_THE_PIT_BEGINS_TO_SHOW = 592;

/** @brief The sky the repository ships: its sky events are its layers 4 to
 * 7, born first after 600 ms: none after 11 frames of 50 ms, one after 13. */
constexpr std::string_view THE_SKY = "space";
constexpr std::size_t FIRST_EVENT_LAYER = 4;
constexpr std::size_t LAYERS_OF_THE_SHIPPED_SKY = 8;
constexpr int FRAMES_BEFORE_THE_FIRST_EVENT = 11;
constexpr int FRAMES_UNTIL_THE_FIRST_EVENT = 2;

/** @brief A time that goes backwards. */
constexpr std::chrono::seconds ONE_SECOND_BACKWARDS{-1};

/** @brief A layer of tiles whose project no folder holds. */
constexpr std::string_view WALLS_OF_A_MISSING_PROJECT = R"(
[[layer]]
name = "walls"
tiles = { project = "biomes/nowhere/nowhere.ldtk", layer = "Near", tileset = "biomes/fosse/fosse_near.png" }
)";
