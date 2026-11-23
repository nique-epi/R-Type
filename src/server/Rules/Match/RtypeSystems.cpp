#include "RtypeSystems.hpp"
#include <memory>
#include "BossSystem.hpp"
#include "BossSystemParts.hpp"
#include "CollisionSystem.hpp"
#include "EventBus.hpp"
#include "GameContext.hpp"
#include "GameSettings.hpp"
#include "HurtZoneSystem.hpp"
#include "LagCompensatedHitSystem.hpp"
#include "MovementSystem.hpp"
#include "PilotSystem.hpp"
#include "PlayerRoster.hpp"
#include "PlayfieldExitSystem.hpp"
#include "RtypeSystemConstants.hpp"
#include "SummoningSystem.hpp"
#include "SystemScheduler.hpp"
#include "ThreatReading.hpp"

namespace rtype::server {

namespace {

/** @returns What the boss system of @p game is built from: its bus, the
 * count of its connected players, the boss health percent of its waves and
 * the threat of @p threat. */
game::BossSystemParts bossSystemPartsOf(GameContext& game,
                                        const IThreatLevel* threat) {
  PlayerRoster* const roster = &game.roster.get();
  return game::BossSystemParts{
      .events = game.events,
      .players = [roster] { return roster->connected().size(); },
      .healthPercent = game.settings.waves.bossHealthPercent,
      .threatPercent = [threat] { return threatPercentOf(threat); }};
}

}  // namespace

void addRtypeSystems(GameContext& game, const IThreatLevel* threat) {
  engine::SystemScheduler& systems = game.systems;
  engine::EventBus& events = game.events;
  systems.add(
      std::make_unique<game::BossSystem>(bossSystemPartsOf(game, threat)),
      BOSS_SYSTEM_NAME);
  systems.add(std::make_unique<game::PilotSystem>(events), PILOT_SYSTEM_NAME);
  systems.add(std::make_unique<game::MovementSystem>(), MOVEMENT_SYSTEM_NAME);
  systems.add(std::make_unique<game::PlayfieldExitSystem>(events),
              PLAYFIELD_EXIT_SYSTEM_NAME);
  systems.add(std::make_unique<game::CollisionSystem>(events),
              COLLISION_SYSTEM_NAME);
  systems.add(std::make_unique<game::LagCompensatedHitSystem>(events),
              LAG_COMPENSATED_HIT_SYSTEM_NAME);
  systems.add(std::make_unique<game::HurtZoneSystem>(events),
              HURT_ZONE_SYSTEM_NAME);
  systems.add(std::make_unique<game::SummoningSystem>(), SUMMONING_SYSTEM_NAME);
}

}  // namespace rtype::server
