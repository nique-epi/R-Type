#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::server {

/**
 * @brief Module name the replication classes write their log lines under.
 */
constexpr std::string_view REPLICATION_LOGGER_NAME = "Replication";

/**
 * @brief The largest playfield size for which the range of a wire coordinate
 * (-4096 to 4095.875 units) holds the playfield and one playfield size beyond
 * each edge, see the Position section of the protocol page.
 */
constexpr float LARGEST_PLAYFIELD_SIZE_ON_THE_WIRE = 2047.0F;

/**
 * @brief The lives the world state gives every player. Provisional: the server
 * does not count lives yet, so a player never loses one.
 */
constexpr std::uint8_t PLAYER_LIVES_UNTRACKED = 3;

}  // namespace rtype::server
