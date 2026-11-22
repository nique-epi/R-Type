#include "ScoreKeeperFixture.hpp"
#include <array>
#include <cstdint>
#include <string>
#include "EnemyKilled.hpp"
#include "PilotCountConstants.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"

namespace rtype::server::testing {

ScoreKeeperFixture::ScoreKeeperFixture() {
  first_ = roster_.add(playerEndpoint(0), std::string{PLAYER_NICKNAME},
                       SESSION_TOKEN);
  second_ = roster_.add(playerEndpoint(1), std::string{PLAYER_NICKNAME},
                        SESSION_TOKEN);
}

void ScoreKeeperFixture::kill(std::uint8_t playerNumber, std::uint32_t points) {
  events_.publish(
      game::EnemyKilled{.playerNumber = playerNumber, .points = points});
  events_.dispatch();
}

void ScoreKeeperFixture::killBoss(
    std::uint8_t playerNumber, std::uint32_t points,
    const std::array<std::uint32_t, game::MOST_PILOTS>& damageByPilot) {
  events_.publish(game::EnemyKilled{.playerNumber = playerNumber,
                                    .points = points,
                                    .damageByPilot = damageByPilot});
  events_.dispatch();
}

std::uint32_t ScoreKeeperFixture::firstScore() const { return first_->score; }

std::uint32_t ScoreKeeperFixture::secondScore() const { return second_->score; }

void ScoreKeeperFixture::giveFirstPlayerScore(std::uint32_t score) {
  first_->score = score;
}

}  // namespace rtype::server::testing
