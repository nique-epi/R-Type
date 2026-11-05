#pragma once

#include <vector>
#include "ServerSample.hpp"

namespace rtype::network {

/** @brief One page of the samples of a server, newest first: the answer of
 * `GET /api/servers/{id}/samples`. */
struct SampleListing {
  /** @brief MAXIMUM_PAGE_SIZE at most. */
  std::vector<ServerSample> samples;

  bool operator==(const SampleListing&) const = default;
};

}  // namespace rtype::network
