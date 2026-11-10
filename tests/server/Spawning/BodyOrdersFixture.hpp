#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>
#include "BehaviourScripts.hpp"
#include "BodyOrders.hpp"
#include "BodyOrdersParts.hpp"
#include "EntityDestruction.hpp"
#include "EntitySpawned.hpp"
#include "EventBus.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "World.hpp"

namespace rtype::server::testing {

/**
 * @brief A world of the shipped sheets and the BodyOrders that carries
 * out what its scripts ask, called directly as the scripts would; the
 * datagrams it sends and the removals it publishes are recorded.
 */
class BodyOrdersFixture : public ::testing::Test {
 protected:
  BodyOrdersFixture();

  /** @brief Connects @p count players, who are then told about each body. */
  void connectPlayers(std::size_t count);
  /** @returns What `behaviours.spawn` returns for @p sheet at @p position. */
  [[nodiscard]] std::uint32_t spawnByScript(std::string_view sheet,
                                            game::Position position);
  /** @brief What `behaviours.destroy` does for @p networkId. */
  void removeByScript(std::uint32_t networkId);
  /** @brief Ends the tick: the asked bodies appear and go, and the bus
   * delivers what was published. */
  void endTick();
  /** @returns The network id of a body of @p sheet placed by the game itself
   * at @p position. */
  [[nodiscard]] std::uint32_t placeDirectly(std::string_view sheet,
                                            game::Position position);
  /** @returns Where the body of @p networkId is; nothing when none is. */
  [[nodiscard]] std::optional<game::Position> positionOf(
      std::uint32_t networkId);
  /** @returns The EntitySpawned sent to the player connected @p index-th. */
  [[nodiscard]] std::vector<network::EntitySpawned> spawnsSentTo(
      std::size_t index) const;
  /** @returns What builds a BodyOrders of this world attached to
   * @p scripts, nullptr for none, as a new game does. */
  [[nodiscard]] BodyOrdersParts partsFor(game::BehaviourScripts* scripts);
  /** @returns The removals published so far. */
  [[nodiscard]] const std::vector<game::EntityDestruction>& removals() const;

 private:
  game::World world_;
  PlayerRoster roster_;
  engine::EventBus events_;
  std::vector<network::OutgoingDatagram> sent_;
  std::vector<game::EntityDestruction> removals_;
  BodyOrders bodies_;
};

}  // namespace rtype::server::testing
