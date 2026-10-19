#pragma once

#include <cstddef>
#include <cstdint>
#include "ComponentRegistry.hpp"
#include "EntityRegistry.hpp"
#include "NetworkIdAllocator.hpp"
#include "Position.hpp"

namespace rtype::game {

/**
 * @brief Game state shared by client and server. It knows neither sockets nor
 * the network protocol.
 */
class World {
 public:
  World() = default;
  World(const World&) = delete;
  World& operator=(const World&) = delete;
  World(World&&) = delete;
  World& operator=(World&&) = delete;
  ~World() = default;

  /**
   * @brief Creates the ship of a player at @p position, with the Position, the
   * EntityType::Player and the NetworkIdentity that make it a networked
   * entity.
   * @returns The network id the ship was given, never NO_ENTITY_ID.
   * @throws NetworkIdExhaustedException when every network id was given.
   */
  std::uint32_t spawnPlayer(Position position);
  [[nodiscard]] std::size_t entityCount() const;

  /**
   * @returns The components of the entities of this world.
   */
  engine::ComponentRegistry& components();

 private:
  engine::EntityRegistry registry_;
  engine::ComponentRegistry components_{registry_};
  engine::NetworkIdAllocator networkIds_;
};

}  // namespace rtype::game
