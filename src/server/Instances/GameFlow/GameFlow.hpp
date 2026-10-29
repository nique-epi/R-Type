#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include "BydoSpawner.hpp"
#include "CollisionRules.hpp"
#include "DamageRules.hpp"
#include "GameContext.hpp"
#include "GameState.hpp"
#include "IGameFlow.hpp"
#include "IGameReplication.hpp"
#include "LifeKeeper.hpp"
#include "Logger.hpp"
#include "WeaponSystem.hpp"
#include "World.hpp"
#include "WorldSlot.hpp"

namespace rtype::server {

/**
 * @brief How a game runs: its state, its tick and its world.
 *
 * A game goes from WaitingForPlayers to InProgress when at least one player is
 * connected and every connected player is ready, from InProgress to Finished
 * when finish() is called, once no player is left in it, or once no player
 * left in it has a life, and from Finished back to WaitingForPlayers after
 * FINISHED_STATE_TICKS ticks. Coming back to WaitingForPlayers builds a new
 * world: every player gets a new ship and its STARTING_LIVES back, is no longer
 * ready, and the tick is 0. The players stay connected.
 *
 * The flow tells the players where the game stands, as the A session section
 * of the protocol page describes, through the IGameReplication it is given.
 * While the game waits for players, the replication sends them the room; it
 * builds a new announcer of the room with each new world, so the room goes out
 * at the first tick of every wait. The tick that starts the game sends every
 * connected player GameStarted, then, on that tick and every tick of the game,
 * the replication sends them the world, from tick 0: GameStarted always leaves
 * before the first WorldState. A game that ends by itself tells the
 * replication, which reads the score of every player then and sends them that
 * result at the first tick of the end, then every GAME_ENDED_INTERVAL until
 * the game waits for players again; no WorldState goes out meanwhile.
 *
 * While the game is in progress, each tick first makes the ship of every away
 * player want nothing (see silenceAwayShips) and writes the Intent of every
 * Bydo (see steerBydos), while the Intent of the ship of a player is its last
 * input (see steerShipByInput), then runs
 * the systems of the world (PatternSystem, MovementSystem, PlayfieldExitSystem,
 * which removes a Bydo or a missile of a Bydo that left on the left, or a
 * missile of a player that left on the right, and announces it on the event
 * bus, then CollisionSystem), then fires the Gun of every ship and Bydo whose
 * Intent wants to shoot: a ship at most one missile every SHOT_INTERVAL_TICKS
 * ticks, a Bydo one to the left every ENEMY_SHOT_INTERVAL (see WeaponSystem),
 * then lets the Bydos of the current wave appear (see BydoSpawner), then counts
 * the waits and the protections of the lives (see LifeKeeper). The ships and
 * the Bydos move and fire in no other state. Each world starts its random draws
 * from a seed of its own, the seed of the game plus the number of worlds built
 * before it, so the seed of the game is enough to replay every world of a
 * session.
 *
 * The collisions CollisionSystem finds are judged when the event bus delivers
 * them: CollisionRules turns a missile of a player that touches a Bydo into a
 * MissileHit, and DamageRules destroys the missile, takes MISSILE_DAMAGE from
 * the health of the Bydo and destroys the Bydo once it has none left, an
 * EnemyKilled crediting its points to the player who fired. CollisionRules also
 * turns a ship that touches a Bydo into a ShipContact, which LifeKeeper turns
 * into the loss of the ship, the Bydo and a life, and a missile of a Bydo that
 * touches a ship into a MissileHit, which DamageRules turns into the removal of
 * the missile and LifeKeeper into the loss of the ship and a life: the ship
 * comes back RESPAWN_DELAY later, invulnerable for INVULNERABILITY_DURATION,
 * and a player with no life left watches. A life is taken while the bus is
 * dispatched, after onTick(), so a game whose last life was taken ends at the
 * next onTick(), before the game moves along its states. The rules work on the
 * world they were built with, and new ones come with each new world, so the
 * events of a tick must be delivered before the world is replaced: the server
 * dispatches the bus at the end of every tick.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class GameFlow final : public IGameFlow {
 public:
  /**
   * @param game The context of the game; it must outlive the flow. The flow
   * reads its roster, bus, systems, settings, send function and ready rule.
   * @param world Holds the world of the game, which the flow replaces with
   * each new world; it must outlive the flow.
   * @param replication Tells the players how the game stands, and gives them
   * their ships; it must outlive the flow.
   */
  GameFlow(GameContext& game, WorldSlot& world, IGameReplication& replication);

  GameFlow(const GameFlow&) = delete;
  GameFlow& operator=(const GameFlow&) = delete;
  GameFlow(GameFlow&&) = delete;
  GameFlow& operator=(GameFlow&&) = delete;
  ~GameFlow() override = default;

  /**
   * @brief Builds the first world and the rules that work on it, which
   * subscribe to the event bus. Called once, before the first onTick().
   */
  void begin();

  [[nodiscard]] GameState state() const override;

  /**
   * @returns How many ticks were simulated since the game started: 0 while it
   * waits for players, and again once it waits after a game.
   */
  [[nodiscard]] std::uint32_t tick() const override;

  /**
   * @returns The world of the game. A new one replaces it when the game waits
   * for players again, so do not keep the reference across ticks.
   */
  [[nodiscard]] game::World& world();

  /**
   * @brief Runs one tick: ends a game in progress that no player is left in, or
   * that no player left in has a life, then moves the game along its states.
   * While it waits, the players are told the room; while it is in progress, the
   * tick is counted, the ships move, the systems run, the ships whose players
   * hold shoot fire, the Bydos fire, the Bydos appear, the waits and
   * protections of the lives are counted and the world is sent; while it is
   * finished, the players are told the result.
   */
  void onTick();

  /**
   * @brief Ends the game in progress without telling its players a result:
   * only a game that ends by itself does (see finishIfOver). Does nothing in
   * any other state.
   */
  void finish() override;

 private:
  /** @brief Moves the game along its states by one tick: the start of the
   * game, its systems, its missiles and its Bydos, or the end of the wait after
   * it. */
  void advanceState();

  /** @brief Logs why and ends the game, as finish() ends it, when it is in
   * progress with no player left in it, or with no player left who has a
   * life, then tells the replication, which tells the players the result.
   */
  void finishIfOver();

  /** @brief Builds a new world, and every part of the game that works on one
   * world: the rules that judge its collisions, the spawner of its Bydos, the
   * launcher of its missiles, the gunner of its Bydos and the keeper of the
   * lives, after telling the replication about the new world. DamageRules is
   * built before LifeKeeper, so the bus hands each MissileHit to DamageRules
   * first (see LifeKeeper). */
  void buildWorld();

  void startNewWorld();

  GameContext& game_;
  WorldSlot& worldSlot_;
  IGameReplication& replication_;
  std::unique_ptr<BydoSpawner> bydoSpawner_;
  std::unique_ptr<WeaponSystem> weaponSystem_;
  std::unique_ptr<game::CollisionRules> collisionRules_;
  std::unique_ptr<game::DamageRules> damageRules_;
  std::unique_ptr<LifeKeeper> lifeKeeper_;
  GameState state_{GameState::WaitingForPlayers};
  std::uint32_t worldsBuilt_{0};
  std::uint32_t tick_{0};
  std::size_t finishedTicks_{0};
  logging::Logger logger_;
};

}  // namespace rtype::server
