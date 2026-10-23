#pragma once

#include <cstdint>
#include <vector>
#include "EntityDestroyed.hpp"
#include "EntitySpawned.hpp"
#include "GameEvent.hpp"
#include "Position.hpp"
#include "WorldState.hpp"

namespace rtype::client::testing {

/**
 * @returns An EntitySpawned announcing the entity.
 */
network::EntitySpawned spawned(std::uint32_t entityId,
                               network::WireEntityType type,
                               game::Position position);

/**
 * @returns The entity a WorldState lists.
 */
network::EntityState listed(std::uint32_t entityId,
                            network::WireEntityType type,
                            game::Position position);

/**
 * @returns A WorldState of tick @p tick listing @p entities, and no player.
 */
network::WorldState stateListing(std::uint32_t tick,
                                 std::vector<network::EntityState> entities);

/**
 * @returns A WorldState of tick @p tick listing @p players, and no entity.
 */
network::WorldState stateListingPlayers(
    std::uint32_t tick, std::vector<network::PlayerState> players);

/**
 * @returns An EntityDestroyed naming the entity, killed: its explosion shows.
 */
network::EntityDestroyed killed(std::uint32_t entityId);

/**
 * @returns An EntityDestroyed naming the entity, removed: nothing shows.
 */
network::EntityDestroyed removed(std::uint32_t entityId);

/**
 * @returns A player of a WorldState flying the ship @p shipEntityId: 3 lives,
 * a score of 0, and it can be hit.
 */
network::PlayerState playerFlying(std::uint8_t playerNumber,
                                  std::uint32_t shipEntityId);

/**
 * @returns A player of the second pilot, SECOND_PLAYER_NUMBER, flying the
 * ship SHIP_ID, able or not to be hit.
 */
network::PlayerState secondPilotFlying(bool invulnerable);

/**
 * @returns A GameEvent of that number saying the entity was hit.
 */
network::GameEvent damaged(std::uint16_t eventNumber, std::uint32_t entityId);

/**
 * @returns A GameEvent of that number saying the player was hit.
 */
network::GameEvent playerHit(std::uint16_t eventNumber,
                             std::uint8_t playerNumber);

}  // namespace rtype::client::testing
