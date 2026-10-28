#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "Departures.hpp"
#include "Endpoint.hpp"
#include "EventBus.hpp"
#include "PlayerDeparture.hpp"
#include "PlayerRoster.hpp"
#include "SentDatagrams.hpp"
#include "SessionConstants.hpp"
#include "SessionsFixtures.hpp"
#include "SubscriptionHandle.hpp"

namespace rtype::server {
namespace {

using testing::addPlayer;
using testing::playerEndpoint;

/** @brief A roster with one player, and the departures announced for it. */
struct DeparturesScene {
  DeparturesScene()
      : player_(&addPlayer(roster_)),
        departures_(roster_, events_,
                    [this](const ConnectedPlayer& closing) {
                      closingPlayers_.push_back(closing);
                      seatFreedWhenClosing_ =
                          roster_.find(closing.endpoint) == nullptr;
                    }),
        subscription_(events_.subscribe<game::PlayerDeparture>(
            [this](const game::PlayerDeparture& departure) {
              announced_.push_back(departure);
            })) {}

  DeparturesScene(const DeparturesScene&) = delete;
  DeparturesScene& operator=(const DeparturesScene&) = delete;
  DeparturesScene(DeparturesScene&&) = delete;
  DeparturesScene& operator=(DeparturesScene&&) = delete;
  ~DeparturesScene() { events_.unsubscribe(subscription_); }

  /** @returns The departures announced once the bus has delivered them. */
  const std::vector<game::PlayerDeparture>& announced() {
    events_.dispatch();
    return announced_;
  }

  PlayerRoster roster_;
  engine::EventBus events_;
  ConnectedPlayer* player_;
  std::vector<ConnectedPlayer> closingPlayers_;
  bool seatFreedWhenClosing_{false};
  Departures departures_;

  std::vector<game::PlayerDeparture> announced_;
  engine::SubscriptionHandle subscription_;
};

/**
 * Given a game with one player
 * When its session closes because it left
 * Then its seat is free, the closing function saw it already free, a
 * departure (Left) is announced and the player is kept for whoever forgets it
 */
TEST(Departures, ClosesTheSessionOfAPlayerWhoLeft) {
  DeparturesScene scene;
  const network::Endpoint endpoint = scene.player_->endpoint;
  const std::uint8_t number = scene.player_->playerNumber;

  scene.departures_.close(endpoint, game::DepartureCause::Left);

  EXPECT_EQ(scene.roster_.find(endpoint), nullptr);
  EXPECT_TRUE(scene.seatFreedWhenClosing_);
  ASSERT_EQ(scene.closingPlayers_.size(), 1U);
  EXPECT_EQ(scene.closingPlayers_.front().playerNumber, number);
  ASSERT_EQ(scene.announced().size(), 1U);
  EXPECT_EQ(scene.announced().front().playerNumber, number);
  EXPECT_EQ(scene.announced().front().cause, game::DepartureCause::Left);
  EXPECT_EQ(scene.departures_.takeDeparted().size(), 1U);
}

/**
 * Given a game with one player
 * When a session closes for an address no player sends from
 * Then nothing is called, announced or kept
 */
TEST(Departures, ClosesNothingForAnUnknownAddress) {
  DeparturesScene scene;

  scene.departures_.close(playerEndpoint(1), game::DepartureCause::Left);

  EXPECT_TRUE(scene.closingPlayers_.empty());
  EXPECT_TRUE(scene.announced().empty());
  EXPECT_TRUE(scene.departures_.takeDeparted().empty());
  EXPECT_NE(scene.roster_.find(scene.player_->endpoint), nullptr);
}

/**
 * Given a player closed once
 * When the departed players are taken twice
 * Then the second call returns none
 */
TEST(Departures, HandsEachDepartedPlayerOverOnce) {
  DeparturesScene scene;
  scene.departures_.close(scene.player_->endpoint, game::DepartureCause::Left);
  static_cast<void>(scene.departures_.takeDeparted());

  EXPECT_TRUE(scene.departures_.takeDeparted().empty());
}

/**
 * Given a player not heard from for SESSION_TIMEOUT_TICKS ticks
 * When one more tick of silence is counted
 * Then its session closes with a departure (TimedOut), and not a tick before
 */
TEST(Departures, ClosesASilentSessionOnceTheTimeoutIsReached) {
  DeparturesScene scene;
  for (std::size_t tick = 0; tick < SESSION_TIMEOUT_TICKS; ++tick) {
    scene.departures_.closeSilentSessions();
  }
  ASSERT_NE(scene.roster_.find(playerEndpoint(0)), nullptr);

  scene.departures_.closeSilentSessions();

  EXPECT_EQ(scene.roster_.find(playerEndpoint(0)), nullptr);
  ASSERT_EQ(scene.announced().size(), 1U);
  EXPECT_EQ(scene.announced().front().cause, game::DepartureCause::TimedOut);
}

/**
 * Given a player heard from at the last tick before its timeout
 * When silence is counted again
 * Then its session stays open
 */
TEST(Departures, KeepsTheSessionOfAPlayerHeardFrom) {
  DeparturesScene scene;
  for (std::size_t tick = 0; tick < SESSION_TIMEOUT_TICKS; ++tick) {
    scene.departures_.closeSilentSessions();
  }
  scene.player_->ticksSinceReceived = 0;

  scene.departures_.closeSilentSessions();

  EXPECT_NE(scene.roster_.find(playerEndpoint(0)), nullptr);
  EXPECT_TRUE(scene.announced().empty());
}

}  // namespace
}  // namespace rtype::server
