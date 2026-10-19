#include "RenderSystem.hpp"
#include <algorithm>
#include <cstddef>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "HitFlash.hpp"
#include "IDrawSurface.hpp"
#include "Position.hpp"
#include "Sprite.hpp"
#include "SpriteTone.hpp"

namespace rtype::client {

void RenderSystem::render(engine::ComponentRegistry& components,
                          IDrawSurface& surface) {
  for (auto& drawCalls : drawCallsByLayer_) {
    drawCalls.clear();
  }
  components.forEach<game::Position, game::Sprite>(
      [this, &components](engine::Entity entity, game::Position& position,
                          game::Sprite& sprite) {
        const auto layerIndex = static_cast<std::size_t>(sprite.layer);
        if (layerIndex >= drawCallsByLayer_.size()) {
          return;
        }
        const HitFlash* flash = components.get<HitFlash>(entity);
        drawCallsByLayer_[layerIndex].push_back(
            {.entityIndex = entity.index,
             .position = position,
             .sprite = &sprite,
             .tone = flash != nullptr ? flash->tone : SpriteTone::Normal});
      });
  for (auto& drawCalls : drawCallsByLayer_) {
    std::ranges::sort(drawCalls, {}, &DrawCall::entityIndex);
    for (const DrawCall& drawCall : drawCalls) {
      surface.draw(drawCall.sprite->assetId, drawCall.sprite->frame,
                   drawCall.position, drawCall.tone);
    }
  }
}

}  // namespace rtype::client
