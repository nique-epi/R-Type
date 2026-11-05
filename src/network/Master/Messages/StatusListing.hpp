#pragma once

#include <vector>
#include "DatedStatus.hpp"

namespace rtype::network {

/** @brief One page of the statuses a server held, newest first: the answer of
 * `GET /api/servers/{id}/statuses`. */
struct StatusListing {
  /** @brief MAXIMUM_PAGE_SIZE at most. */
  std::vector<DatedStatus> statuses;

  bool operator==(const StatusListing&) const = default;
};

}  // namespace rtype::network
