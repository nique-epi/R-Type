#pragma once

#include <string_view>

namespace rtype::server {

/**
 * @brief Module name the classes that let players in write their log lines
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

}  // namespace rtype::server
