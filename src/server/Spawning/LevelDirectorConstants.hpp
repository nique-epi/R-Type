#pragma once

#include <string_view>

namespace rtype::server {

/** @brief Logger name the level director writes its log lines under. */
constexpr std::string_view LEVEL_DIRECTOR_LOGGER_NAME = "LevelDirector";

/**
 * @brief How long before the start of its scroll the biome a level opens in
 * began, in logical units. A client draws the entrance of a biome from the
 * right edge as the biome begins; an entrance of parallax 1 crosses the 480
 * background pixels of the screen and the 32 columns of its seam after 512
 * background pixels, 2048 units at 4 units a pixel. Twice that leaves room for
 * an entrance laid a little right of the start of its layer, or slower.
 */
constexpr float OPENING_BIOME_LEAD = 4096.0F;

}  // namespace rtype::server
