#pragma once

#include <SFML/Graphics/Color.hpp>
#include <optional>
#include <string>
#include "DownedShip.hpp"
#include "Entity.hpp"
#include "HitSpot.hpp"
#include "PlayerPalette.hpp"
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

/** @returns The Bydo nearest a missile of an enemy that appears at @p missile,
 * among those at its right, where a Bydo fires from, and where it stands;
 * nothing when no Bydo is at its right. */
[[nodiscard]] std::optional<Shooter> bydoShooterOf(
    engine::ComponentRegistry& components, game::Position missile);

/** @returns The color of the pilot of a ship, or nothing for a ship without
 * PilotShip or with a color outside 0 to PLAYER_COLOR_COUNT - 1. */
[[nodiscard]] std::optional<int> pilotColorOf(
    engine::ComponentRegistry& components, engine::Entity ship);

/** @returns The light shade @p palette gives a pilot color, or
 * UNKNOWN_PILOT_LIGHT when the pilot is not known. */
[[nodiscard]] sf::Color pilotLightOf(const PlayerPalette& palette,
                                     std::optional<int> pilotColor);

/** @returns How an effect names a pilot before a text: its number as the
 * waiting room writes it, then PILOT_LABEL_SEPARATOR; nothing when the pilot
 * is not known. */
[[nodiscard]] std::string pilotLabelOf(std::optional<int> pilotColor);

}  // namespace rtype::client
