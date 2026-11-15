#pragma once

namespace rtype::server {

struct GameContext;

/**
 * @brief Adds the systems that run on the world of the R-Type game @p game
 * each tick to its scheduler, in the order they run: BossSystem, PilotSystem,
 * MovementSystem, PlayfieldExitSystem, CollisionSystem,
 * LagCompensatedHitSystem, HurtZoneSystem, then SummoningSystem. They publish
 * on the bus of
 * @p game, and BossSystem scales a boss for the players of its roster and
 * the boss health percent of its waves; @p game must outlive its systems.
 */
void addRtypeSystems(GameContext& game);

}  // namespace rtype::server
