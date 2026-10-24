#pragma once

#include <chrono>
#include "TimeConstants.hpp"

/** @brief How many frames the tests read the camera over: a second at 60
 * frames a second. */
constexpr int FRAMES_IN_A_SECOND = 60;

/** @brief The most trauma the camera can take. */
constexpr float FULL_TRAUMA = 1.0F;

/** @brief The trauma of a big catch, and of a ship destroyed. */
constexpr float BIG_CATCH_TRAUMA_REQUIRED = 0.45F;
constexpr float SHIP_DOWN_TRAUMA_REQUIRED = 0.3F;

/** @brief A trauma that, added twice, passes the most. */
constexpr float HIGH_TRAUMA = 0.8F;

/** @brief The most pixels the camera may be off on an axis. */
constexpr int LARGEST_SHAKE = 6;

/** @brief Times on each side of the end of a trauma of 0.3, which loses 1.5 a
 * second: some is left after the first, none after the second. */
constexpr rtype::engine::Duration BEFORE_A_TRAUMA_OF_03_IS_GONE =
    std::chrono::milliseconds(150);
constexpr rtype::engine::Duration AFTER_A_TRAUMA_OF_03_IS_GONE =
    std::chrono::milliseconds(250);
