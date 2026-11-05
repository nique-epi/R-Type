#pragma once

#include <cstdint>
#include "ServerStatusChange.hpp"

namespace rtype::master {

/** @brief A change of status of a server, as the table status_changes keeps
 * it. */
struct StatusChangeRecord {
  std::int64_t serverId{};
  network::ServerStatusChange change;
};

}  // namespace rtype::master
