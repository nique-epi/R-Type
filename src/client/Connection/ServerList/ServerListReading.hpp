#pragma once

#include <cstdint>
#include <optional>
#include "ServerListing.hpp"

namespace rtype::client {

/** @brief What the master answered to an ask for the list of servers. */
struct ServerListReading {
  /** @brief The number of the ask it answers (see MasterAsks). */
  std::uint64_t ask{};
  /** @brief Every server the master lists; nothing when the master did not
   * answer, refused, or answered something it could not read. */
  std::optional<network::ServerListing> listing;
};

}  // namespace rtype::client
