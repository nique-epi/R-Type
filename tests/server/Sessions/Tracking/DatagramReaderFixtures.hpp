#pragma once

#include <cstdint>
#include "IncomingDatagram.hpp"

namespace rtype::server::testing {

/** @returns A Heartbeat the player at index 0 sends in the datagram of
 * sequence @p sequence. */
network::IncomingDatagram heartbeatOfSequence(std::uint16_t sequence);

}  // namespace rtype::server::testing
