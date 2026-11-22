#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include "ConnectedPlayer.hpp"
#include "EnemyKilled.hpp"
#include "EventBus.hpp"
#include "PlayerRoster.hpp"
#include "ScoreKeeper.hpp"

namespace rtype::server::testing {

/** @brief A roster of two players, a bus and the keeper watching both. */
class ScoreKeeperFixture : public ::testing::Test {
 protected:
  ScoreKeeperFixture();

  void kill(std::uint8_t playerNumber, std::uint32_t points);
  /** @brief Publishes @p kill, a boss with its tally or its bounty, and
   * delivers it. */
  void publish(const game::EnemyKilled& kill);

  [[nodiscard]] std::uint32_t firstScore() const;
  [[nodiscard]] std::uint32_t secondScore() const;
  [[nodiscard]] std::uint32_t firstBounty() const;
  [[nodiscard]] std::uint32_t secondBounty() const;
  void giveFirstPlayerScore(std::uint32_t score);

 private:
  engine::EventBus events_;
  PlayerRoster roster_;
  ScoreKeeper keeper_{events_, roster_};
  ConnectedPlayer* first_{nullptr};
  ConnectedPlayer* second_{nullptr};
};

}  // namespace rtype::server::testing
