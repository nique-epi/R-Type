#include "EntityReplicatorFixture.hpp"
#include <cstddef>
#include <ostream>
#include <vector>
#include "Entity.hpp"
#include "EntityReplicator.hpp"
#include "EntitySprites.hpp"
#include "EntityType.hpp"
#include "Position.hpp"
#include "Sprite.hpp"

namespace rtype::client::testing {

void PrintTo(const DrawnEntity& entity, std::ostream* output) {
  *output << "at " << entity.x << ", " << entity.y << " showing "
          << entity.assetId << " from " << entity.frame.left << ", "
          << entity.frame.top;
}

DrawnEntity drawnAs(game::EntityType type, game::Position position) {
  const game::Sprite sprite = spriteOf(type).value_or(game::Sprite{});
  return DrawnEntity{.x = position.x,
                     .y = position.y,
                     .assetId = sprite.assetId,
                     .frame = sprite.frame,
                     .layer = sprite.layer};
}

EntityReplicator& EntityReplicatorFixture::replicator() { return replicator_; }

std::vector<DrawnEntity> EntityReplicatorFixture::drawnEntities() {
  std::vector<DrawnEntity> drawn;
  components_.forEach<game::Position, game::Sprite>(
      [&drawn](engine::Entity, const game::Position& position,
               const game::Sprite& sprite) {
        drawn.push_back(DrawnEntity{.x = position.x,
                                    .y = position.y,
                                    .assetId = sprite.assetId,
                                    .frame = sprite.frame,
                                    .layer = sprite.layer});
      });
  return drawn;
}

std::size_t EntityReplicatorFixture::entityCount() const {
  return entities_.size();
}

}  // namespace rtype::client::testing
