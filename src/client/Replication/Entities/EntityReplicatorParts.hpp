#pragma once

#include <cstdint>
#include <functional>
#include "EntityFeedback.hpp"
#include "IEntityTypeSource.hpp"
#include "IOwnMissileListener.hpp"

namespace rtype::engine {

class ComponentRegistry;
class EntityRegistry;

}  // namespace rtype::engine

namespace rtype::client {

/**
 * @brief What an EntityReplicator is built from. Every reference must outlive
 * the replicator.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct EntityReplicatorParts {
  /** @brief The registry the entities are created in. */
  std::reference_wrapper<engine::EntityRegistry> entities;
  /** @brief The components of those entities. */
  std::reference_wrapper<engine::ComponentRegistry> components;
  /** @brief Plays the effects in the same registries, and the sounds. */
  EntityFeedback feedback;
  /** @brief What the entity type number of a message means. */
  std::reference_wrapper<const IEntityTypeSource> types;
  /** @brief The number of the player of this client, whose missiles the
   * listener may draw instead of the replicator. */
  std::uint8_t ownPlayerNumber{};
  /** @brief Who draws the missiles of the player of this client; nullptr to
   * draw them as any other entity. */
  IOwnMissileListener* ownMissiles{nullptr};
};

}  // namespace rtype::client
