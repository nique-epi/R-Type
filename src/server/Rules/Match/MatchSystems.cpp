#include "MatchSystems.hpp"
#include <cstdint>
#include <functional>
#include <memory>
#include "BodyOrders.hpp"
#include "BodyOrdersParts.hpp"
#include "BossStage.hpp"
#include "BossStageParts.hpp"
#include "CollisionRules.hpp"
#include "DamageRules.hpp"
#include "GameContext.hpp"
#include "IHitArbitration.hpp"
#include "LifeKeeper.hpp"
#include "LifeKeeperParts.hpp"
#include "LookItemKeeper.hpp"
#include "LookItemKeeperParts.hpp"
#include "MatchSystemsParts.hpp"
#include "PlayerController.hpp"
#include "TimeConstants.hpp"
#include "VolleySystem.hpp"
#include "VolleySystemParts.hpp"
#include "WaveSpawner.hpp"
#include "WaveSpawnerParts.hpp"
#include "WeaponSystem.hpp"
#include "WeaponSystemParts.hpp"
#include "World.hpp"

namespace rtype::server {

namespace {

/**
 * @brief Makes the ship of every player who is away want nothing (see
 * silenceAwayShips), then runs the systems of @p world for one tick.
 */
void runWorldSystems(GameContext& game, game::World& world) {
  silenceAwayShips(world, game.roster.get());
  game.systems.get().run(world.components(), engine::SIMULATION_TICK_DURATION);
}

/** @returns The boss stage of the level played in @p world. */
std::unique_ptr<BossStage> bossStageFor(GameContext& game, game::World& world) {
  return std::make_unique<BossStage>(
      BossStageParts{.world = world,
                     .roster = game.roster.get(),
                     .send = game.send,
                     .events = game.events.get(),
                     .types = game.settings.types,
                     .boss = game.settings.waves.boss,
                     .healthPercent = game.settings.waves.bossHealthPercent});
}

/** @returns The spawner of the Bydos of @p world, its draws from @p seed. */
std::unique_ptr<WaveSpawner> waveSpawnerFor(GameContext& game,
                                            game::World& world,
                                            std::uint32_t seed) {
  return std::make_unique<WaveSpawner>(
      WaveSpawnerParts{.world = world,
                       .roster = game.roster.get(),
                       .send = game.send,
                       .seed = seed,
                       .waves = game.settings.waves,
                       .types = game.settings.types});
}

/** @returns The weapon system of the missiles of @p world. */
std::unique_ptr<WeaponSystem> weaponSystemFor(GameContext& game,
                                              game::World& world) {
  return std::make_unique<WeaponSystem>(
      WeaponSystemParts{.world = world,
                        .roster = game.roster.get(),
                        .send = game.send,
                        .types = game.settings.types,
                        .events = game.events.get()});
}

/** @returns The volley system of the enemies of @p world. */
std::unique_ptr<VolleySystem> volleySystemFor(GameContext& game,
                                              game::World& world) {
  return std::make_unique<VolleySystem>(
      VolleySystemParts{.world = world,
                        .roster = game.roster.get(),
                        .send = game.send,
                        .types = game.settings.types,
                        .seed = game.settings.seed});
}

/** @returns What carries out, in @p world, what the scripts of the game ask. */
std::unique_ptr<BodyOrders> bodyOrdersFor(GameContext& game,
                                          game::World& world) {
  return std::make_unique<BodyOrders>(
      BodyOrdersParts{.world = world,
                      .roster = game.roster.get(),
                      .send = game.send,
                      .types = game.settings.types,
                      .events = game.events.get(),
                      .scripts = game.settings.scripts});
}

/** @returns Whether the boss of the level may come at this tick, after
 * moving @p waves on a tick: once the last wave is over, or at once, without
 * any wave, in a game that @p startsAtBoss. */
bool advanceTheWaves(WaveSpawner& waves, bool startsAtBoss) {
  if (startsAtBoss) {
    return true;
  }
  waves.onTick();
  return waves.lastWaveFinished();
}

/** @returns The keeper of the lives of the players of the world of @p parts. */
std::unique_ptr<LifeKeeper> lifeKeeperFor(const MatchSystemsParts& parts) {
  const GameContext& game = parts.game.get();
  return std::make_unique<LifeKeeper>(
      LifeKeeperParts{.events = game.events.get(),
                      .world = parts.world.get(),
                      .roster = game.roster.get(),
                      .respawn = parts.respawn,
                      .shipsNeverDie = game.settings.startsAtBoss});
}

/** @returns The keeper of the look items the players of @p world win. */
std::unique_ptr<LookItemKeeper> lookItemKeeperFor(GameContext& game,
                                                  game::World& world) {
  return std::make_unique<LookItemKeeper>(
      LookItemKeeperParts{.events = game.events.get(),
                          .world = world,
                          .roster = game.roster.get(),
                          .send = game.send});
}

}  // namespace

MatchSystems::MatchSystems(const MatchSystemsParts& parts)
    : game_(parts.game),
      world_(parts.world),
      collisionRules_(std::make_unique<game::CollisionRules>(
          game_.get().events.get(), world_.get().components(),
          game_.get().settings.rules.friendlyFire)),
      damageRules_(std::make_unique<game::DamageRules>(
          game_.get().events.get(), world_.get().components())),
      waveSpawner_(waveSpawnerFor(game_, world_, parts.waveSeed)),
      weaponSystem_(weaponSystemFor(game_, world_)),
      volleySystem_(volleySystemFor(game_, world_)),
      bodyOrders_(bodyOrdersFor(game_, world_)),
      bossStage_(bossStageFor(game_, world_)),
      lifeKeeper_(lifeKeeperFor(parts)),
      lookItemKeeper_(lookItemKeeperFor(game_, world_)) {}

void MatchSystems::onTick(IHitArbitration& arbitration, std::uint32_t tick) {
  GameContext& game = game_.get();
  lookItemKeeper_->onTick();
  arbitration.giveViews(world_, game.roster.get(), tick);
  runWorldSystems(game, world_);
  weaponSystem_->onTick();
  volleySystem_->onTick();
  bodyOrders_->onTick();
  bossStage_->onTick(
      advanceTheWaves(*waveSpawner_, game.settings.startsAtBoss));
  lifeKeeper_->onTick();
}

const BossStage& MatchSystems::bossStage() const { return *bossStage_; }

}  // namespace rtype::server
