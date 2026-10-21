#include "EntityReplicatorFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <ostream>
#include <vector>
#include "AnimatedSprite.hpp"
#include "EffectConstants.hpp"
#include "Entity.hpp"
#include "EntityAnnouncements.hpp"
#include "EntityReplicator.hpp"
#include "EntityReplicatorTestConstants.hpp"
#include "EntitySprites.hpp"
#include "EntityType.hpp"
#include "Layer.hpp"
#include "Position.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteFrame.hpp"
#include "WorldState.hpp"

namespace rtype::client::testing {

void PrintTo(const DrawnEntity& entity, std::ostream* output) {
  *output << "at " << entity.x << ", " << entity.y << " showing "
          << entity.assetId << " from " << entity.frame.left << ", "
          << entity.frame.top;
}

DrawnEntity drawnAs(game::EntityType type, std::uint32_t networkId,
                    game::Position position) {
  const std::optional<AnimatedSprite> look = animatedSpriteOf(type, networkId);
  const game::Sprite sprite = look.has_value() ? look->sprite : game::Sprite{};
  return DrawnEntity{.x = position.x,
                     .y = position.y,
                     .assetId = sprite.assetId,
                     .frame = sprite.frame,
                     .layer = sprite.layer};
}

DrawnEntity effectAt(game::SpriteFrame firstFrame, game::Position position) {
  return DrawnEntity{.x = position.x,
                     .y = position.y,
                     .assetId = EFFECT_SHEET_ID,
                     .frame = firstFrame,
                     .layer = game::Layer::Effects};
}

EntityReplicator& EntityReplicatorFixture::replicator() { return replicator_; }

void EntityReplicatorFixture::announceOneOfEachType() {
  replicator_.apply(
      spawned(SHIP_ID, network::WireEntityType::PlayerShip, SHIP_POSITION));
  replicator_.apply(spawned(PLAYER_MISSILE_ID,
                            network::WireEntityType::PlayerMissile,
                            PLAYER_MISSILE_POSITION));
  replicator_.apply(
      spawned(BYDO_ID, network::WireEntityType::Bydo, BYDO_POSITION));
  replicator_.apply(spawned(ENEMY_MISSILE_ID,
                            network::WireEntityType::EnemyMissile,
                            ENEMY_MISSILE_POSITION));
}

std::vector<DrawnEntity> EntityReplicatorFixture::drawnEntities() {
  return drawnIn(game::Layer::Entities);
}

std::vector<DrawnEntity> EntityReplicatorFixture::drawnEffects() {
  return drawnIn(game::Layer::Effects);
}

std::size_t EntityReplicatorFixture::animatedEntityCount() {
  std::size_t count = 0;
  components_.forEach<game::Sprite, SpriteAnimation>(
      [&count](engine::Entity, const game::Sprite& sprite,
               const SpriteAnimation&) {
        if (sprite.layer == game::Layer::Entities) {
          ++count;
        }
      });
  return count;
}

std::size_t EntityReplicatorFixture::entityCount() const {
  return entities_.size();
}

std::vector<DrawnEntity> EntityReplicatorFixture::drawnIn(game::Layer layer) {
  std::vector<DrawnEntity> drawn;
  components_.forEach<game::Position, game::Sprite>(
      [&drawn, layer](engine::Entity, const game::Position& position,
                      const game::Sprite& sprite) {
        if (sprite.layer == layer) {
          drawn.push_back(DrawnEntity{.x = position.x,
                                      .y = position.y,
                                      .assetId = sprite.assetId,
                                      .frame = sprite.frame,
                                      .layer = sprite.layer});
        }
      });
  return drawn;
}

}  // namespace rtype::client::testing
