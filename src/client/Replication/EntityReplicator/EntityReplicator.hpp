#pragma once

#include "Entity.hpp"
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
struct WorldState;

}  // namespace rtype::network

namespace rtype::client {

class EffectSpawner;

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
 * It plays the effects the protocol page ties to these messages: the
 * appearance of every entity it creates, the muzzle flash of a missile; the
 * explosion of an entity an EntityDestroyed says was Killed, centered where
 * the entity stood; nothing for one Removed. Each plays once per entity: a
 * later copy of an EntityDestroyed names an entity the client no longer has,
 * and a later copy of an EntitySpawned names one it has.
 *
 * It follows the client rules of the protocol page. A WorldState older than
 * the newest one applied is ignored (see WorldStateFilter); an equal tick is
 * another part of the same state and is applied. An entity a WorldState lists
 * and the client does not have is created. An EntitySpawned naming an entity
 * the client has, and an EntityDestroyed naming one it does not have, change
 * nothing; so does a message naming NO_ENTITY_ID or a type outside
 * WireEntityType. The players of a WorldState are not read. An entity no
 * WorldState lists any more stays until an EntityDestroyed names it: removing
 * it after a delay is not done here.
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
   * @param effects Plays the effects in the same registries; must outlive
   * this.
   */
  EntityReplicator(engine::EntityRegistry& entities,
                   engine::ComponentRegistry& components,
                   EffectSpawner& effects);

  /**
   * @brief Creates the entity the message announces, unless the client
   * already has it.
   */
  void apply(const network::EntitySpawned& message);

  /**
   * @brief Moves every entity the state lists to its position, and creates
   * those the client does not have, unless the state is older than the newest
   * one applied.
   */
  void apply(const network::WorldState& state);

  /**
   * @brief Destroys the entity the message names, if the client has it, after
   * playing its explosion when the message says it was killed.
   */
  void apply(const network::EntityDestroyed& message);

 private:
  /**
   * @brief Creates the entity, with the sprite and the animation of its type,
   * binds its id to it, then plays its appearance. Creates nothing for the id
   * NO_ENTITY_ID or a type outside WireEntityType: every check runs before
   * anything is created.
   */
  void create(const network::EntityState& announced);

  /**
   * @brief Puts the entity at the position; does nothing for an entity
   * without a Position.
   */
  void move(engine::Entity entity, game::Position position);

  /**
   * @brief Plays the explosion of the entity, centered where it stands; does
   * nothing for an entity without a Position or an EntityType.
   */
  void explode(engine::Entity entity);

  engine::EntityRegistry* entities_{};
  engine::ComponentRegistry* components_{};
  EffectSpawner* effects_{};
  engine::NetworkEntityTable entityByNetworkId_;
  WorldStateFilter worldStateFilter_;
};

}  // namespace rtype::client
