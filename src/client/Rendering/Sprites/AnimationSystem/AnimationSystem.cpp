#include "AnimationSystem.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

void AnimationSystem::update(engine::ComponentRegistry& components,
                             engine::Duration elapsed) {
  components.forEach<SpriteAnimation, game::Sprite>(
      [&components, elapsed](engine::Entity entity, SpriteAnimation& animation,
                             game::Sprite& sprite) {
        animation.elapsed += elapsed;
        if (animation.animation.hasEnded(animation.elapsed)) {
          if (!animation.afterwards.has_value()) {
            components.destroy(entity);
            return;
          }
          animation.animation = *animation.afterwards;
          animation.afterwards.reset();
          animation.elapsed = engine::Duration::zero();
        }
        sprite.frame = animation.animation.frameAt(animation.elapsed)
                           .value_or(sprite.frame);
      });
}

}  // namespace rtype::client
