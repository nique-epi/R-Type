#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "EntitySheets.hpp"
#include "EventBus.hpp"
#include "GameContext.hpp"
#include "GameEnded.hpp"
#include "GameFlow.hpp"
#include "GameReplication.hpp"
#include "GameSettings.hpp"
#include "GameStateChanged.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "ReadyKeeper.hpp"
#include "SessionHooks.hpp"
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

  /** @brief A game flow whose Bydos come in @p waves. */
  explicit GameFlowFixture(WavePlan waves);
  /** @brief A game flow whose Bydos come in @p waves, of the sheets @p types,
   * which must outlive the fixture. */
  GameFlowFixture(WavePlan waves, const game::EntitySheets& types);

  /** @brief A game flow set up with @p settings. */
  explicit GameFlowFixture(GameSettings settings);

 protected:
  /** @brief Connects the player at @p index and gives it a ship. */
  ConnectedPlayer& join(std::size_t index);

  /** @brief Connects the players at @p indexes, who are all ready, and runs the
   * tick that starts the game. */
  void startGameWith(std::initializer_list<std::size_t> indexes);

  /** @brief Takes every life of @p player, the ship in play included. */
  void takeEveryLifeOf(ConnectedPlayer& player);

  /** @brief Runs one tick, then delivers the events it published. */
  void tick();

  void runTicks(std::size_t count);

  /** @brief Destroys every Bydo of the world, as if they were all killed. */
  void clearTheBydos();

  /** @returns The GameEnded sent to the player at @p index, oldest first. */
  [[nodiscard]] std::vector<network::GameEnded> endsSentTo(
      std::size_t index) const;

  [[nodiscard]] GameFlow& flow();
  [[nodiscard]] PlayerRoster& roster();

  /** @returns Every GameStateChanged the flow published, in order. */
  [[nodiscard]] const std::vector<GameStateChanged>& changes() const;

 private:
  engine::EventBus events_;
  engine::SystemScheduler systems_;
  SessionHooks hooks_;
  WorldSlot world_;
  PlayerRoster roster_;
  ReadyKeeper readyKeeper_;
  std::vector<network::OutgoingDatagram> sent_;
  GameContext context_;
  GameReplication replication_;
  GameFlow flow_;
  std::vector<GameStateChanged> changes_;
};

/**
 * @brief A game flow whose level holds one wave: the game is won once the
 * first wave is over.
 */
class GameFlowOneWaveLevelFixture : public GameFlowFixture {
 public:
  GameFlowOneWaveLevelFixture();
};

/**
 * @brief A game flow whose rules turn friendly fire on, and a way to put a
 * missile of one player on the ship of another.
 */
class GameFlowFriendlyFireFixture : public GameFlowFixture {
 public:
  GameFlowFriendlyFireFixture();

 protected:
  /** @brief Puts a missile of the player numbered @p shooter on the ship of
   * @p target, then runs the tick that finds them touching. */
  void fireAt(const ConnectedPlayer& target, std::uint8_t shooter);
};

}  // namespace rtype::server::testing
