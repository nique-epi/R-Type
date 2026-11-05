#include "AnimationSystem.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "Playback.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @brief Replaces the ended @p animation by the one that follows it, or
 * destroys @p entity when it was played once and nothing follows.
 * @returns Whether @p entity is still there to show a frame. */
bool finish(engine::ComponentRegistry& components, engine::Entity entity,
            SpriteAnimation& animation) {
  if (animation.afterwards.has_value()) {
    animation.animation = *animation.afterwards;
    animation.afterwards.reset();
    animation.elapsed = engine::Duration::zero();
    return true;
  }
  if (animation.animation.playback == Playback::Once) {
    components.destroy(entity);
    return false;
  }
  return true;
}

}  // namespace

void AnimationSystem::update(engine::ComponentRegistry& components,
                             engine::Duration elapsed) {
  components.forEach<SpriteAnimation, game::Sprite>(
      [&components, elapsed](engine::Entity entity, SpriteAnimation& animation,
                             game::Sprite& sprite) {
        animation.elapsed += elapsed;
        if (animation.animation.hasEnded(animation.elapsed) &&
            !finish(components, entity, animation)) {
          return;
        }
        sprite.frame = animation.animation.frameAt(animation.elapsed)
                           .value_or(sprite.frame);
      });
}

}  // namespace rtype::client
