#pragma once

#include <SFML/Graphics/Color.hpp>
#include <optional>
#include "DownedShip.hpp"
#include "Entity.hpp"
#include "HitSpot.hpp"
#include "Position.hpp"
#include "Shooter.hpp"

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::client {

/** @returns What a ship leaves to its effects, or nothing for a ship without
 * Position or Sprite. */
[[nodiscard]] std::optional<DownedShip> downedShipOf(
    engine::ComponentRegistry& components, engine::Entity ship);

/** @returns What an entity hit is and where it stands, or nothing for an
 * entity without EntityType or Position. */
[[nodiscard]] std::optional<HitSpot> hitSpotOf(
    engine::ComponentRegistry& components, engine::Entity entity);

/** @returns The nearest ship at the left of a missile of a player that
 * appears at @p missile, or nothing. */
[[nodiscard]] std::optional<Shooter> shooterOf(
    engine::ComponentRegistry& components, game::Position missile);

/** @returns The color of the pilot of a ship, or nothing for a ship without
 * PilotShip or with a color outside 0 to PLAYER_COLOR_COUNT - 1. */
[[nodiscard]] std::optional<int> pilotColorOf(
    engine::ComponentRegistry& components, engine::Entity ship);

/** @returns The light shade of a pilot color, or UNKNOWN_PILOT_LIGHT when the
 * pilot is not known. */
[[nodiscard]] sf::Color pilotLightOf(std::optional<int> pilotColor);

}  // namespace rtype::client
