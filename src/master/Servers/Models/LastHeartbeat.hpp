#pragma once

#include <optional>
#include "ServerHeartbeat.hpp"

namespace rtype::master {

/** @brief The last heartbeat of a server that exists: none while it has sent
 * none since it registered. */
struct LastHeartbeat {
  std::optional<network::ServerHeartbeat> heartbeat = std::nullopt;
};

}  // namespace rtype::master
