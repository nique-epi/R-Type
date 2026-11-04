#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "DeltaWorldState.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "EntityType.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "WorldStateSender.hpp"

namespace rtype::server::testing {

/**
 * @brief A world, a roster, a sender to its players, and the datagrams the
 * sender handed over, for the tests of the states written as differences.
 */
class DeltaStateSenderFixture : public ::testing::Test {
 public:
  DeltaStateSenderFixture();

 protected:
  /**
   * @brief Connects a player who sends from the client on @p port.
   * @returns The player, who stays in the roster.
   */
  ConnectedPlayer& connect(std::uint16_t port);

  /**
   * @brief Adds an Enemy under @p networkId at @p position.
   * @returns The entity, which a test moves or destroys.
   */
  engine::Entity addEnemy(std::uint32_t networkId, game::Position position);

  /**
   * @brief Puts the entity @p enemy at @p position.
   */
  void move(engine::Entity enemy, game::Position position);

  /**
   * @brief Destroys the entity @p enemy.
   */
  void destroy(engine::Entity enemy);

  /**
   * @brief Calls the sender's onTick @p count times, the tick counting up from
   * the last call.
   */
  void runTicks(std::size_t count);

  /**
   * @brief Gives @p player the acknowledgement of every datagram sent to it so
   * far, as a client that accepted them all would.
   */
  void acknowledgeEverythingSentTo(ConnectedPlayer& player);

  /**
   * @brief Gives @p player the acknowledgement of the newest datagram sent to
   * it and of no other, as a client would after losing the earlier ones.
   */
  void acknowledgeOnlyTheNewestSentTo(ConnectedPlayer& player);

  /**
   * @brief Runs MEASURED_SECONDS of a game for a player on FIRST_PLAYER_PORT
   * who acknowledges every state at once, in a scene of MEASURED_ENTITIES
   * enemies of which the first @p movingEntities move at every tick.
   * @returns The bytes of the datagrams sent to the player.
   */
  [[nodiscard]] std::size_t bytesOfAMeasuredRun(std::size_t movingEntities);

  /**
   * @returns The parts of states handed over to the player at @p port, oldest
   * first.
   */
  [[nodiscard]] std::vector<network::DeltaWorldState> partsSentTo(
      std::uint16_t port) const;

 private:
  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
  std::unique_ptr<WorldStateSender> sender_;
  std::uint32_t tick_{0};
};

}  // namespace rtype::server::testing
