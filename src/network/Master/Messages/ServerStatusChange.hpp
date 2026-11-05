#pragma once

#include <chrono>
#include "ServerStatus.hpp"

namespace rtype::network {

/** @brief The status a server took, and the second the master saw it. */
struct ServerStatusChange {
  std::chrono::sys_seconds changedAt;
  ServerStatus status{ServerStatus::Offline};

  bool operator==(const ServerStatusChange&) const = default;
};

}  // namespace rtype::network
