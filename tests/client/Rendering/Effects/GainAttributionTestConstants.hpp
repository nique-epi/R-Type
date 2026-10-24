#pragma once

#include "Position.hpp"

/** @brief The colors just outside the pilots, on each side. */
constexpr int BEFORE_THE_FIRST_PILOT = -1;
constexpr int AFTER_THE_LAST_PILOT = 4;

/** @brief Where a Bydo died, and where a later one did. */
constexpr rtype::game::Position WHERE_IT_DIED{.x = 800.0F, .y = 400.0F};
constexpr rtype::game::Position WHERE_THE_LAST_DIED{.x = 900.0F, .y = 300.0F};
