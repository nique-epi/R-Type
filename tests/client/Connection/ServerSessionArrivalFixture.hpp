#pragma once

#include <gtest/gtest.h>
#include <chrono>
#include <optional>
#include "ArrivalTally.hpp"
#include "ContentLoading.hpp"
#include "ContentPackCache.hpp"
#include "GameLoadedReminder.hpp"
#include "Heartbeat.hpp"
#include "MessageDispatcher.hpp"
#include "RecordingDatagramLink.hpp"
#include "RoundTripFigures.hpp"
#include "RoundTripWatch.hpp"
#include "ServerSession.hpp"
#include "ServerSessionParts.hpp"
#include "SessionEntityTypes.hpp"
#include "SessionEntityTypesFixtures.hpp"
#include "SimulatedClock.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief A session and its round trip watch over a recording link, which
 * hands the session datagrams carrying the time they arrived. */
class ServerSessionArrivalFixture : public ::testing::Test {
 protected:
  /** @brief Connects, then has the server accept the attempt under
   * SESSION_TOKEN. */
  void openSession();

  /** @brief Moves the clock by @p elapsed, then runs one frame of the
   * session. */
  void update(engine::Duration elapsed);

  /** @returns The newest Heartbeat the session sent. */
  [[nodiscard]] network::Heartbeat lastSentHeartbeat() const;

  /** @brief Hands the session a datagram of the session holding @p heartbeat,
   * stamped @p receivedAt, then runs one frame. */
  void deliverStamped(const network::Heartbeat& heartbeat,
                      std::chrono::nanoseconds receivedAt);

  /** @returns What the watch measured for the accepted player. */
  [[nodiscard]] std::optional<network::RoundTripFigures> ownRoundTrip() const;

 private:
  RecordingDatagramLink link_;
  network::MessageDispatcher dispatcher_;
  GameLoadedReminder gameLoaded_;
  SimulatedClock clock_;
  RoundTripWatch roundTrips_{dispatcher_, clock_};
  SessionEntityTypes entityTypes_{testLocalContent()};
  ContentLoading content_{ContentPackCache{unusedCacheFolder()}};
  ArrivalTally arrivals_;
  ServerSession session_{ServerSessionParts{.link = link_,
                                            .dispatcher = dispatcher_,
                                            .gameLoaded = gameLoaded_,
                                            .entityTypes = entityTypes_,
                                            .content = content_,
                                            .arrivals = arrivals_,
                                            .observers = {roundTrips_}}};
};

}  // namespace rtype::client::testing
