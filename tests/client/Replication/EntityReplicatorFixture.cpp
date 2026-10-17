#include "EntityReplicatorFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <utility>
#include <vector>
#include "Entity.hpp"
#include "EntityDestroyed.hpp"
#include "EntityReplicator.hpp"
#include "EntitySpawned.hpp"
#include "EntitySprites.hpp"
#include "EntityType.hpp"
#include "Position.hpp"
#include "Sprite.hpp"
#include "WorldState.hpp"

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

network::EntitySpawned spawned(std::uint32_t entityId,
                               network::WireEntityType type,
                               game::Position position) {
  return network::EntitySpawned{.entityId = entityId,
                                .entityType = type,
                                .x = position.x,
                                .y = position.y};
}

network::EntityState listed(std::uint32_t entityId,
                            network::WireEntityType type,
                            game::Position position) {
  return network::EntityState{.entityId = entityId,
                              .entityType = type,
                              .x = position.x,
                              .y = position.y};
}

network::WorldState stateListing(std::uint32_t tick,
                                 std::vector<network::EntityState> entities) {
  return network::WorldState{
      .tick = tick, .players = {}, .entities = std::move(entities)};
}

network::EntityDestroyed destroyed(std::uint32_t entityId) {
  return network::EntityDestroyed{.entityId = entityId,
                                  .reason = network::DestructionReason::Killed};
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
