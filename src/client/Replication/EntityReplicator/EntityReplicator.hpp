#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <vector>
#include "Entity.hpp"
#include "EntityFeedback.hpp"
#include "EntityType.hpp"
#include "LastListedTicks.hpp"
#include "NetworkEntityTable.hpp"
#include "Position.hpp"
#include "WorldStateFilter.hpp"

namespace rtype::engine {

class ComponentRegistry;
class EntityRegistry;

}  // namespace rtype::engine

namespace rtype::network {

struct EntityDestroyed;
struct EntitySpawned;
struct EntityState;
struct GameEvent;
struct PlayerState;
struct WorldState;

}  // namespace rtype::network

namespace rtype::client {

struct AnimatedSprite;

/**
 * @brief Applies the entities the server announces to the registries of the
 * client: creates an entity from its type, moves it, destroys it, so that
 * rendering only draws what the registries hold.
 *
 * A created entity gets a Position, in logical units as the message gives it,
 * its EntityType, and the Sprite and SpriteAnimation animatedSpriteOf() gives
 * its type and its network id: what RenderSystem and AnimationSystem read, the
 * sprite already showing the frame the animation starts on. No rule of the
 * game runs here: where an entity is and when it disappears come from the
 * server only.
 *
 * It tells the effects what the protocol page ties to these messages: the
 * arrival of every ship it creates and the muzzle flash of every missile; the
 * explosion of an entity an EntityDestroyed says was Killed, the whole of T4
 * for a ship, centered where the entity stood; nothing for one Removed; the
 * hit of the entity an EntityDamaged names. It plays the sound of each, from
 * SoundConstants.hpp: the shot of a missile of a player or of an enemy, the
 * small explosion of a Bydo killed and the big one of a ship, the hit of the
 * entity an EntityDamaged names, and the hit of a player a PlayerHit names.
 * Each plays once per entity: a later copy of an EntityDestroyed names an
 * entity the client no longer has, a later copy of an EntitySpawned names one
 * it has, and the connection hands over the first copy of a GameEvent only.
 *
 * It follows the client rules of the protocol page. A WorldState older than
 * the newest one applied is ignored (see WorldStateFilter); an equal tick is
 * another part of the same state and is applied. An entity a WorldState lists
 * and the client does not have is created. An EntitySpawned naming an entity
 * the client has, and an EntityDestroyed naming one it does not have, change
 * nothing; so does a message naming NO_ENTITY_ID or a type outside
 * WireEntityType. An entity no WorldState listed for UNLISTED_ENTITY_TICKS up
 * to a closed tick is removed, without an effect, once a part of a newer state
 * closes that tick (see LastListedTicks): the server destroyed it and every
 * copy of its EntityDestroyed was lost.
 *
 * Of the players of a WorldState, it marks the ship of each with the color
 * of its pilot (see PilotShip) and whether it can be hit, then gives the
 * effects its score, from which they show the figure of a gain; a player
 * number outside FIRST_PLAYER_NUMBER to MAXIMUM_PLAYERS, or a ship id that
 * names no ship, is ignored. The players are read after the entities and
 * before the appearances of the entities the state created, so a ship
 * created by a world state arrives in the color of its pilot.
 *
 * One replicator serves one game: the ticks start again with each game, and a
 * replicator kept from the previous one would ignore every state until the new
 * ticks pass the old ones. Entities it created must be destroyed through it,
 * or their id stays bound to a dead entity that no message moves again. Call
 * it from the frame loop only, outside any forEach of the registry, and only
 * with messages whose sender passed the checks: it is not thread-safe, every
 * call may add components, and a forged WorldState would raise the newest
 * tick applied.
 */
class EntityReplicator {
 public:
  /**
   * @param entities Registry the entities are created in; must outlive this.
   * @param components Components of the entities; must outlive this.
   * @param feedback Plays the effects in the same registries, and the
   * sounds; both must outlive this.
   */
  EntityReplicator(engine::EntityRegistry& entities,
                   engine::ComponentRegistry& components,
                   EntityFeedback feedback);

  /**
   * @brief Creates the entity the message announces, unless the client
   * already has it, then plays its appearance.
   */
  void apply(const network::EntitySpawned& message);

  /**
   * @brief Moves every entity the state lists to its position, and creates
   * those the client does not have, then removes those no state listed for
   * UNLISTED_ENTITY_TICKS up to the tick it closes, then reads its players,
   * then plays the appearance of the entities created; nothing for a state
   * older than the newest one applied.
   */
  void apply(const network::WorldState& state);

  /**
   * @brief Destroys the entity the message names, if the client has it, after
   * playing its explosion when the message says it was killed.
   */
  void apply(const network::EntityDestroyed& message);

  /**
   * @brief Plays the hit of the entity an EntityDamaged names, if the client
   * has it, with its sound; plays only the sound of a hit player for a
   * PlayerHit; nothing for a PlayerEliminated.
   */
  void apply(const network::GameEvent& event);

 private:
  /**
   * @returns The entity created for an entity announced, with the sprite and
   * the animation of its type, its id bound; nothing for the id NO_ENTITY_ID
   * or a type outside WireEntityType, every check running before anything is
   * created. Plays no effect.
   */
  std::optional<engine::Entity> create(const network::EntityState& announced);

  /**
   * @returns A new entity at @p position, with its type and the sprite and
   * animation of @p look: the components rendering and animation read. Its id
   * is bound by the caller.
   */
  engine::Entity addEntity(game::EntityType type, game::Position position,
                           AnimatedSprite look);

  /** @brief Moves the entities a state of @p tick lists, and creates those the
   * client does not have; records that the state listed each of them.
   * @returns The entities created, for their appearance. */
  std::vector<engine::Entity> applyEntities(
      std::span<const network::EntityState> entities, std::uint32_t tick);

  /** @brief Marks the ship of each player and notes its score; see the
   * class. */
  void applyPlayers(std::span<const network::PlayerState> players);

  /** @brief Marks the ship of a player with its pilot color and whether it
   * can be hit, when the client has that ship. */
  void markShip(const network::PlayerState& player, int pilotColor);

  /**
   * @brief Removes, without an effect, every entity unlisted for
   * UNLISTED_ENTITY_TICKS up to the tick @p tick closes: nothing for another
   * part of the newest tick.
   */
  void removeUnlistedAt(std::uint32_t tick);

  engine::EntityRegistry* entities_{};
  engine::ComponentRegistry* components_{};
  EntityFeedback feedback_;
  engine::NetworkEntityTable entityByNetworkId_;
  WorldStateFilter worldStateFilter_;
  LastListedTicks lastListedTicks_;
};

}  // namespace rtype::client
