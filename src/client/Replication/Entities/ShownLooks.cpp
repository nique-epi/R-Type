#include "ShownLooks.hpp"
#include <cstdint>
#include <optional>
#include "Animation.hpp"
#include "AppearanceReference.hpp"
#include "ComponentRegistry.hpp"
#include "Enraged.hpp"
#include "Entity.hpp"
#include "EntityAppearance.hpp"
#include "EntitySprites.hpp"
#include "ShownLook.hpp"
#include "SpriteAnimation.hpp"

namespace rtype::client {

namespace {

/** @returns The number of the look @p entity shows, 0 when it shows its
 * idle. */
std::uint8_t shownLookOf(engine::ComponentRegistry& components,
                         engine::Entity entity) {
  const auto* const shown = components.get<game::ShownLook>(entity);
  return shown == nullptr ? std::uint8_t{0} : shown->number;
}

/** @returns Whether @p playing plays @p resting: the same frames of the same
 * sheet, whatever time has passed in it. */
bool plays(const SpriteAnimation& playing, const Animation& resting) {
  return playing.animation.frames.data() == resting.frames.data() &&
         playing.animation.frames.size() == resting.frames.size();
}

}  // namespace

Animation restingAnimationOf(engine::ComponentRegistry& components,
                             engine::Entity entity,
                             const EntityAppearance& appearance) {
  const std::optional<Animation> look =
      lookAnimationOf(appearance, shownLookOf(components, entity));
  if (look.has_value()) {
    return *look;
  }
  return components.has<Enraged>(entity) ? enragedIdleAnimationOf(appearance)
                                         : idleAnimationOf(appearance);
}

void showLook(engine::ComponentRegistry& components, engine::Entity entity,
              std::uint8_t look) {
  if (shownLookOf(components, entity) == look) {
    return;
  }
  const auto* const reference = components.get<AppearanceReference>(entity);
  auto* const playing = components.get<SpriteAnimation>(entity);
  if (reference == nullptr || reference->appearance == nullptr ||
      playing == nullptr) {
    return;
  }
  const EntityAppearance& appearance = *reference->appearance;
  const Animation before = restingAnimationOf(components, entity, appearance);
  components.add(entity, game::ShownLook{.number = look});
  const Animation after = restingAnimationOf(components, entity, appearance);
  if (plays(*playing, before)) {
    *playing = SpriteAnimation{.animation = after};
  } else if (playing->afterwards.has_value()) {
    playing->afterwards = after;
  }
}

}  // namespace rtype::client
