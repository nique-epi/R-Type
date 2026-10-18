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
          components.destroy(entity);
          return;
        }
        sprite.frame = animation.animation.frameAt(animation.elapsed)
                           .value_or(sprite.frame);
      });
}

}  // namespace rtype::client
