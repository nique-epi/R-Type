#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include "Endpoint.hpp"

namespace rtype::server {

/** @brief An encoded LeaderboardAnswer, on its way to the player who asked. */
struct LeaderboardReply {
  network::Endpoint recipient;
  /** @brief The session it was asked in: an address alone may be another
   * player by the time the answer is ready. */
  std::uint64_t sessionToken{};
  std::vector<std::byte> payload;
};

}  // namespace rtype::server
