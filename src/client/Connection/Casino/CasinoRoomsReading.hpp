#pragma once

#include <optional>
#include "CasinoRoomListing.hpp"

namespace rtype::client {

/** @brief What the master answered to an ask for the rooms of the casino:
 * nothing when it did not answer, refused, or answered what the client cannot
 * read. */
struct CasinoRoomsReading {
  std::optional<network::CasinoRoomListing> listing;
};

}  // namespace rtype::client
