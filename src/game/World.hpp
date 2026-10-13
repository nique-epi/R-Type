#pragma once

#include <cstddef>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"

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

  engine::Entity spawnPlayer();
  std::size_t entityCount() const;

  /**
   * @returns The components of the entities of this world.
   */
  engine::ComponentRegistry& components();

 private:
  engine::EntityRegistry registry_;
  engine::ComponentRegistry components_{registry_};
};

}  // namespace rtype::game
