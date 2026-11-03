#pragma once

#include <chrono>
#include <cstddef>
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief PlayerInput the game sends per second, one per frame at most.
 * Provisional, see the protocol page.
 */
constexpr std::size_t PLAYER_INPUTS_PER_SECOND = 60;

/** @brief Time between two PlayerInput of the game. */
constexpr engine::Duration PLAYER_INPUT_INTERVAL =
    std::chrono::duration_cast<engine::Duration>(std::chrono::seconds(1)) /
    PLAYER_INPUTS_PER_SECOND;

}  // namespace rtype::client
