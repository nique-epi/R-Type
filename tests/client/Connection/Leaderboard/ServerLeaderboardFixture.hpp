#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>
#include "GameLoadedReminder.hpp"
#include "LeaderboardAnswer.hpp"
#include "LeaderboardRequest.hpp"
#include "MessageDispatcher.hpp"
#include "OutgoingDatagram.hpp"
#include "RecordingDatagramLink.hpp"
#include "ServerLeaderboard.hpp"
#include "ServerSession.hpp"
#include "ServerSessionParts.hpp"
#include "SessionEntityTypes.hpp"
#include "SessionEntityTypesFixtures.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief A session and the leaderboard that follows it, wired as the
 * connection module wires them, over a recording link: the tests drive the
 * session with datagrams and read the page. */
class ServerLeaderboardFixture : public ::testing::Test {
 protected:
  /** @returns The leaderboard the tests ask for. */
  [[nodiscard]] static network::LeaderboardRequest askedRequest();

  /** @returns An answer for a difficulty: KIRA first, and no own row. */
  [[nodiscard]] static network::LeaderboardAnswer answerFor(
      std::string_view difficulty);

  /** @brief Connects, then has the server accept the attempt, its acceptance
   * carrying SESSION_TOKEN. */
  void connectAndGetAccepted();

  /** @brief Runs one frame of the network stage. */
  void update(engine::Duration elapsed = engine::Duration::zero());

  /** @brief Delivers @p answer in a datagram of the server carrying this
   * sequence and SESSION_TOKEN. */
  void deliverInSession(const network::LeaderboardAnswer& answer,
                        std::uint16_t sequence);

  [[nodiscard]] ServerLeaderboard& leaderboard();

  [[nodiscard]] ServerSession& session();

  [[nodiscard]] const std::vector<network::OutgoingDatagram>& sent() const;

  /** @returns How many datagrams sent so far hold a LeaderboardRequest. */
  [[nodiscard]] std::size_t sentRequestCount() const;

 private:
  RecordingDatagramLink link_;
  network::MessageDispatcher dispatcher_;
  GameLoadedReminder gameLoaded_;
  ServerLeaderboard leaderboard_{dispatcher_};
  SessionEntityTypes entityTypes_{isDrawnInTheTests};
  ServerSession session_{
      ServerSessionParts{.link = link_,
                         .dispatcher = dispatcher_,
                         .gameLoaded = gameLoaded_,
                         .entityTypes = entityTypes_,
                         .observers = {gameLoaded_, leaderboard_}}};
};

}  // namespace rtype::client::testing
