#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "EntitySheet.hpp"
#include "NetworkEntityTable.hpp"
#include "NetworkIdAllocator.hpp"
#include "Position.hpp"
#include "ShipMotion.hpp"

namespace rtype::game {

/**
 * @brief Game state shared by client and server. It knows neither sockets nor
 * the network protocol.
 *
 * Every entity is made from the sheet of its type by spawn(). The ship of each
 * player, a body no behaviour pilots, is listed under its network id, so
 * shipPositionOf(), shipMotionOf() and removePlayer() find it without visiting
 * the other entities; removePlayer() takes it out of that list. The sheets
 * belong to an EntitySheets that outlives the world: a Pilot keeps a reference
 * to the behaviour and the settings of its sheet.
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
   * @brief Creates, at @p position, an entity of the type @p sheet describes:
   * its Position, its EntityType (the side of the sheet), its CollisionBox (the
   * hitbox), its EntityTypeNumber, an Intent that wants nothing until a pilot
   * writes it and the NetworkIdentity that makes it a networked entity; a
   * ShipMotion or a SteadyMotion of its top speed, as its motion says; a Pilot
   * when a behaviour pilots it, steering it from @p position, at age zero,
   * with the settings of the sheet; its Health, its ScoreValue and its Gun
   * when the sheet gives them, the gun waiting the first shot of the sheet.
   * A body no behaviour pilots is the ship of a player: it also gets
   * STARTING_LIVES Lives and no protection in its Invulnerable, and is listed
   * as a ship. With @p owner, the entity gets a MissileOwner naming that
   * player, so a kill is credited to it.
   * @returns The network id the entity was given, never NO_ENTITY_ID.
   * @throws NetworkIdExhaustedException when every network id was given;
   * nothing is created then.
   */
  std::uint32_t spawn(const EntitySheet& sheet, Position position,
                      std::optional<std::uint8_t> owner = std::nullopt);

  /**
   * @brief Destroys the ship of a player, found by the network id spawn()
   * gave it.
   * @returns Whether a ship carried @p shipNetworkId. Nothing changes when
   * none did, an entity of another type carrying that id included.
   */
  bool removePlayer(std::uint32_t shipNetworkId);

  [[nodiscard]] std::size_t entityCount() const;

  /**
   * @returns The components of the entities of this world.
   */
  engine::ComponentRegistry& components();

  /**
   * @returns The Position of the ship spawn() gave @p shipId, or nullptr for
   * any other id: the id of an enemy or a missile, an id this world never
   * gave, NO_ENTITY_ID. The pointer is invalidated by the next add or remove of
   * a Position, like ComponentRegistry::get.
   */
  [[nodiscard]] Position* shipPositionOf(std::uint32_t shipId);

  /**
   * @returns The ShipMotion of the ship spawn() gave @p shipId, or nullptr for
   * any other id, as shipPositionOf() does. The pointer is invalidated by the
   * next add or remove of a ShipMotion, like ComponentRegistry::get.
   */
  [[nodiscard]] ShipMotion* shipMotionOf(std::uint32_t shipId);

  /**
   * @returns The component of type T of the ship spawn() gave @p shipId
   * (Intent, Lives, Invulnerable, Gun…), or nullptr for any other id or a ship
   * without that component, as shipPositionOf() does. The pointer is
   * invalidated by the next add or remove of a T, like ComponentRegistry::get.
   */
  template <typename T>
  [[nodiscard]] T* shipComponentOf(std::uint32_t shipId) {
    const std::optional<engine::Entity> ship = shipByNetworkId_.find(shipId);
    return ship.has_value() ? components_.get<T>(*ship) : nullptr;
  }

 private:
  engine::EntityRegistry registry_;
  engine::ComponentRegistry components_{registry_};
  engine::NetworkIdAllocator networkIds_;
  engine::NetworkEntityTable shipByNetworkId_;
};

}  // namespace rtype::game
