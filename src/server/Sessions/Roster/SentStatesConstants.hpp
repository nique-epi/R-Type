#pragma once

#include <cstddef>
#include "ProtocolConstants.hpp"

namespace rtype::server {

/**
 * @brief How many sent states are kept per player: as many as the ticks a
 * delta reaches back, since a state is sent at most once a tick, so the state
 * a delta may use as its reference is still kept.
 */
constexpr std::size_t SENT_STATE_HISTORY_SIZE =
    network::MAXIMUM_REFERENCE_DISTANCE;

}  // namespace rtype::server
