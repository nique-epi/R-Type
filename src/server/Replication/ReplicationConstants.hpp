#pragma once

#include <string_view>

namespace rtype::server {

/**
 * @brief Module name the replication classes write their log lines under.
 */
constexpr std::string_view REPLICATION_LOGGER_NAME = "Replication";

/**
 * @brief What the replication module of a game is called in an error.
 */
constexpr std::string_view REPLICATION_MODULE_NAME = "replication";

/**
 * @brief The largest playfield size for which the range of a wire coordinate
 * (-4096 to 4095.875 units) holds the playfield and one playfield size beyond
 * each edge, see the Position section of the protocol page.
 */
constexpr float LARGEST_PLAYFIELD_SIZE_ON_THE_WIRE = 2047.0F;

}  // namespace rtype::server
