#pragma once

namespace rtype::server {

struct GameContext;
class IThreatLevel;

/**
 * @brief Adds the systems that run on the world of the R-Type game @p game
 * each tick to its scheduler, in the order they run: BossSystem, PilotSystem,
 * MovementSystem, PlayfieldExitSystem, CollisionSystem,
 * LagCompensatedHitSystem, HurtZoneSystem, then SummoningSystem. They publish
 * on the bus of
 * @p game, and BossSystem scales a boss for the players of its roster and
 * the boss health percent of its waves, raised for the threat of @p threat
 * as its warning ends (none when it is nullptr, in a game without the module
 * threat); @p game and @p threat must outlive its systems.
 */
void addRtypeSystems(GameContext& game, const IThreatLevel* threat);

}  // namespace rtype::server
