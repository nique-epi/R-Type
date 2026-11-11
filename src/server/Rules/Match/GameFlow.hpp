#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include "GameContext.hpp"
#include "GameState.hpp"
#include "IGameFlow.hpp"
#include "IGameReplication.hpp"
#include "IHitArbitration.hpp"
#include "Logger.hpp"
#include "MatchSystems.hpp"
#include "StartBarrier.hpp"
#include "World.hpp"
#include "WorldSlot.hpp"

namespace rtype::server {

/**
 * @brief How a game runs: its state, its tick and its world.
 *
 * A game goes from WaitingForPlayers to Starting when at least one player is
 * connected and every connected player is ready, from Starting to InProgress
 * once every connected player told it the game screen is shown or
 * START_BARRIER_TICKS ticks went by (see StartBarrier), from InProgress to
 * Finished
 * when finish() is called, once no player is left in it, once no player left
 * in it has a life (the game is lost), or once the level is cleared (the game
 * is won): its last wave over, and the boss its difficulty names, if any, gone
 * with every other enemy (see BossStage). A game that plays a level a script
 * directs (GameSettings::level) runs no wave: it is won once that level is
 * cleared, lost once its script failed, and a team that lost every life past
 * a checkpoint of the level goes back to it with the lives of the rules
 * instead of losing (see LevelDirector). From Finished it goes back to
 * WaitingForPlayers after FINISHED_STATE_TICKS ticks. Coming back to
 * WaitingForPlayers builds a new world: every player gets a new ship and the
 * lives of the rules back, is no longer ready, and the tick is 0. The players
 * stay connected.
 *
 * The flow tells the players where the game stands, as the A session section
 * of the protocol page describes, through the IGameReplication it is given.
 * While the game waits for players, the replication sends them the room; it
 * builds a new announcer of the room with each new world, so the room goes out
 * at the first tick of every wait. The tick that puts the game in Starting
 * sends every connected player GameStarted, which leaves again to the players
 * not loaded while the game is Starting; on the tick that puts the game in
 * progress and every tick of the game, the replication sends them the world,
 * from tick 0: GameStarted always leaves before the first WorldState. A game
 * that ends by itself tells the replication how it ended, which reads the
 * score of every player then and sends them that result at the first tick of
 * the end, then
 * every GAME_ENDED_INTERVAL until the game waits for players again; no
 * WorldState goes out meanwhile.
 *
 * While the game is in progress, each tick first gives the ViewDelay of the
 * hit arbitration of the game, FavorThePlayers for now: the missiles and the
 * ships of the players are tested where their players saw the enemies (see
 * LagCompensatedHitSystem). It then makes the ship of every away
 * player want nothing (see silenceAwayShips), while the Intent of the ship of
 * a player is its last input (see steerShipByInput), then runs the systems of
 * the world (PilotSystem, which lets the behaviour of every Bydo write its
 * Intent, MovementSystem, PlayfieldExitSystem,
 * which removes a Bydo that left on the left, or a missile wholly past an
 * edge it flies towards, and announces it on the event bus, then
 * CollisionSystem), then fires the Gun of every ship and Bydo whose
 * Intent wants to shoot, each at the interval of the gun its sheet gives, and
 * bursts every projectile whose time came into its shards (see WeaponSystem),
 * then fires the volleys of every enemy whose Intent starts an action its
 * sheet fires one on (see VolleySystem), then makes appear and removes the
 * bodies the scripts asked for (see EntityOrders),
 * then lets the Bydos of the current wave appear (see WaveSpawner), then counts
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
 * into the loss of the ship, the Bydo (unless it carries SurvivesContact) and a
 * life, HurtZoneSystem publishing the same ShipContact for a ship that touches
 * an active box of the HurtZone of a Bydo, and a missile of a Bydo that
 * touches a ship into a MissileHit, which DamageRules turns into the removal of
 * the missile and LifeKeeper into the loss of the ship and a life: the ship
 * comes back RESPAWN_DELAY later, invulnerable for INVULNERABILITY_DURATION,
 * and a player with no life left watches. A life is taken while the bus is
 * dispatched, after onTick(), so a game whose last life was taken ends at the
 * next onTick(), before the game moves along its states. The rules work on the
 * world they were built with, and new ones come with each new world, so the
 * events of a tick must be delivered before the world is replaced: the server
 * dispatches the bus at the end of every tick. Each move from one state to
 * another is published on that bus as a GameStateChanged, once per move, so a
 * reader learns of it in the same tick without asking state() at every tick.
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
   * @brief Runs one tick: ends a game in progress that no player is left in,
   * that no player left in has a life, or whose last wave is over, then moves
   * the game along its states.
   * While it waits, the players are told the room; while it is in progress, the
   * tick is counted, the systems run (they move the ships and the Bydos by
   * their Intent), every Gun whose Intent wants to shoot fires, the Bydos
   * appear, the waits and protections of the lives are counted and the world
   * is sent; while it is
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
   * progress with no player left in it, with no player left who has a life, or
   * with its level cleared or failed, then tells the replication how it
   * ended, which tells the players the result. A team without a life whose
   * level reached a checkpoint restarts from it instead.
   */
  void finishIfOver();

  /** @brief Builds a new world, and every part of the game that works on one
   * world: the rules that judge its collisions, the spawner of its Bydos, the
   * weapon system of its missiles and the keeper of the lives, after telling
   * the replication about the new world. DamageRules is built before
   * LifeKeeper, so the bus hands each MissileHit to DamageRules first (see
   * LifeKeeper). */
  void buildWorld();

  void startNewWorld();

  GameContext& game_;
  WorldSlot& worldSlot_;
  IGameReplication& replication_;
  std::unique_ptr<MatchSystems> systems_;
  StartBarrier startBarrier_;
  std::unique_ptr<IHitArbitration> hitArbitration_;
  GameState state_{GameState::WaitingForPlayers};
  std::uint32_t worldsBuilt_{0};
  std::uint32_t tick_{0};
  std::size_t finishedTicks_{0};
  logging::Logger logger_;
};

}  // namespace rtype::server
