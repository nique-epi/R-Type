#pragma once

#include <cstddef>
#include "Entity.hpp"
#include "EntityRegistry.hpp"

namespace rtype::game {

/**
 * @brief Game state shared by client and server. It knows neither sockets nor
 * the network protocol.
 */
class World {
 public:
  engine::Entity spawnPlayer();
  std::size_t entityCount() const;

 private:
  engine::EntityRegistry registry_;
};

}  // namespace rtype::game
