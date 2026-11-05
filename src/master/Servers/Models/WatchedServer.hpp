#pragma once

#include <cstdint>
#include "ServerStatus.hpp"
#include "StatusEvidence.hpp"

namespace rtype::master {

/** @brief A registered server whose status the master refreshes: what the
 * status is computed from, and the status it holds now. */
struct WatchedServer {
  std::int64_t serverId{};
  StatusEvidence evidence;
  network::ServerStatus status{network::ServerStatus::Offline};
};

}  // namespace rtype::master
