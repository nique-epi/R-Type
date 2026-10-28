#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <initializer_list>
#include "ConnectedPlayer.hpp"
#include "EventBus.hpp"
#include "GameFlow.hpp"
#include "GameReplication.hpp"
#include "PlayerRoster.hpp"
#include "ReadyKeeper.hpp"
#include "SystemScheduler.hpp"
#include "WavePlan.hpp"
#include "WorldSlot.hpp"

namespace rtype::server::testing {

/**
 * @brief A game flow over a roster the test fills itself: no datagram is read
 * and no session closes, so what a test sees is the state machine alone. A
 * player is ready when its ready flag is set, as ReadyKeeper reads it, and each
 * tick delivers the events it published, as the server does.
 */
class GameFlowFixture : public ::testing::Test {
 public:
  GameFlowFixture();

 protected:
  /** @brief Connects the player at @p index and gives it a ship. */
  ConnectedPlayer& join(std::size_t index);

  /** @brief Connects the players at @p indexes, who are all ready, and runs the
   * tick that starts the game. */
  void startGameWith(std::initializer_list<std::size_t> indexes);

  /** @brief Connects one player, who is ready, and runs the tick that starts
   * the game. */
  void startGameWithOnePlayer();

  /** @brief Runs one tick, then delivers the events it published. */
  void tick();

  void runTicks(std::size_t count);

  [[nodiscard]] GameFlow& flow();
  [[nodiscard]] PlayerRoster& roster();

 private:
  engine::EventBus events_;
  engine::SystemScheduler systems_;
  WorldSlot world_;
  PlayerRoster roster_;
  ReadyKeeper readyKeeper_;
  GameReplication replication_;
  WavePlan wavePlan_;
  GameFlow flow_;
};

}  // namespace rtype::server::testing
