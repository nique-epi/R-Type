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

namespace rtype::server::testing {

/**
 * @brief The ports of the two clients the tests send to.
 */
constexpr std::uint16_t FIRST_CLIENT_PORT = 4000;
constexpr std::uint16_t SECOND_CLIENT_PORT = 4001;

/**
 * @brief The network id and the position of the ship of the tests that need one
 * entity.
 */
constexpr std::uint32_t SHIP_NETWORK_ID = 5;
constexpr game::Position SHIP_POSITION{.x = 1.0F, .y = 2.0F};

/**
 * @brief The network ids and the positions of the four entities, one of each
 * type, of the test that lists them.
 */
constexpr std::uint32_t PLAYER_NETWORK_ID = 1;
constexpr std::uint32_t ENEMY_NETWORK_ID = 2;
constexpr std::uint32_t PLAYER_MISSILE_NETWORK_ID = 3;
constexpr std::uint32_t ENEMY_MISSILE_NETWORK_ID = 4;
constexpr game::Position PLAYER_POSITION{.x = 10.0F, .y = 20.0F};
constexpr game::Position ENEMY_POSITION{.x = 11.0F, .y = 21.0F};
constexpr game::Position PLAYER_MISSILE_POSITION{.x = 12.0F, .y = 22.0F};
constexpr game::Position ENEMY_MISSILE_POSITION{.x = 13.0F, .y = 23.0F};

/**
 * @brief The network id of an entity that has no type.
 */
constexpr std::uint32_t UNTYPED_NETWORK_ID = 99;

/**
 * @brief More entities than one datagram holds.
 */
constexpr std::size_t MANY_ENTITIES = 300;

/**
 * @brief Bytes of the largest payload a datagram carries.
 */
constexpr std::size_t DATAGRAM_PAYLOAD_LIMIT = 1175;

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
class WorldStateSenderTest : public ::testing::Test {
 public:
  WorldStateSenderTest();

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
