#pragma once

#include <chrono>
#include <string_view>

namespace rtype::client {

/**
 * @brief The master the client reaches when the `master-url` setting names
 * none: `http://`, then network::LOOPBACK_IPV4_ADDRESS and
 * network::DEFAULT_MASTER_PORT joined by MASTER_PORT_SEPARATOR. The reads need
 * no key.
 */
constexpr std::string_view MASTER_URL_SCHEME = "http://";
constexpr std::string_view MASTER_PORT_SEPARATOR = ":";

/** @brief How long MasterLink sleeps before it looks again whether it must
 * stop, when no ask comes; a stop wakes it at once. */
constexpr std::chrono::seconds MASTER_ASK_WAIT{1};

/**
 * @brief How long a round of pings waits for the answers of its servers once
 * the master listed them: a server that has not answered by then is
 * unreachable and is not listed. A server answers within one tick, so a ping
 * above it is no use to a player either.
 */
constexpr std::chrono::seconds PING_GIVE_UP_TIME{2};

}  // namespace rtype::client
