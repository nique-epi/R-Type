#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>
#include "TimeConstants.hpp"

/** @brief The biome the client has a decor for, its rank among the biomes,
 * and a name no biome has. */
constexpr std::string_view KNOWN_BIOME = "fosse";
constexpr std::size_t RANK_OF_THE_KNOWN_BIOME = 0;
constexpr std::string_view UNKNOWN_BIOME = "nowhere";

/** @brief A file no assets folder holds. */
constexpr std::string_view FILE_NOWHERE = "/nowhere/no_biome.ldtk";

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
