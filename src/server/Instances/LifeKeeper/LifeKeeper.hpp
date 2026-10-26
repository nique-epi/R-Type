#pragma once

#include <functional>
#include "LifeKeeperParts.hpp"
#include "Logger.hpp"
#include "SubscriptionHandle.hpp"

namespace rtype::engine {

class EventBus;

}  // namespace rtype::engine

namespace rtype::game {

class World;
struct MissileHit;
struct ShipContact;

}  // namespace rtype::game

namespace rtype::server {

class PlayerRoster;
struct ConnectedPlayer;

/**
 * @brief Keeps the lives of the players of one world: takes one each time the
 * ship of a player touches an enemy or is hit by a missile of an enemy, brings
 * the ship back RESPAWN_DELAY later, out of reach for INVULNERABILITY_DURATION,
 * and leaves a player with no life left without a ship, watching.
 *
 * Subscribes to ShipContact and MissileHit on construction and unsubscribes on
 * destruction. A contact costs a life only when the ship and the enemy are both
 * still in the world, and the ship is the one of a connected player who has a
 * life and is not invulnerable. Then the ship and the enemy are destroyed, the
 * player loses a life and its ship, and the bus is told, in this order, an
 * EntityDestruction (Killed) for the ship, one for the enemy, a PlayerHit, and
 * a PlayerElimination when no life is left. Nobody earns the points of the
 * enemy. Any other contact changes nothing: a contact naming an entity already
 * gone is ignored, so two enemies touching one ship in the same dispatch cost
 * one life, and an enemy touching two ships destroys the one the bus delivers
 * first; an invulnerable ship and the enemy it touches are both left alone.
 *
 * A hit costs a life when its target is, in the same way, the ship of a
 * connected player who has a life and is not invulnerable. Then the ship is
 * destroyed, the player loses a life and its ship, and the bus is told an
 * EntityDestruction (Killed) for the ship, a PlayerHit, and a PlayerElimination
 * when no life is left. The missile is not the keeper's to destroy: DamageRules
 * removes it, on an invulnerable ship too. DamageRules ignores a hit whose
 * target is already gone, so it must subscribe before the keeper, and the bus
 * hands it each hit first. A hit on anything else, a Bydo hit by a missile of a
 * player, changes nothing here.
 *
 * A player left with a life waits RESPAWN_DELAY_TICKS ticks of onTick() without
 * a ship, then gets a new one through the respawn function of its parts, and is
 * invulnerable for INVULNERABILITY_TICKS ticks, the tick it comes back
 * included. When no network id is left for the new ship, a warning is logged
 * and the player stays without a ship in that world.
 *
 * A keeper works on one world: the game builds a new one with each new world,
 * and gives every player its STARTING_LIVES back. The bus, the world and the
 * roster must outlive the keeper.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class LifeKeeper {
 public:
  explicit LifeKeeper(LifeKeeperParts parts);
  ~LifeKeeper();

  LifeKeeper(const LifeKeeper&) = delete;
  LifeKeeper& operator=(const LifeKeeper&) = delete;
  LifeKeeper(LifeKeeper&&) = delete;
  LifeKeeper& operator=(LifeKeeper&&) = delete;

  /**
   * @brief Runs one tick of the game for every connected player: one tick less
   * of invulnerability for a player who has some, then one tick less of the
   * wait of a player without a ship, who gets a new one when its wait ends.
   */
  void onTick();

 private:
  void onShipContact(const game::ShipContact& contact);
  void onMissileHit(const game::MissileHit& hit);

  /** @brief Counts one tick of the wait of @p player down, and gives it a new
   * ship when the wait ends. */
  void countWaitDown(ConnectedPlayer& player);

  /** @brief Gives @p player a new ship, invulnerable for
   * INVULNERABILITY_TICKS ticks, or logs that no network id is left for it. */
  void respawn(ConnectedPlayer& player);

  engine::EventBus& events_;
  game::World& world_;
  PlayerRoster& roster_;
  std::function<void(ConnectedPlayer&)> respawn_;
  engine::SubscriptionHandle contactSubscription_;
  engine::SubscriptionHandle hitSubscription_;
  logging::Logger logger_;
};

}  // namespace rtype::server
