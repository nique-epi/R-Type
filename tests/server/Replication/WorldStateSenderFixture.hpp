#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "Endpoint.hpp"
#include "EntityRegistry.hpp"
#include "EntityType.hpp"
#include "Invulnerable.hpp"
#include "Lives.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "WorldState.hpp"
#include "WorldStateSender.hpp"

namespace rtype::server::testing {

/**
 * @returns The loopback address of a client listening on @p port.
 */
network::Endpoint client(std::uint16_t port);

/**
 * @returns The entity the sender must list for the given parts.
 */
network::EntityState expectedEntity(std::uint32_t networkId,
                                    network::WireEntityType type,
                                    game::Position position);

/**
 * @brief A world, a roster, a sender to the players of that roster, and the
 * datagrams the sender handed to its send function, in order.
 */
class WorldStateSenderFixture : public ::testing::Test {
 public:
  WorldStateSenderFixture();

 protected:
  /**
   * @brief Connects a player who sends from the client on @p port, under
   * @p sessionToken.
   * @returns The player, who stays in the roster.
   */
  ConnectedPlayer& connect(std::uint16_t port, std::uint64_t sessionToken);

  /**
   * @brief Adds an entity with a NetworkIdentity, a Position and an EntityType.
   */
  void addNetworkedEntity(game::EntityType type, game::Position position,
                          std::uint32_t networkId);

  /**
   * @brief Adds the ship of a player: an entity under @p networkId with a
   * ShipMotion, an Intent, @p lives and @p protection.
   */
  void addShip(std::uint32_t networkId, game::Lives lives,
               game::Invulnerable protection);

  /**
   * @brief Adds an entity with a Position and an EntityType, and no
   * NetworkIdentity.
   */
  void addEntityWithoutNetworkId(game::EntityType type,
                                 game::Position position);

  /**
   * @brief Adds an entity with a NetworkIdentity and a Position, and no
   * EntityType.
   */
  void addEntityWithoutType(game::Position position, std::uint32_t networkId);

  /**
   * @brief Calls the sender's onTick @p count times, the tick counting up from
   * the last call.
   */
  void runTicks(std::size_t count);

  /**
   * @returns The WorldState the datagram handed over at @p sentIndex holds
   * after its header.
   */
  [[nodiscard]] network::WorldState decodedState(std::size_t sentIndex) const;

  /**
   * @returns The header of the datagram handed over at @p sentIndex.
   */
  [[nodiscard]] network::PacketHeader headerOf(std::size_t sentIndex) const;

  /**
   * @returns The datagrams the sender handed to its send function, in order.
   */
  [[nodiscard]] const std::vector<network::OutgoingDatagram>& sentDatagrams()
      const;

 private:
  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
  WorldStateSender sender_;
  std::uint32_t tick_{0};
};

}  // namespace rtype::server::testing
