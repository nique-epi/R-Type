#pragma once

#include <string_view>

namespace rtype::server {

/** @brief Logger name the information module writes its lines under. */
constexpr std::string_view INFORMATION_LOGGER_NAME = "Information";

/** @brief The name a server announces while server-name is left as it is,
 * and why a name is refused. */
constexpr std::string_view DEFAULT_SERVER_NAME = "R-Type server";
constexpr std::string_view INVALID_SERVER_NAME_TEXT =
    "a server name is 1 to 32 bytes of printable UTF-8";

/** @brief The game version the server announces, from the CMake project. */
constexpr std::string_view SERVER_GAME_VERSION = RTYPE_GAME_VERSION;

/** @brief Why the ServerInformationDesk drops a datagram, as its lines say. */
constexpr std::string_view NO_INFORMATION_REQUEST_DROP_REASON =
    "its header is not of this protocol and version, or it holds no "
    "information request";

}  // namespace rtype::server
