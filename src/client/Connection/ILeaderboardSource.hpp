#pragma once

#include "LeaderboardPage.hpp"
#include "LeaderboardRequest.hpp"

namespace rtype::client {

/**
 * @brief Where a leaderboard screen reads its rows: a server, or later the
 * master. A source never makes the frame wait: ask() returns at once, and the
 * page changes on a later frame.
 */
class ILeaderboardSource {
 public:
  virtual ~ILeaderboardSource() = default;

  /** @brief Starts reading a leaderboard, in place of any read under way. */
  virtual void ask(const network::LeaderboardRequest& request) = 0;

  /** @brief Stops the read under way. It never throws: a screen calls it from
   * its destructor. */
  virtual void stopAsking() noexcept = 0;

  /** @returns How the read stands. Valid until the next call to ask(). */
  [[nodiscard]] virtual const LeaderboardPage& page() const = 0;
};

}  // namespace rtype::client
