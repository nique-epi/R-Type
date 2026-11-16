#pragma once

#include <chrono>
#include <string_view>

namespace rtype::client {

/** @brief The scheme masterUrlOf() writes before the address and the port of
 * the master. */
constexpr std::string_view MASTER_URL_SCHEME = "http://";

/**
 * @brief How long a round of pings waits for the answers of its servers once
 * the master listed them: a server that has not answered by then is
 * unreachable and is not listed. A server answers within one tick, so a ping
 * above it is no use to a player either.
 */
constexpr std::chrono::seconds PING_GIVE_UP_TIME{2};

}  // namespace rtype::client
