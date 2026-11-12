#pragma once

#include <cstdint>
#include "CollisionBox.hpp"
#include "Position.hpp"

namespace rtype::client::testing {

/** @brief A level scrolling at 120 units a second, drawn at tick 100 while the
 * server is at tick 106: 6 ticks, a tenth of a second, so 12 units of lead. */
constexpr float SCROLL_SPEED_OF_A_LEVEL = 120.0F;
constexpr std::uint32_t TICK_DRAWN = 100;
constexpr double SERVER_TICK_SIX_AHEAD = 106.0;
constexpr float LEAD_OF_SIX_TICKS = 12.0F;
constexpr double SERVER_TICK_BEHIND = 98.0;

/** @brief A wall of 64 by 256 drawn at (600, 300), and where its box is once
 * moved ahead by 12 units. */
constexpr game::CollisionBox DRAWN_WALL_BOX{.width = 64.0F, .height = 256.0F};
constexpr game::Position WHERE_THE_WALL_IS_DRAWN{.x = 600.0F, .y = 300.0F};
constexpr float WALL_CENTER_AHEAD_X = 588.0F;

}  // namespace rtype::client::testing
