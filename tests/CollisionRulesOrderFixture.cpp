#include "CollisionRulesOrderFixture.hpp"
#include "Collision.hpp"
#include "Entity.hpp"

rtype::engine::Collision CollisionRulesOrderFixture::collisionOf(
    rtype::engine::Entity entity, rtype::engine::Entity other) {
  return GetParam()
             ? rtype::engine::Collision{.first = other, .second = entity}
             : rtype::engine::Collision{.first = entity, .second = other};
}
