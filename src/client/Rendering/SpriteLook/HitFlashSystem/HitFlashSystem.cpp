#include "HitFlashSystem.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "HitFlash.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

void HitFlashSystem::update(engine::ComponentRegistry& components,
                            engine::Duration elapsed) {
  spentFlashes_.clear();
  components.forEach<HitFlash, HitFlash>(
      [this, elapsed](engine::Entity entity, HitFlash& flash, HitFlash&) {
        flash.remaining -= elapsed;
        if (flash.remaining <= engine::Duration::zero()) {
          spentFlashes_.push_back(entity);
        }
      });
  for (const engine::Entity entity : spentFlashes_) {
    components.remove<HitFlash>(entity);
  }
}

}  // namespace rtype::client
