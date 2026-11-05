#pragma once

#include <vector>
#include "ServerStatusChange.hpp"

namespace rtype::network {

/** @brief One page of the changes of status of a server, newest first: the
 * answer of `GET /api/servers/{id}/status-changes`. */
struct StatusChangeListing {
  /** @brief MAXIMUM_PAGE_SIZE at most. */
  std::vector<ServerStatusChange> statusChanges;

  bool operator==(const StatusChangeListing&) const = default;
};

}  // namespace rtype::network
