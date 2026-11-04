#pragma once

#include <cstdint>
#include <string>
#include "BoundedQueue.hpp"
#include "Endpoint.hpp"
#include "LeaderboardReply.hpp"
#include "LeaderboardRequest.hpp"

namespace rtype::server {

/** @brief A player asks for a leaderboard: what the storage thread needs to
 * read it and to hand the answer back. */
struct LeaderboardAsk {
  network::LeaderboardRequest request;
  std::string nickname;
  network::Endpoint asker;
  std::uint64_t sessionToken{};
  /**
   * @brief Where the answer goes. A pointer, since a BoundedQueue requires its
   * messages to have a default value; it stays valid while the storage thread
   * runs, since the storage module stops after the games and before any game
   * is destroyed.
   */
  concurrency::BoundedQueue<LeaderboardReply>* replies{nullptr};
};

}  // namespace rtype::server
