#pragma once

#include <cstdint>
#include <deque>
#include <optional>
#include "PacketHeader.hpp"
#include "SentState.hpp"

namespace rtype::server {

/**
 * @brief The world states sent to one player, and which of them the player is
 * known to have in full: the reference of the next delta.
 *
 * A state is acknowledged once the datagram of every one of its parts is: the
 * client acknowledges every datagram it accepts, and accepts them in order, so
 * a part it acknowledged has been read. A state the player acknowledged is the
 * newest one it holds in full, never an older one.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class SentStates {
 public:
  /**
   * @brief Keeps @p state, the newest sent. A state that is not newer than the
   * newest kept means the game started over: everything kept, and the
   * acknowledged state, is forgotten first. The oldest is dropped beyond
   * SENT_STATE_HISTORY_SIZE states.
   */
  void remember(SentState state);

  /**
   * @brief Takes the acknowledgement of @p header, from a datagram of the
   * player that passed every check, and keeps as acknowledged the newest state
   * whose parts it acknowledges all, when it is newer than the one kept. A
   * state with no part is never acknowledged.
   */
  void acknowledge(const network::PacketHeader& header);

  /**
   * @returns The acknowledged state a state of @p tick may be written against:
   * older than @p tick by 1 to MAXIMUM_REFERENCE_DISTANCE ticks and still kept;
   * nullptr otherwise. The pointer holds until the next remember().
   */
  [[nodiscard]] const SentState* referenceFor(std::uint32_t tick) const;

 private:
  std::deque<SentState> history_;
  std::optional<std::uint32_t> acknowledgedTick_;
};

}  // namespace rtype::server
