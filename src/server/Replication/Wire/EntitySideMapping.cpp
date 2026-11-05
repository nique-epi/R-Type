#include "EntitySideMapping.hpp"
#include "EntitySide.hpp"
#include "EntityType.hpp"

namespace rtype::server {

network::EntitySide toEntitySide(game::EntityType type) {
  switch (type) {
    case game::EntityType::Player:
      return network::EntitySide::Player;
    case game::EntityType::Enemy:
      return network::EntitySide::Enemy;
    case game::EntityType::PlayerMissile:
      return network::EntitySide::PlayerMissile;
    case game::EntityType::EnemyMissile:
      return network::EntitySide::EnemyMissile;
  }
  return network::EntitySide::Player;
}

}  // namespace rtype::server
