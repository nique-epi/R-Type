#pragma once

#include <chrono>
#include <cstdint>
#include "ServerMeasures.hpp"

namespace rtype::network {

/** @brief What the master kept of the heartbeats of a server in one second. */
struct ServerSample {
  std::chrono::sys_seconds takenAt;
  std::uint16_t gameCount{};
  std::uint16_t playerCount{};
  ServerMeasures measures;

  bool operator==(const ServerSample&) const = default;
};

}  // namespace rtype::network
