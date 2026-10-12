#include "RenderSystem.hpp"
#include <algorithm>
#include <cstddef>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "IDrawSurface.hpp"
#include "Position.hpp"
#include "Sprite.hpp"

namespace rtype::client {

void RenderSystem::render(engine::ComponentRegistry& components,
                          IDrawSurface& surface) {
  for (auto& drawCalls : drawCallsByLayer_) {
    drawCalls.clear();
  }
  components.forEach<game::Position, game::Sprite>(
      [this](engine::Entity entity, game::Position& position,
             game::Sprite& sprite) {
        drawCallsByLayer_[static_cast<std::size_t>(sprite.layer)].push_back(
            {.entityIndex = entity.index,
             .position = position,
             .assetId = &sprite.assetId});
      });
  for (auto& drawCalls : drawCallsByLayer_) {
    std::ranges::sort(drawCalls, {}, &DrawCall::entityIndex);
    for (const DrawCall& drawCall : drawCalls) {
      surface.draw(*drawCall.assetId, drawCall.position);
    }
  }
}

}  // namespace rtype::client
