#pragma once

#include <chrono>
#include <cstdint>
#include "ServerHeartbeat.hpp"

namespace rtype::master {

/** @brief A heartbeat a registered server sent, and when it arrived. */
struct HeartbeatRecord {
  std::int64_t serverId{};
  network::ServerHeartbeat heartbeat;
  std::chrono::sys_seconds receivedAt;
};

}  // namespace rtype::master
