#include "ScrollingBodies.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "Position.hpp"
#include "ScrollMotion.hpp"

namespace rtype::game {

void carryAlongTheScroll(engine::ComponentRegistry& components, float travel) {
  components.forEach<ScrollMotion, Position>(
      [travel](engine::Entity, const ScrollMotion&, Position& position) {
        position.x -= travel;
      });
}

}  // namespace rtype::game
