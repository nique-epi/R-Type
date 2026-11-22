#include "MatchSystems.hpp"
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string_view>
#include "BossStage.hpp"
#include "BossStageParts.hpp"
#include "CollisionRules.hpp"
#include "DamageRules.hpp"
#include "EntityOrders.hpp"
#include "EntityOrdersParts.hpp"
#include "GameContext.hpp"
#include "GameInstanceConstants.hpp"
#include "GameSettings.hpp"
#include "IEntityOrders.hpp"
#include "IHitArbitration.hpp"
#include "LevelConstants.hpp"
#include "LevelDirector.hpp"
#include "LevelDirectorParts.hpp"
#include "LevelTransition.hpp"
#include "LifeKeeper.hpp"
#include "LifeKeeperParts.hpp"
#include "LookItemKeeper.hpp"
#include "LookItemKeeperParts.hpp"
#include "LuauLevelEntry.hpp"
#include "MatchSystemsParts.hpp"
#include "PlayerController.hpp"
#include "ServerException.hpp"
#include "ShipBand.hpp"
#include "TimeConstants.hpp"
#include "TransitionHold.hpp"
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

/**
 * @brief Holds the ships of @p world while the level @p director directs plays
 * a transition (see holdEveryShip()), and puts them back at their start once
 * LEVEL_TRANSITION_RETURN_TICKS are left of it; nothing in a game that plays
 * its waves.
 */
void holdTheShipsOfATransition(GameContext& game, game::World& world,
                               const LevelDirector* director) {
  if (director == nullptr) {
    return;
  }
  const std::optional<game::LevelTransition>& transition =
      director->progress().transition;
  if (!transition.has_value()) {
    return;
  }
  const std::uint32_t ticksLeft = transition->ticksLeft;
  holdEveryShip(world, game.roster.get(), ticksLeft);
  if (ticksLeft == game::LEVEL_TRANSITION_RETURN_TICKS) {
    returnEveryShip(world, game.roster.get());
  }
}

/** @brief Gives the ships of @p world the band of the biome the level
 * @p director directs shows (see bandEveryShip()); nothing in a game that
 * plays its waves, whose ships fly in the whole play area. */
void keepTheShipsInTheirBand(GameContext& game, game::World& world,
                             const LevelDirector* director) {
  if (director == nullptr) {
    return;
  }
  bandEveryShip(world, game.roster.get(), director->band());
}

/** @returns The boss stage of the level played in @p world, its boss created
 * through @p orders; nullptr in a game that plays a level a script directs. */
std::unique_ptr<BossStage> bossStageFor(GameContext& game, game::World& world,
                                        game::IEntityOrders& orders) {
  if (game.settings.level.has_value()) {
    return nullptr;
  }
  return std::make_unique<BossStage>(BossStageParts{
      .world = world, .orders = orders, .boss = game.settings.waves.boss});
}

/** @returns The spawner of the Bydos of @p world, its draws from @p seed;
 * nullptr in a game that plays a level a script directs. */
std::unique_ptr<WaveSpawner> waveSpawnerFor(GameContext& game,
                                            game::World& world,
                                            std::uint32_t seed) {
  if (game.settings.level.has_value()) {
    return nullptr;
  }
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
std::unique_ptr<EntityOrders> entityOrdersFor(GameContext& game,
                                              game::World& world) {
  return std::make_unique<EntityOrders>(
      EntityOrdersParts{.world = world,
                        .roster = game.roster.get(),
                        .send = game.send,
                        .types = game.settings.types,
                        .events = game.events.get(),
                        .scripts = game.settings.scripts});
}

/** @returns What directs the level the game of @p game plays in @p world,
 * its placed enemies created through @p orders; nullptr for a game that
 * plays the waves of its difficulty.
 * @throws UnknownLevelException when no script registered that level, the
 * game having no scripts included. */
std::unique_ptr<LevelDirector> levelDirectorFor(GameContext& game,
                                                game::World& world,
                                                game::IEntityOrders& orders) {
  const GameSettings& settings = game.settings;
  if (!settings.level.has_value()) {
    return nullptr;
  }
  if (settings.scripts == nullptr) {
    throw UnknownLevelException(*settings.level, {});
  }
  return std::make_unique<LevelDirector>(
      LevelDirectorParts{.world = world,
                         .events = game.events.get(),
                         .orders = orders,
                         .scripts = *settings.scripts,
                         .level = *settings.level,
                         .startingStage = settings.startingStage,
                         .continues = settings.continues});
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
      entityOrders_(entityOrdersFor(game_, world_)),
      levelDirector_(levelDirectorFor(game_, world_, *entityOrders_)),
      bossStage_(bossStageFor(game_, world_, *entityOrders_)),
      lifeKeeper_(lifeKeeperFor(parts)),
      lookItemKeeper_(lookItemKeeperFor(game_, world_)) {}

void MatchSystems::onTick(IHitArbitration& arbitration, std::uint32_t tick) {
  GameContext& game = game_.get();
  lookItemKeeper_->onTick();
  arbitration.giveViews(world_, game.roster.get(), tick);
  if (levelDirector_ != nullptr) {
    levelDirector_->onTick();
  }
  holdTheShipsOfATransition(game, world_, levelDirector_.get());
  keepTheShipsInTheirBand(game, world_, levelDirector_.get());
  runWorldSystems(game, world_);
  weaponSystem_->onTick();
  volleySystem_->onTick();
  entityOrders_->onTick();
  if (levelDirector_ == nullptr) {
    bossStage_->onTick(
        advanceTheWaves(*waveSpawner_, game.settings.startsAtBoss));
  }
  lifeKeeper_->onTick();
}

std::optional<std::string_view> MatchSystems::victoryReason() const {
  if (levelDirector_ != nullptr) {
    return levelDirector_->cleared()
               ? std::optional{LEVEL_CLEARED_FINISH_REASON}
               : std::nullopt;
  }
  if (!bossStage_->levelCleared()) {
    return std::nullopt;
  }
  return bossStage_->hasBoss() ? BOSS_DEFEATED_FINISH_REASON
                               : LAST_WAVE_OVER_FINISH_REASON;
}

bool MatchSystems::levelFailed() const {
  return levelDirector_ != nullptr && levelDirector_->failed();
}

std::uint32_t MatchSystems::clearReward() const {
  const GameSettings& settings = game_.get().settings;
  if (!settings.level.has_value() || settings.scripts == nullptr) {
    return 0;
  }
  return entryOfLevel(settings.scripts->levels(), *settings.level).reward;
}

bool MatchSystems::restartFromCheckpoint() {
  if (levelDirector_ == nullptr || !levelDirector_->restartFromCheckpoint()) {
    return false;
  }
  lifeKeeper_->reviveTheTeam(game_.get().settings.rules.lives);
  return true;
}

}  // namespace rtype::server
