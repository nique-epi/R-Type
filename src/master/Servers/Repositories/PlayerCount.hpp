#pragma once

#include <cstdint>
#include "ServerHeartbeat.hpp"

namespace rtype::master {

/** @returns How many players the games of @p heartbeat hold together. */
[[nodiscard]] std::int64_t playerCountOf(
    const network::ServerHeartbeat& heartbeat);

}  // namespace rtype::master
