#pragma once

#include <cstdint>
#include <vector>
#include "EntityDestroyed.hpp"
#include "EntitySpawned.hpp"
#include "IOwnMissileListener.hpp"

namespace rtype::client {

/**
 * @brief The missiles of the player of this client that its prediction draws:
 * their network ids, so the replicator draws none of them, and the listener
 * it hands their messages to. At most CLAIMED_MISSILE_COUNT are kept, the
 * oldest dropped first: a missile whose EntityDestroyed never came is
 * forgotten instead of kept for the rest of the game.
 */
class ClaimedMissiles {
 public:
  /** @param listener nullptr claims nothing. It must outlive this object. */
  ClaimedMissiles(std::uint8_t ownPlayerNumber, IOwnMissileListener* listener);

  /**
   * @brief Claims @p spawned when it is a missile of the player of this
   * client and the listener predicts them, and hands it to the listener.
   * @returns Whether it was claimed: the replicator then draws nothing of it.
   */
  [[nodiscard]] bool claim(const network::EntitySpawned& spawned);

  /** @returns Whether the entity @p entityId is a claimed missile. */
  [[nodiscard]] bool holds(std::uint32_t entityId) const;

  /**
   * @brief Hands @p destroyed to the listener and forgets its missile, when
   * it names a claimed one.
   * @returns Whether it named a claimed missile.
   */
  [[nodiscard]] bool release(const network::EntityDestroyed& destroyed);

 private:
  std::uint8_t ownPlayerNumber_;
  IOwnMissileListener* listener_;
  std::vector<std::uint32_t> entityIds_;
};

}  // namespace rtype::client
