#include "EntityReplicatorFixtures.hpp"
#include <SFML/Graphics/Color.hpp>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <ostream>
#include <vector>
#include "AnimatedSprite.hpp"
#include "ComponentRegistry.hpp"
#include "EffectConstants.hpp"
#include "Entity.hpp"
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

namespace rtype::client::testing {

namespace {

/**
 * @returns Every entity of @p layer that has a Position and a Sprite.
 */
std::vector<DrawnEntity> drawnIn(engine::ComponentRegistry& components,
                                 game::Layer layer) {
  std::vector<DrawnEntity> drawn;
  components.forEach<game::Position, game::Sprite>(
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

/** @returns The only entity of that type, or nothing. */
std::optional<engine::Entity> onlyEntityOf(
    engine::ComponentRegistry& components, game::EntityType type) {
  std::optional<engine::Entity> found;
  components.forEach<game::EntityType, game::Position>(
      [&found, type](engine::Entity entity, const game::EntityType& entityType,
                     const game::Position&) {
        if (entityType == type) {
          found = entity;
        }
      });
  return found;
}

}  // namespace

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

std::vector<DrawnEntity> drawnEntities(engine::ComponentRegistry& components) {
  std::vector<DrawnEntity> drawn;
  components.forEach<game::EntityType, game::Sprite>(
      [&components, &drawn](engine::Entity entity, const game::EntityType&,
                            const game::Sprite& sprite) {
        const game::Position* position = components.get<game::Position>(entity);
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

std::vector<DrawnEntity> drawnEffects(engine::ComponentRegistry& components) {
  return drawnIn(components, game::Layer::Effects);
}

std::size_t animatedEntityCount(engine::ComponentRegistry& components) {
  std::size_t count = 0;
  components.forEach<game::EntityType, SpriteAnimation>(
      [&count](engine::Entity, const game::EntityType&,
               const SpriteAnimation&) { ++count; });
  return count;
}

std::optional<int> pilotColorOfThe(engine::ComponentRegistry& components,
                                   game::EntityType type) {
  const std::optional<engine::Entity> entity = onlyEntityOf(components, type);
  const PilotShip* pilot =
      entity.has_value() ? components.get<PilotShip>(*entity) : nullptr;
  return pilot != nullptr ? std::optional<int>(pilot->color) : std::nullopt;
}

bool isInvulnerable(engine::ComponentRegistry& components,
                    game::EntityType type) {
  const std::optional<engine::Entity> entity = onlyEntityOf(components, type);
  return entity.has_value() && components.has<Invulnerability>(*entity);
}

bool isFlashed(engine::ComponentRegistry& components, game::EntityType type) {
  const std::optional<engine::Entity> entity = onlyEntityOf(components, type);
  return entity.has_value() && components.has<HitFlash>(*entity);
}

bool isArriving(engine::ComponentRegistry& components, game::EntityType type) {
  const std::optional<engine::Entity> entity = onlyEntityOf(components, type);
  return entity.has_value() && components.has<Reveal>(*entity);
}

std::vector<sf::Color> speedLineColors(engine::ComponentRegistry& components) {
  std::vector<sf::Color> colors;
  components.forEach<SpeedLine, game::Position>(
      [&colors](engine::Entity, const SpeedLine& line, const game::Position&) {
        colors.push_back(line.color);
      });
  return colors;
}

std::size_t frozenPictureCount(engine::ComponentRegistry& components) {
  std::size_t count = 0;
  components.forEach<Lifetime, game::Sprite>(
      [&count](engine::Entity, const Lifetime&, const game::Sprite&) {
        ++count;
      });
  return count;
}

std::size_t announcedEntityCount(engine::ComponentRegistry& components) {
  std::size_t count = 0;
  components.forEach<game::EntityType, game::EntityType>(
      [&count](engine::Entity, const game::EntityType&,
               const game::EntityType&) { ++count; });
  return count;
}

}  // namespace rtype::client::testing
