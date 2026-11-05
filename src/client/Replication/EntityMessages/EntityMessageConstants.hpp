#pragma once

#include <cstddef>
#include <string_view>
#include "ProtocolConstants.hpp"

namespace rtype::client {

/** @brief How many rebuilt world states are kept: a delta reaches back at most
 * MAXIMUM_REFERENCE_DISTANCE ticks and the server sends a state at most once a
 * tick, so the state it reaches back to is among them. */
constexpr std::size_t REBUILT_STATE_HISTORY_SIZE =
    network::MAXIMUM_REFERENCE_DISTANCE;

/** @brief The name the world state assembler logs under. */
constexpr std::string_view ASSEMBLER_LOGGER_NAME = "Replication";

}  // namespace rtype::client
