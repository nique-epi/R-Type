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
  /** @brief Whether it carried a valid token of a probe, so receivedAt
   * becomes the last answer of its server. */
  bool probeAnswered{false};
};

}  // namespace rtype::master
