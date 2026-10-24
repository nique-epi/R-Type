#pragma once

#include <cstdint>
#include <optional>
#include <unordered_map>
#include <vector>

namespace rtype::client {

/**
 * @brief Remembers, for each entity the client has, the tick of the newest
 * WorldState that listed it, and names the entities no complete state listed
 * for UNLISTED_ENTITY_TICKS: the server destroyed them and every copy of their
 * EntityDestroyed was lost, so they would stay on screen as ghosts.
 *
 * A state too big for one datagram comes in several parts with the same tick,
 * and a later part may list an entity an earlier one left out. The client only
 * knows the state of a tick complete once a part of a newer tick arrives, so
 * an entity is judged on that complete state, never on a part.
 *
 * It counts the ticks of the states it is given, never the time that passes:
 * while no state arrives, no entity ages, so a stall of the server or a burst
 * of losses removes nothing. Ticks are compared as plain integers, as
 * WorldStateFilter compares them: they do not wrap within a game. It holds one
 * entry per entity it was told about, until that entity is forgotten or taken.
 */
class LastListedTicks {
 public:
  /**
   * @brief Records that a WorldState of @p tick lists the entity. Call it only
   * for a state WorldStateFilter accepted, so the tick recorded never goes
   * back.
   */
  void listed(std::uint32_t networkId, std::uint32_t tick);

  /**
   * @brief Starts counting an entity no state listed, as one an EntitySpawned
   * created: from the tick of the last state given to takeUnlistedAt(), or
   * from the next one when none was given yet.
   */
  void appeared(std::uint32_t networkId);

  /**
   * @brief Stops counting the entity: the client destroyed it.
   */
  void forget(std::uint32_t networkId);

  /**
   * @brief Call it once for each part of a state WorldStateFilter accepted,
   * after listed() for each of its entities. An entity that appeared before
   * any state starts counting at @p tick.
   * @returns When @p tick is newer than the tick given before, which makes the
   * state of that previous tick complete: the entities that state finds
   * UNLISTED_ENTITY_TICKS or more after their newest listing, in no particular
   * order; they are forgotten. Nothing for the first state, nor for another
   * part of the newest tick.
   */
  [[nodiscard]] std::vector<std::uint32_t> takeUnlistedAt(std::uint32_t tick);

 private:
  /**
   * @returns The tick whose state @p tick completes: the newest tick given
   * before, when @p tick is newer than it; nothing otherwise.
   */
  [[nodiscard]] std::optional<std::uint32_t> completedBy(
      std::uint32_t tick) const;

  /**
   * @brief Gives @p tick to every entity that appeared before any state.
   */
  void startWaitingEntitiesAt(std::uint32_t tick);

  /**
   * @returns The entities whose newest listing comes UNLISTED_ENTITY_TICKS or
   * more before @p tick.
   */
  [[nodiscard]] std::vector<std::uint32_t> unlistedAt(std::uint32_t tick) const;

  std::unordered_map<std::uint32_t, std::optional<std::uint32_t>>
      lastListedTickByNetworkId_;
  std::optional<std::uint32_t> newestTick_;
};

}  // namespace rtype::client
