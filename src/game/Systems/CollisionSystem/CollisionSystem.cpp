#include "CollisionSystem.hpp"
#include <cstddef>
#include "Bounds.hpp"
#include "Collision.hpp"
#include "CollisionBox.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EventBus.hpp"
#include "Overlaps.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

CollisionSystem::CollisionSystem(engine::EventBus& events) : events_(events) {}

void CollisionSystem::update(engine::ComponentRegistry& components,
                             [[maybe_unused]] engine::Duration elapsed) {
  candidates_.clear();
  components.forEach<Position, CollisionBox>([this](engine::Entity entity,
                                                    const Position& position,
                                                    const CollisionBox& box) {
    candidates_.push_back(
        Candidate{.entity = entity,
                  .bounds = engine::Bounds{.centerX = position.x,
                                           .centerY = position.y,
                                           .width = box.width,
                                           .height = box.height}});
  });
  for (std::size_t first = 0; first < candidates_.size(); ++first) {
    for (std::size_t second = first + 1; second < candidates_.size();
         ++second) {
      if (engine::overlaps(candidates_[first].bounds,
                           candidates_[second].bounds)) {
        events_.publish(
            engine::Collision{.first = candidates_[first].entity,
                              .second = candidates_[second].entity});
      }
    }
  }
}

}  // namespace rtype::game
