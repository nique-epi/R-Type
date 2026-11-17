#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <vector>
#include "DebugJournal.hpp"
#include "DeparturesFixture.hpp"
#include "Endpoint.hpp"
#include "LogFormat.hpp"
#include "PlayerDeparture.hpp"
#include "ProtocolConstants.hpp"
#include "SentDatagrams.hpp"
#include "SessionConstants.hpp"
#include "SessionsTestConstants.hpp"

namespace rtype::server {

using testing::AWAY_EVENT_NAME;
using testing::DeparturesFixture;
using testing::EXPELLED_EVENT_NAME;
using testing::LEFT_EVENT_NAME;
using testing::playerEndpoint;
using testing::TIMED_OUT_EVENT_NAME;

/**
 * Given a journal written as JSON Lines and a game with one player
 * When the player leaves
 * Then one player.left event names its number, its address and its port
 */
TEST_F(DeparturesFixture, WritesAPlayerWhoLeftAsAnEvent) {
  const DebugJournal journal{rtype::logging::LogFormat::Json};
  const std::uint8_t number = player().playerNumber;

  departures().close(player().endpoint, game::DepartureCause::Left);

  const std::vector<nlohmann::json> events =
      journal.file().events(LEFT_EVENT_NAME);
  ASSERT_EQ(events.size(), 1U);
  EXPECT_EQ(events.front().at("player").get<std::int64_t>(), number);
  EXPECT_EQ(events.front().at("address").get<std::string>(),
            playerEndpoint(0).address);
  EXPECT_EQ(events.front().at("port").get<std::int64_t>(),
            playerEndpoint(0).port);
}

/**
 * Given a journal written as JSON Lines and a game with one player
 * When the host expels the player
 * Then one player.expelled event names it, and no player.left event
 */
TEST_F(DeparturesFixture, WritesAnExpelledPlayerAsAnEvent) {
  const DebugJournal journal{rtype::logging::LogFormat::Json};
  const std::uint8_t number = player().playerNumber;

  departures().close(player().endpoint, game::DepartureCause::Expelled);

  const std::vector<nlohmann::json> events =
      journal.file().events(EXPELLED_EVENT_NAME);
  ASSERT_EQ(events.size(), 1U);
  EXPECT_EQ(events.front().at("player").get<std::int64_t>(), number);
  EXPECT_EQ(events.front().at("address").get<std::string>(),
            playerEndpoint(0).address);
  EXPECT_EQ(events.front().at("port").get<std::int64_t>(),
            playerEndpoint(0).port);
  EXPECT_TRUE(journal.file().events(LEFT_EVENT_NAME).empty());
}

/**
 * Given a journal written as JSON Lines and a player silent past the timeout
 * When one more tick of silence makes it away
 * Then one player.away event names it, how long it was silent and how long
 * its seat is kept, in seconds
 */
TEST_F(DeparturesFixture, WritesAPlayerWhoGoesAwayAsAnEvent) {
  const DebugJournal journal{rtype::logging::LogFormat::Json};
  const std::uint8_t number = player().playerNumber;

  for (std::size_t tick = 0; tick < SESSION_TIMEOUT_TICKS; ++tick) {
    departures().closeSilentSessions();
  }

  const std::vector<nlohmann::json> events =
      journal.file().events(AWAY_EVENT_NAME);
  ASSERT_EQ(events.size(), 1U);
  EXPECT_EQ(events.front().at("player").get<std::int64_t>(), number);
  EXPECT_EQ(events.front().at("address").get<std::string>(),
            playerEndpoint(0).address);
  EXPECT_EQ(events.front().at("port").get<std::int64_t>(),
            playerEndpoint(0).port);
  EXPECT_EQ(events.front().at("silent_seconds").get<std::int64_t>(),
            network::SESSION_TIMEOUT.count());
  EXPECT_EQ(events.front().at("seat_kept_seconds").get<std::int64_t>(),
            network::RECONNECTION_GRACE.count());
}

/**
 * Given a journal written as JSON Lines and a player silent until its session
 * closes
 * When the session closes
 * Then one player.timed_out event names it and how long it was silent
 */
TEST_F(DeparturesFixture, WritesAPlayerWhoTimedOutAsAnEvent) {
  const DebugJournal journal{rtype::logging::LogFormat::Json};
  const std::uint8_t number = player().playerNumber;

  for (std::size_t tick = 0; tick <= SESSION_CLOSING_TICKS; ++tick) {
    departures().closeSilentSessions();
  }

  const std::vector<nlohmann::json> events =
      journal.file().events(TIMED_OUT_EVENT_NAME);
  ASSERT_EQ(events.size(), 1U);
  EXPECT_EQ(events.front().at("player").get<std::int64_t>(), number);
  EXPECT_EQ(events.front().at("address").get<std::string>(),
            playerEndpoint(0).address);
  EXPECT_EQ(events.front().at("port").get<std::int64_t>(),
            playerEndpoint(0).port);
  EXPECT_EQ(events.front().at("silent_seconds").get<std::int64_t>(),
            network::SESSION_SILENCE_LIMIT.count());
}

}  // namespace rtype::server
