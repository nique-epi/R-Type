#pragma once

#include <cstdint>
#include <optional>
#include "CasinoRoomListing.hpp"

namespace rtype::client {

/** @brief What the master answered to an ask for the rooms of the casino:
 * nothing when it did not answer, refused, or answered what the client cannot
 * read. */
struct CasinoRoomsReading {
  /** @brief The number of the ask it answers (see CasinoAsks). */
  std::uint64_t ask{};
  std::optional<network::CasinoRoomListing> listing;
};

}  // namespace rtype::client
