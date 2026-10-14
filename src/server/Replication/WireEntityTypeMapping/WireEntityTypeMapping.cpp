#include "WireEntityTypeMapping.hpp"

namespace rtype::server {

network::WireEntityType toWireEntityType(game::EntityType type) {
  switch (type) {
    case game::EntityType::Player:
      return network::WireEntityType::PlayerShip;
    case game::EntityType::Enemy:
      return network::WireEntityType::Bydo;
    case game::EntityType::PlayerMissile:
      return network::WireEntityType::PlayerMissile;
    case game::EntityType::EnemyMissile:
      return network::WireEntityType::EnemyMissile;
  }
  return network::WireEntityType::PlayerShip;
}

}  // namespace rtype::server
