#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::network {

/** @brief What cpp-httplib gives in place of a port it could not bind. */
constexpr int NO_PORT_BOUND = -1;

/**
 * @brief The largest body of a request a route reads, 64 KiB: a heartbeat of
 * a server is a few KiB, and a larger body gets 413 without being kept whole,
 * where cpp-httplib would keep up to 100 MiB. A path no route answers gets 404
 * and its body is never read.
 */
constexpr std::size_t MAX_REQUEST_BODY_BYTES = std::size_t{64} * 1024;

/** @brief What the journal says when the HTTP server stops on its own. */
constexpr std::string_view LISTENING_LOST_TEXT =
    "the HTTP server stopped listening";

/** @brief The header a reverse proxy names the client of a request in. */
constexpr std::string_view FORWARDED_FOR_HEADER = "X-Forwarded-For";

/** @brief What separates two addresses of FORWARDED_FOR_HEADER, the last one
 * written by the proxy nearest the server. */
constexpr char FORWARDED_ADDRESS_SEPARATOR = ',';

/** @brief The blanks a proxy may write around an address of
 * FORWARDED_FOR_HEADER. */
constexpr std::string_view FORWARDED_ADDRESS_BLANKS = " \t";

/** @brief What the journal says of an exception that is no std::exception. */
constexpr std::string_view UNKNOWN_ERROR_TEXT = "unknown error";

}  // namespace rtype::network
