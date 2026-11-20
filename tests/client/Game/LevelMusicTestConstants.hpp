#pragma once

#include <cstddef>
#include <string_view>

/** @brief A biome whose background names a music, the music it names, and a
 * biome whose background names none. */
constexpr std::string_view WESTERN_BIOME = "deseret";
constexpr std::string_view WESTERN_MUSIC = "music/big_iron.mp3";
constexpr std::string_view SILENT_BIOME = "space";

/** @brief How many times the music of a game is asked for. */
constexpr std::size_t ASKED_ONCE = 1;
constexpr std::size_t ASKED_TWICE = 2;
