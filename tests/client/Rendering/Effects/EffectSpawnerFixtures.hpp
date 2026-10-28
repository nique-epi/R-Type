#pragma once

#include <SFML/Graphics/Color.hpp>
#include <cstddef>
#include <optional>
#include <vector>
#include "Caption.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "HitFlash.hpp"
#include "Position.hpp"
#include "Sprite.hpp"
#include "SpriteTone.hpp"
#include "TimeConstants.hpp"

/** @returns The white picture a ship destroyed leaves, or nothing. */
[[nodiscard]] std::optional<rtype::engine::Entity> frozenPicture(
    rtype::engine::ComponentRegistry& components);

/** @returns How many entities carry a component of that type and a
 * Position. */
template <typename Component>
[[nodiscard]] std::size_t count(rtype::engine::ComponentRegistry& components) {
  std::size_t found = 0;
  components.forEach<Component, rtype::game::Position>(
      [&found](rtype::engine::Entity, const Component&,
               const rtype::game::Position&) { ++found; });
  return found;
}

/** @returns How much longer the first frame of an effect stays. */
[[nodiscard]] rtype::engine::Duration framesHeldFor(
    rtype::engine::ComponentRegistry& components, rtype::engine::Entity effect);

/** @returns The tone an entity is drawn with by its hit flash, Normal
 * without one. */
[[nodiscard]] rtype::client::SpriteTone toneOf(
    rtype::engine::ComponentRegistry& components, rtype::engine::Entity entity);

/** @returns The entity an effect follows, or nothing. */
[[nodiscard]] std::optional<rtype::engine::Entity> anchorOf(
    rtype::engine::ComponentRegistry& components, rtype::engine::Entity effect);

/** @returns Whether the entity is drawn a few pixels away for a moment. */
[[nodiscard]] bool isNudged(const rtype::engine::ComponentRegistry& components,
                            rtype::engine::Entity entity);

/** @returns The age of the reveal of a ship arriving, or zero without
 * one. */
[[nodiscard]] rtype::engine::Duration revealAgeOf(
    rtype::engine::ComponentRegistry& components, rtype::engine::Entity ship);

/** @returns Whether the ship is invulnerable and dithered all along, never
 * blinking. */
[[nodiscard]] bool isSteadilyInvulnerable(
    rtype::engine::ComponentRegistry& components, rtype::engine::Entity ship);

/** @returns Whether every particle goes through that color as it ages. */
[[nodiscard]] bool everyParticleGoesThrough(
    rtype::engine::ComponentRegistry& components, sf::Color color);

/** @returns Whether a particle starts in that color. */
[[nodiscard]] bool anyParticleStartsWith(
    rtype::engine::ComponentRegistry& components, sf::Color color);

/** @returns The first caption written at a Position, the figure of a gain or a
 * sound cue, or nothing. */
[[nodiscard]] std::optional<rtype::client::Caption> caption(
    rtype::engine::ComponentRegistry& components);

/** @returns The first entity that writes a Caption at a Position; nothing
 * when there is none. */
[[nodiscard]] std::optional<rtype::engine::Entity> captioned(
    rtype::engine::ComponentRegistry& components);

/** @returns The entities drawn in the Effects layer, in index order. */
[[nodiscard]] std::vector<rtype::engine::Entity> shownEffects(
    rtype::engine::ComponentRegistry& components);

/** @returns The sprite an entity is drawn with, an empty one without. */
[[nodiscard]] rtype::game::Sprite drawnSprite(
    rtype::engine::ComponentRegistry& components, rtype::engine::Entity entity);

/** @returns The position of an entity, the origin without one. */
[[nodiscard]] rtype::game::Position positionOf(
    rtype::engine::ComponentRegistry& components, rtype::engine::Entity entity);

/** @returns The flash of an entity, or nothing when it has none. */
[[nodiscard]] const rtype::client::HitFlash* flashOf(
    rtype::engine::ComponentRegistry& components, rtype::engine::Entity entity);
