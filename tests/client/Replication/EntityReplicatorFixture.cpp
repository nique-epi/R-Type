#include "EntityReplicatorFixture.hpp"
#include <SFML/Graphics/Color.hpp>
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
#include "HitFlash.hpp"
#include "Invulnerability.hpp"
#include "Layer.hpp"
#include "Lifetime.hpp"
#include "PilotShip.hpp"
#include "Position.hpp"
#include "Reveal.hpp"
#include "SpeedLine.hpp"
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
  std::vector<DrawnEntity> drawn;
  components_.forEach<game::EntityType, game::Sprite>(
      [this, &drawn](engine::Entity entity, const game::EntityType&,
                     const game::Sprite& sprite) {
        const game::Position* position =
            components_.get<game::Position>(entity);
        if (position != nullptr) {
          drawn.push_back(DrawnEntity{.x = position->x,
                                      .y = position->y,
                                      .assetId = sprite.assetId,
                                      .frame = sprite.frame,
                                      .layer = sprite.layer});
        }
      });
  return drawn;
}

std::vector<DrawnEntity> EntityReplicatorFixture::drawnEffects() {
  return drawnIn(game::Layer::Effects);
}

std::size_t EntityReplicatorFixture::animatedEntityCount() {
  std::size_t count = 0;
  components_.forEach<game::EntityType, SpriteAnimation>(
      [&count](engine::Entity, const game::EntityType&,
               const SpriteAnimation&) { ++count; });
  return count;
}

std::size_t EntityReplicatorFixture::announcedEntityCount() {
  std::size_t count = 0;
  components_.forEach<game::EntityType, game::EntityType>(
      [&count](engine::Entity, const game::EntityType&,
               const game::EntityType&) { ++count; });
  return count;
}

std::optional<int> EntityReplicatorFixture::pilotColorOfThe(
    game::EntityType type) {
  const std::optional<engine::Entity> entity = onlyEntityOf(type);
  const PilotShip* pilot =
      entity.has_value() ? components_.get<PilotShip>(*entity) : nullptr;
  return pilot != nullptr ? std::optional<int>(pilot->color) : std::nullopt;
}

bool EntityReplicatorFixture::isInvulnerable(game::EntityType type) {
  const std::optional<engine::Entity> entity = onlyEntityOf(type);
  return entity.has_value() && components_.has<Invulnerability>(*entity);
}

bool EntityReplicatorFixture::isFlashed(game::EntityType type) {
  const std::optional<engine::Entity> entity = onlyEntityOf(type);
  return entity.has_value() && components_.has<HitFlash>(*entity);
}

bool EntityReplicatorFixture::isArriving(game::EntityType type) {
  const std::optional<engine::Entity> entity = onlyEntityOf(type);
  return entity.has_value() && components_.has<Reveal>(*entity);
}

std::vector<sf::Color> EntityReplicatorFixture::speedLineColors() {
  std::vector<sf::Color> colors;
  components_.forEach<SpeedLine, game::Position>(
      [&colors](engine::Entity, const SpeedLine& line, const game::Position&) {
        colors.push_back(line.color);
      });
  return colors;
}

std::size_t EntityReplicatorFixture::frozenPictureCount() {
  std::size_t count = 0;
  components_.forEach<Lifetime, game::Sprite>(
      [&count](engine::Entity, const Lifetime&, const game::Sprite&) {
        ++count;
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

std::optional<engine::Entity> EntityReplicatorFixture::onlyEntityOf(
    game::EntityType type) {
  std::optional<engine::Entity> found;
  components_.forEach<game::EntityType, game::Position>(
      [&found, type](engine::Entity entity, const game::EntityType& entityType,
                     const game::Position&) {
        if (entityType == type) {
          found = entity;
        }
      });
  return found;
}

}  // namespace rtype::client::testing
