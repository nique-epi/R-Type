#pragma once

#include <chrono>
#include "ProtocolConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Time between two PlayerInput of the game, one per frame at most:
 * the rate of the protocol, network::PLAYER_INPUTS_PER_SECOND.
 */
constexpr engine::Duration PLAYER_INPUT_INTERVAL =
    std::chrono::duration_cast<engine::Duration>(std::chrono::seconds(1)) /
    network::PLAYER_INPUTS_PER_SECOND;

}  // namespace rtype::client
