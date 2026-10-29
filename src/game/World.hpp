#pragma once

#include <cstddef>
#include <cstdint>
#include "BydoKind.hpp"
#include "ComponentRegistry.hpp"
#include "EntityRegistry.hpp"
#include "Intent.hpp"
#include "NetworkEntityTable.hpp"
#include "NetworkIdAllocator.hpp"
#include "Position.hpp"
#include "ShipMotion.hpp"

namespace rtype::game {

/**
 * @brief Game state shared by client and server. It knows neither sockets nor
 * the network protocol.
 *
 * The ship of each player is listed under its network id, so shipPositionOf(),
 * shipMotionOf() and removePlayer() find it without visiting the other
 * entities; removePlayer() takes it out of that list.
 */
class World {
 public:
  World() = default;
  World(const World&) = delete;
  World& operator=(const World&) = delete;
  World(World&&) = delete;
  World& operator=(World&&) = delete;
  ~World() = default;

  /**
   * @brief Creates the ship of a player at @p position, with the Position, the
   * EntityType::Player and the NetworkIdentity that make it a networked
   * entity, a CollisionBox of SHIP_WIDTH by SHIP_HEIGHT, an Intent that wants
   * nothing until a controller writes it, and a Gun that fires a missile of a
   * player every SHOT_INTERVAL_TICKS ticks and is ready at once: MovementSystem
   * moves the ship by its Intent, and the server fires its Gun by it.
   * @returns The network id the ship was given, never NO_ENTITY_ID.
   * @throws NetworkIdExhaustedException when every network id was given.
   */
  std::uint32_t spawnPlayer(Position position);

  /**
   * @brief Destroys the ship of a player, found by the network id
   * spawnPlayer() gave it.
   * @returns Whether a ship carried @p shipNetworkId. Nothing changes when
   * none did, an entity of another type carrying that id included.
   */
  bool removePlayer(std::uint32_t shipNetworkId);

  /**
   * @brief Creates a Bydo of @p kind at @p position, with the Position, the
   * EntityType::Enemy and the NetworkIdentity that make it a networked entity,
   * a CollisionBox of BYDO_WIDTH by BYDO_HEIGHT, an Intent that wants nothing
   * until a controller writes it, and a Gun that fires a missile of an enemy
   * every ENEMY_SHOT_INTERVAL_TICKS ticks, the first one after a whole
   * interval. It moves left at BYDO_SPEED, and follows the pattern of its kind
   * around the row it appeared on: MovementSystem and PatternSystem move it.
   * @returns The network id the Bydo was given, never NO_ENTITY_ID.
   * @throws NetworkIdExhaustedException when every network id was given.
   */
  std::uint32_t spawnBydo(Position position, BydoKind kind);

  /**
   * @brief Creates, at @p position, a missile fired by the player numbered
   * @p playerNumber, with the Position, the EntityType::PlayerMissile and the
   * NetworkIdentity that make it a networked entity, a MissileOwner naming
   * that player, so a kill is credited to it, and a CollisionBox of
   * PLAYER_MISSILE_WIDTH by PLAYER_MISSILE_HEIGHT. It flies right at
   * PLAYER_MISSILE_SPEED: MovementSystem moves it.
   * @returns The network id the missile was given, never NO_ENTITY_ID.
   * @throws NetworkIdExhaustedException when every network id was given.
   */
  std::uint32_t spawnPlayerMissile(Position position,
                                   std::uint8_t playerNumber);

  /**
   * @brief Creates, at @p position, a missile fired by an enemy, with the
   * Position, the EntityType::EnemyMissile and the NetworkIdentity that make it
   * a networked entity, and a CollisionBox of ENEMY_MISSILE_WIDTH by
   * ENEMY_MISSILE_HEIGHT. It has no MissileOwner, since no player fired it. It
   * flies left at ENEMY_MISSILE_SPEED: MovementSystem moves it.
   * @returns The network id the missile was given, never NO_ENTITY_ID.
   * @throws NetworkIdExhaustedException when every network id was given.
   */
  std::uint32_t spawnEnemyMissile(Position position);
  [[nodiscard]] std::size_t entityCount() const;

  /**
   * @returns The components of the entities of this world.
   */
  engine::ComponentRegistry& components();

  /**
   * @returns The Position of the ship spawnPlayer gave @p shipId, or nullptr
   * for any other id: the id of a Bydo, an id this world never gave,
   * NO_ENTITY_ID. The pointer is invalidated by the next add or remove of a
   * Position, like ComponentRegistry::get.
   */
  [[nodiscard]] Position* shipPositionOf(std::uint32_t shipId);

  /**
   * @returns The ShipMotion of the ship spawnPlayer gave @p shipId, or nullptr
   * for any other id, as shipPositionOf() does. The pointer is invalidated by
   * the next add or remove of a ShipMotion, like ComponentRegistry::get.
   */
  [[nodiscard]] ShipMotion* shipMotionOf(std::uint32_t shipId);

  /**
   * @returns The Intent of the ship spawnPlayer gave @p shipId, or nullptr for
   * any other id, as shipPositionOf() does. The pointer is invalidated by the
   * next add or remove of an Intent, like ComponentRegistry::get.
   */
  [[nodiscard]] Intent* shipIntentOf(std::uint32_t shipId);

 private:
  engine::EntityRegistry registry_;
  engine::ComponentRegistry components_{registry_};
  engine::NetworkIdAllocator networkIds_;
  engine::NetworkEntityTable shipByNetworkId_;
};

}  // namespace rtype::game
