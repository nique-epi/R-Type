#pragma once

#include <vector>
#include "CasinoRoom.hpp"

namespace rtype::network {

/** @brief The answer of `GET /api/casino/rooms`: every room, by number. */
struct CasinoRoomListing {
  std::vector<CasinoRoom> rooms;

  bool operator==(const CasinoRoomListing&) const = default;
};

}  // namespace rtype::network
