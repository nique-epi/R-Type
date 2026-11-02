#pragma once

#include <string_view>
#include "LeaderboardAnswer.hpp"
#include "LeaderboardRequest.hpp"
#include "ServerSessionFixture.hpp"

namespace rtype::client::testing {

/** @brief The leaderboard of a session wired as the connection module wires it:
 * the tests drive the session with datagrams and read the page. */
class ServerLeaderboardFixture : public ServerSessionFixture {
 protected:
  /** @returns The leaderboard the tests ask for. */
  [[nodiscard]] static network::LeaderboardRequest askedRequest();

  /** @returns An answer for a difficulty: KIRA first, and no own row. */
  [[nodiscard]] static network::LeaderboardAnswer answerFor(
      std::string_view difficulty);
};

}  // namespace rtype::client::testing
