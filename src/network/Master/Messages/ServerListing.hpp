#pragma once

#include <vector>
#include "ListedServer.hpp"

namespace rtype::network {

/** @brief One page of the registered servers, by id: the answer of
 * `GET /api/servers`. */
struct ServerListing {
  /** @brief MAXIMUM_PAGE_SIZE at most. */
  std::vector<ListedServer> servers;

  bool operator==(const ServerListing&) const = default;
};

}  // namespace rtype::network
