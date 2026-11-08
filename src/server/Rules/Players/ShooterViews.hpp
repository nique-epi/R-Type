#pragma once

#include <cstdint>
#include "PlayerRoster.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @brief Gives every missile of a player in @p world the ViewDelay of its
 * player at tick @p tick: how many ticks before @p tick the client of that
 * player showed the enemies when it sent the input applied last
 * (ConnectedPlayer::appliedShownTick). 0 for a player whose client showed no
 * state yet, showed a tick not before @p tick, or is not in @p roster; the
 * LagCompensatedHitSystem caps it at MAXIMUM_REWIND_TICKS. Call it before the
 * systems of the tick run, never during a forEach of the registry.
 */
void giveShooterViews(game::World& world, PlayerRoster& roster,
                      std::uint32_t tick);

}  // namespace rtype::server
