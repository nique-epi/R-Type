#pragma once

#include <chrono>
#include "ServerStatus.hpp"

namespace rtype::network {

/** @brief A status a server held, from the second the master saw it take
 * it. */
struct DatedStatus {
  std::chrono::sys_seconds since;
  ServerStatus status{ServerStatus::Offline};

  bool operator==(const DatedStatus&) const = default;
};

}  // namespace rtype::network
