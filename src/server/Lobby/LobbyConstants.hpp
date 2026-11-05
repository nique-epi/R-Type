#pragma once

#include <string_view>
#include "ProtocolConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief Logger name the classes that let players in write their log lines
 * under.
 */
constexpr std::string_view LOBBY_LOGGER_NAME = "Lobby";

/**
 * @brief Why the ConnectionAcceptor drops a datagram, as its lines say.
 */
constexpr std::string_view SHORT_OTHER_VERSION_DROP_REASON =
    "it is of another protocol version, and too short to answer";
constexpr std::string_view NO_REQUEST_DROP_REASON =
    "its header is not of this protocol and version, or it holds no "
    "connection request";

/**
 * @brief The least time between two ConnectionAccepted to the same connected
 * player: half the time between two requests of the client. A request whose
 * acceptance was lost comes about CONNECTION_REQUEST_INTERVAL after the
 * previous one, earlier when it crossed the network faster; half of it lets
 * such a request through unless its trip was 250 ms shorter, and still sends a
 * player who floods valid requests at most 4 acceptances per second.
 */
constexpr engine::Duration CONNECTION_ACCEPTANCE_REPEAT_INTERVAL =
    network::CONNECTION_REQUEST_INTERVAL / 2;

}  // namespace rtype::server
