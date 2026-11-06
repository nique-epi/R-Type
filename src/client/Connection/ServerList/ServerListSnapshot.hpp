#pragma once

#include <vector>
#include "ServerListPhase.hpp"
#include "ServerRow.hpp"

namespace rtype::client {

/** @brief The list of servers as a screen reads it at a frame. */
struct ServerListSnapshot {
  ServerListPhase phase{ServerListPhase::Searching};
  /** @brief Lowest ping first, the servers with no ping last, then lowest id;
   * empty unless phase is Ready. */
  std::vector<ServerRow> servers;

  bool operator==(const ServerListSnapshot&) const = default;
};

}  // namespace rtype::client
