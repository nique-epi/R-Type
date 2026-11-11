#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>
#include "TimeConstants.hpp"

/** @brief A picture two skies wide, 960 pixels on a sky of 480, and one
 * narrower than the sky. */
constexpr int PICTURE_TWO_SKIES_WIDE = 960;
constexpr int PICTURE_NARROWER_THAN_THE_SKY = 200;
constexpr int PICTURE_WITHOUT_WIDTH = 0;

/** @brief Scrolls of a picture, in background pixels: none, a tenth of a
 * sky and a little more, more than a sky, and the whole picture. */
constexpr float NO_SCROLL = 0.0F;
constexpr float SCROLL_OF_A_HUNDRED_PIXELS = 100.0F;
constexpr float SCROLL_OF_A_HUNDRED_PIXELS_AND_MORE = 100.9F;
constexpr float SCROLL_OF_SIX_HUNDRED_PIXELS = 600.0F;
constexpr float SCROLL_OF_THE_WHOLE_PICTURE = 960.0F;

/** @brief Where the copies of a picture start, in background pixels. */
constexpr int LEFT_EDGE_OF_THE_SKY = 0;
constexpr int A_HUNDRED_PIXELS_LEFT_OF_THE_SKY = -100;
constexpr int SIX_HUNDRED_PIXELS_LEFT_OF_THE_SKY = -600;
constexpr int SECOND_COPY_AFTER_SIX_HUNDRED_PIXELS = 360;
constexpr int SECOND_NARROW_COPY = 200;
constexpr int THIRD_NARROW_COPY = 400;

/** @brief A biome the client has a decor for, of the four layers a biome
 * has, the third biome of the levels, and a name no biome has. */
constexpr std::string_view KNOWN_BIOME = "drift_fields";
constexpr std::size_t LAYERS_OF_THE_KNOWN_BIOME = 4;
constexpr std::string_view THIRD_BIOME = "hive_depths";
constexpr std::string_view UNKNOWN_BIOME = "nowhere";

/** @brief A level scrolled 100 logical units that scrolls 120 units per
 * second, where it is one second later, and a level whose scroll is
 * paused. */
constexpr float SHOWN_SCROLL_POSITION = 100.0F;
constexpr float SHOWN_SCROLL_SPEED = 120.0F;
constexpr float SCROLL_POSITION_ONE_SECOND_LATER = 220.0F;
constexpr float PAUSED_SCROLL = 0.0F;
constexpr rtype::engine::Duration ONE_SECOND_OF_DECOR = std::chrono::seconds(1);
constexpr rtype::engine::Duration ONE_SECOND_BACKWARDS_OF_DECOR =
    std::chrono::seconds(-1);
