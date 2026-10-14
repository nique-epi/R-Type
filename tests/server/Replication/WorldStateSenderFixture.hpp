#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "ComponentRegistry.hpp"
#include "Endpoint.hpp"
#include "EntityRegistry.hpp"
#include "EntityType.hpp"
#include "OutgoingDatagram.hpp"
#include "Position.hpp"
#include "WorldState.hpp"
#include "WorldStateSender.hpp"
#include "WorldStateSenderTestConstants.hpp"

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
 * @brief A world, a sender, and the datagrams the sender handed to its send
 * function, in order.
 */
class WorldStateSenderFixture : public ::testing::Test {
 public:
  WorldStateSenderFixture();

 protected:
  /**
   * @brief Adds an entity with a NetworkIdentity, a Position and an EntityType.
   */
  void addNetworkedEntity(game::EntityType type, game::Position position,
                          std::uint32_t networkId);

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
   * @brief Calls the sender's onTick @p count times with @p recipients.
   */
  void runTicks(std::size_t count,
                const std::vector<network::Endpoint>& recipients);

  /**
   * @returns The datagram handed over at @p sentIndex, decoded.
   */
  [[nodiscard]] network::WorldState decodedState(std::size_t sentIndex) const;

  /**
   * @returns The datagrams the sender handed to its send function, in order.
   */
  [[nodiscard]] const std::vector<network::OutgoingDatagram>& sentDatagrams()
      const;

 private:
  engine::EntityRegistry entities_;
  engine::ComponentRegistry components_{entities_};
  std::vector<network::OutgoingDatagram> sent_;
  WorldStateSender sender_;
};

}  // namespace rtype::server::testing
