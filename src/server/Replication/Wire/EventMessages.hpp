#pragma once

#include "BossArrival.hpp"
#include "BossHealthChanged.hpp"
#include "EffectStarted.hpp"
#include "EnemyActionStarted.hpp"
#include "EntityDestruction.hpp"
#include "EntitySpawn.hpp"
#include "EventMessage.hpp"
#include "LevelChange.hpp"
#include "PlayerDeparture.hpp"

namespace rtype::server {

/**
 * The events of the match that need no number become one message of the
 * protocol each, guaranteed or not; GameEventEncoder writes those that become a
 * numbered GameEvent. Each eventMessageOf() throws what the encodePayload() of
 * its message throws, InvalidFieldValueException for an entity id of 0.
 *
 * @returns An EntitySpawned, sent once: the next WorldState creates the entity
 * when it is lost.
 */
[[nodiscard]] EventMessage eventMessageOf(const game::EntitySpawn& event);

/** @returns An EntityDestroyed, guaranteed. */
[[nodiscard]] EventMessage eventMessageOf(const game::EntityDestruction& event);

/** @returns An EnemyAction, guaranteed. */
[[nodiscard]] EventMessage eventMessageOf(
    const game::EnemyActionStarted& event);

/** @returns An EffectPlayed, guaranteed. */
[[nodiscard]] EventMessage eventMessageOf(const game::EffectStarted& event);

/** @returns A PlayerLeft with the number of the player and why it left,
 * guaranteed. */
[[nodiscard]] EventMessage eventMessageOf(const game::PlayerDeparture& event);

/** @returns A BossStatus of a boss arriving, guaranteed. */
[[nodiscard]] EventMessage eventMessageOf(const game::BossArrival& event);

/** @returns A BossStatus of the boss in play, guaranteed: Defeated with no
 * point left, Enraged once enraged, Fighting otherwise. */
[[nodiscard]] EventMessage eventMessageOf(const game::BossHealthChanged& event);

/** @returns A LevelStatus of the biome and the scroll of the level,
 * guaranteed. */
[[nodiscard]] EventMessage eventMessageOf(const game::LevelChange& event);

}  // namespace rtype::server
