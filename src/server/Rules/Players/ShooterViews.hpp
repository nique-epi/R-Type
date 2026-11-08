#pragma once

#include <cstddef>
#include <cstdint>
#include "PlayerRoster.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @brief Gives every missile of a player in @p world the ViewDelay of its
 * player at tick @p tick: how many ticks before @p tick the client of that
 * player showed the enemies when it sent the input applied last
 * (the shownTick of ConnectedPlayer::appliedInput). 0 for a player whose client
 * showed no state yet, showed a tick not before @p tick, or is not in @p
 * roster; capViewDelays() bounds it for a mode. Call it
 * before the systems of the tick run, never during a forEach of the registry.
 */
void giveShooterViews(game::World& world, PlayerRoster& roster,
                      std::uint32_t tick);

/**
 * @brief Gives the ship of every player in @p world the ViewDelay of its
 * player at tick @p tick, as giveShooterViews() does for the missiles: the
 * ship is then tested against the enemies and their missiles its player saw
 * (see LagCompensatedHitSystem). 0 for a ship of no player of @p roster.
 * Call it before the systems of the tick run, never during a forEach.
 */
void giveShipViews(game::World& world, PlayerRoster& roster,
                   std::uint32_t tick);

/** @brief Brings every ViewDelay of @p world above @p mostTicks down to it:
 * the furthest a mode of play looks back. */
void capViewDelays(game::World& world, std::size_t mostTicks);

}  // namespace rtype::server
