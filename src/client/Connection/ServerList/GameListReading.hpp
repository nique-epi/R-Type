#pragma once

#include <cstdint>
#include <optional>
#include "GameListing.hpp"

namespace rtype::client {

/** @brief What the master answered to an ask for the games of a server. */
struct GameListReading {
  /** @brief The number of the ask it answers (see MasterAsks). */
  std::uint64_t ask{};
  /** @brief The games of the server asked for; nothing when the master did not
   * answer, refused, or answered something it could not read. */
  std::optional<network::GameListing> listing;
};

}  // namespace rtype::client
