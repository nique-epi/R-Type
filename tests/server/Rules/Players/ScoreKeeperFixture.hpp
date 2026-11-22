#pragma once

#include <gtest/gtest.h>
#include <array>
#include <cstdint>
#include "ConnectedPlayer.hpp"
#include "EventBus.hpp"
#include "PilotCountConstants.hpp"
#include "PlayerRoster.hpp"
#include "ScoreKeeper.hpp"

namespace rtype::server::testing {

/** @brief A roster of two players, a bus and the keeper watching both. */
class ScoreKeeperFixture : public ::testing::Test {
 protected:
  ScoreKeeperFixture();

  void kill(std::uint8_t playerNumber, std::uint32_t points);
  /** @brief Publishes the kill of a boss worth @p points by @p playerNumber,
   * each pilot having dealt it the damage of @p damageByPilot. */
  void killBoss(
      std::uint8_t playerNumber, std::uint32_t points,
      const std::array<std::uint32_t, game::MOST_PILOTS>& damageByPilot);

  /** @brief Publishes the kill of a boss worth no point that pays @p bounty,
   * destroyed by @p playerNumber. */
  void killPaying(std::uint8_t playerNumber, std::uint32_t bounty);

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
