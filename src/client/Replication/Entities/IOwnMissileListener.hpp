#pragma once

#include "EntityDestroyed.hpp"
#include "EntitySpawned.hpp"

namespace rtype::client {

/**
 * @brief Who draws the missiles of the player of this client when the client
 * fires them itself: the replicator hands it their EntitySpawned and their
 * EntityDestroyed instead of drawing them.
 */
class IOwnMissileListener {
 public:
  virtual ~IOwnMissileListener() = default;

  /** @returns Whether the client fires the missiles of its player itself
   * now, so the replicator must not draw them. */
  [[nodiscard]] virtual bool predictsOwnMissiles() const = 0;

  /** @brief The server made @p missile, a missile of the player of this
   * client. */
  virtual void ownMissileSpawned(const network::EntitySpawned& missile) = 0;

  /** @brief The server removed or destroyed a missile of the player of this
   * client that ownMissileSpawned() was told of. */
  virtual void ownMissileDestroyed(
      const network::EntityDestroyed& destroyed) = 0;
};

}  // namespace rtype::client
