#pragma once

#include <string_view>

namespace rtype::client::testing {

/** @brief The start of the opening sky tests: a level opening in the caves,
 * its scroll at 240 units, the caves begun 4096 units before the start of the
 * scroll, and the music the background of the caves names. */
constexpr std::string_view OPENING_BIOME = "caves";
constexpr float OPENING_SCROLL = 240.0F;
constexpr float OPENING_BIOME_SINCE = -4096.0F;
constexpr std::string_view CAVES_MUSIC = "music/caves.mp3";

}  // namespace rtype::client::testing
