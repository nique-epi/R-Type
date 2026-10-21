#include <gtest/gtest.h>
#include <cstdint>
#include <limits>
#include <string>
#include "ConnectedPlayer.hpp"
#include "Endpoint.hpp"
#include "EnemyKilled.hpp"
#include "EventBus.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "ScoreKeeper.hpp"
#include "ServerLobbyTestConstants.hpp"

namespace rtype::server {
namespace {

using testing::PLAYER_ADDRESS;
using testing::PLAYER_NICKNAME;
using testing::SESSION_TOKEN;

constexpr std::uint16_t FIRST_PORT = 4000;
constexpr std::uint16_t SECOND_PORT = 4001;
constexpr std::uint8_t FIRST_NUMBER = network::FIRST_PLAYER_NUMBER;
constexpr std::uint8_t SECOND_NUMBER = network::FIRST_PLAYER_NUMBER + 1;
constexpr std::uint32_t DRIFTER_WORTH = 100;
constexpr std::uint32_t WEAVER_WORTH = 200;

/** @brief A roster of two players, a bus and the keeper watching both. */
class ScoreKeeperTest : public ::testing::Test {
 protected:
  ScoreKeeperTest() {
    first_ = roster_.add(endpoint(FIRST_PORT), std::string{PLAYER_NICKNAME},
                         SESSION_TOKEN);
    second_ = roster_.add(endpoint(SECOND_PORT), std::string{PLAYER_NICKNAME},
                          SESSION_TOKEN);
  }

  static network::Endpoint endpoint(std::uint16_t port) {
    return network::Endpoint{.address = std::string{PLAYER_ADDRESS},
                             .port = port};
  }

  void kill(std::uint8_t playerNumber, std::uint32_t points) {
    events_.publish(
        game::EnemyKilled{.playerNumber = playerNumber, .points = points});
    events_.dispatch();
  }

  [[nodiscard]] std::uint32_t firstScore() const { return first_->score; }
  [[nodiscard]] std::uint32_t secondScore() const { return second_->score; }
  void giveFirstPlayerScore(std::uint32_t score) { first_->score = score; }

 private:
  engine::EventBus events_;
  PlayerRoster roster_;
  ScoreKeeper keeper_{events_, roster_};
  ConnectedPlayer* first_{nullptr};
  ConnectedPlayer* second_{nullptr};
};

/**
 * Given two connected players with no score
 * When the second player kills an enemy worth points
 * Then the second player has those points and the first still has none
 */
TEST_F(ScoreKeeperTest, CreditsOnlyThePlayerWhoFired) {
  kill(SECOND_NUMBER, DRIFTER_WORTH);

  EXPECT_EQ(secondScore(), DRIFTER_WORTH);
  EXPECT_EQ(firstScore(), 0U);
}

/**
 * Given a player who already killed a Drifter
 * When the player kills a Weaver
 * Then the score is the sum of both
 */
TEST_F(ScoreKeeperTest, AddsEachKillToTheScore) {
  kill(FIRST_NUMBER, DRIFTER_WORTH);
  kill(FIRST_NUMBER, WEAVER_WORTH);

  EXPECT_EQ(firstScore(), DRIFTER_WORTH + WEAVER_WORTH);
}

/**
 * Given two connected players
 * When a kill names a number nobody holds, 0, or a number above the maximum
 * Then no score changes
 */
TEST_F(ScoreKeeperTest, IgnoresAKillOfANumberNobodyHolds) {
  kill(network::FIRST_PLAYER_NUMBER + 2, DRIFTER_WORTH);
  kill(network::NO_PLAYER_NUMBER, DRIFTER_WORTH);
  kill(network::MAXIMUM_PLAYERS + 1, DRIFTER_WORTH);

  EXPECT_EQ(firstScore(), 0U);
  EXPECT_EQ(secondScore(), 0U);
}

/**
 * Given a player whose score is 10 below the largest value
 * When the player kills an enemy worth more than 10
 * Then the score stops at the largest value instead of wrapping around
 */
TEST_F(ScoreKeeperTest, StopsTheScoreAtItsLargestValue) {
  constexpr std::uint32_t LARGEST = std::numeric_limits<std::uint32_t>::max();
  constexpr std::uint32_t TEN = 10;
  giveFirstPlayerScore(LARGEST - TEN);

  kill(FIRST_NUMBER, DRIFTER_WORTH);

  EXPECT_EQ(firstScore(), LARGEST);
}

/**
 * Given a keeper that was destroyed
 * When a kill is published
 * Then nothing reads it
 */
TEST(ScoreKeeper, StopsListeningOnceDestroyed) {
  engine::EventBus events;
  PlayerRoster roster;
  ConnectedPlayer* const player = roster.add(
      network::Endpoint{.address = std::string{PLAYER_ADDRESS}, .port = 4000},
      std::string{PLAYER_NICKNAME}, SESSION_TOKEN);
  {
    const ScoreKeeper keeper{events, roster};
  }

  events.publish(game::EnemyKilled{.playerNumber = network::FIRST_PLAYER_NUMBER,
                                   .points = DRIFTER_WORTH});
  events.dispatch();

  EXPECT_EQ(player->score, 0U);
}

}  // namespace
}  // namespace rtype::server
