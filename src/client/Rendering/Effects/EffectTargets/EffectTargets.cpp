#include "EffectTargets.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <limits>
#include <optional>
#include <string>
#include "ComponentRegistry.hpp"
#include "DownedShip.hpp"
#include "EffectColorConstants.hpp"
#include "EffectConstants.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "HitSpot.hpp"
#include "PilotColor.hpp"
#include "PilotShip.hpp"
#include "PlayerNumberLabel.hpp"
#include "PlayerPalette.hpp"
#include "Position.hpp"
#include "Shooter.hpp"
#include "Sprite.hpp"

namespace rtype::client {

namespace {

/** @returns The square of the distance between two points. */
float squaredDistance(game::Position from, game::Position target) {
  return (sf::Vector2f(target.x, target.y) - sf::Vector2f(from.x, from.y))
      .lengthSquared();
}

/** @brief Which entities may have fired a missile: those of that type, at its
 * right when fromTheRight, else at its left. */
struct ShotOrigin {
  game::EntityType type{};
  bool fromTheRight{};
};

/** @returns The entity nearest @p missile among those @p origin names, and
 * where it stands; nothing when there is none. */
std::optional<Shooter> nearestShooterOf(engine::ComponentRegistry& components,
                                        game::Position missile,
                                        ShotOrigin origin) {
  std::optional<Shooter> nearest;
  float nearestDistance = std::numeric_limits<float>::max();
  components.forEach<game::EntityType, game::Position>(
      [&nearest, &nearestDistance, missile, origin](engine::Entity entity,
                                                    game::EntityType& type,
                                                    game::Position& position) {
        const bool onItsSide = origin.fromTheRight ? position.x >= missile.x
                                                   : position.x <= missile.x;
        const float distance = squaredDistance(position, missile);
        if (type == origin.type && onItsSide && distance < nearestDistance) {
          nearest = Shooter{.ship = entity, .center = position};
          nearestDistance = distance;
        }
      });
  return nearest;
}

}  // namespace

std::optional<DownedShip> downedShipOf(engine::ComponentRegistry& components,
                                       engine::Entity ship) {
  const game::Position* position = components.get<game::Position>(ship);
  const game::Sprite* sprite = components.get<game::Sprite>(ship);
  if (position == nullptr || sprite == nullptr) {
    return std::nullopt;
  }
  return DownedShip{.center = *position,
                    .sprite = *sprite,
                    .pilotColor = pilotColorOf(components, ship)};
}

std::optional<HitSpot> hitSpotOf(engine::ComponentRegistry& components,
                                 engine::Entity entity) {
  const game::EntityType* type = components.get<game::EntityType>(entity);
  const game::Position* position = components.get<game::Position>(entity);
  if (type == nullptr || position == nullptr) {
    return std::nullopt;
  }
  return HitSpot{.type = *type, .center = *position};
}

std::optional<Shooter> shooterOf(engine::ComponentRegistry& components,
                                 game::Position missile) {
  return nearestShooterOf(
      components, missile,
      ShotOrigin{.type = game::EntityType::Player, .fromTheRight = false});
}

std::optional<Shooter> bydoShooterOf(engine::ComponentRegistry& components,
                                     game::Position missile) {
  return nearestShooterOf(
      components, missile,
      ShotOrigin{.type = game::EntityType::Enemy, .fromTheRight = true});
}

std::optional<int> pilotColorOf(engine::ComponentRegistry& components,
                                engine::Entity ship) {
  const PilotShip* pilot = components.get<PilotShip>(ship);
  if (pilot == nullptr || !isPilotColor(pilot->color)) {
    return std::nullopt;
  }
  return pilot->color;
}

sf::Color pilotLightOf(const PlayerPalette& palette,
                       std::optional<int> pilotColor) {
  if (!pilotColor.has_value()) {
    return UNKNOWN_PILOT_LIGHT;
  }
  return palette[static_cast<std::size_t>(*pilotColor)].light;
}

std::string pilotLabelOf(std::optional<int> pilotColor) {
  if (!pilotColor.has_value()) {
    return {};
  }
  return playerNumberLabel(playerNumberOfPilotColor(*pilotColor)) +
         PILOT_LABEL_SEPARATOR;
}

}  // namespace rtype::client
