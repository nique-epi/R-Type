#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief The player of the announced start tests, and the start the server
 * tells it: a level opening in the caves, its scroll at 0, the caves begun
 * 4096 units before it. */
constexpr std::uint8_t STARTING_PLAYER_NUMBER = 1;
constexpr std::string_view ANNOUNCED_BIOME = "caves";
constexpr float ANNOUNCED_BIOME_SINCE = -4096.0F;

}  // namespace rtype::client::testing
