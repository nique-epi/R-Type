#pragma once

#include <cstdint>
#include <vector>
#include "EntityDestroyed.hpp"
#include "EntitySpawned.hpp"
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

}  // namespace rtype::client::testing
