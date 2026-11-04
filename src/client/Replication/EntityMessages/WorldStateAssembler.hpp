#pragma once

#include <deque>
#include <optional>
#include <string>
#include "DeltaWorldState.hpp"
#include "EntityMessageConstants.hpp"
#include "Logger.hpp"
#include "PendingDelta.hpp"
#include "WorldState.hpp"

namespace rtype::client {

/**
 * @brief Rebuilds the full WorldState of each tick from the DeltaWorldState
 * parts of the server, so the rest of the client reads every state in full.
 *
 * It holds the parts of the tick it is receiving, and the states it rebuilt
 * (REBUILT_STATE_HISTORY_SIZE), which the next deltas are written against. A
 * state is complete once every one of its parts arrived: the part of a newer
 * tick ends the wait for the parts missing from an older one, which the session
 * refuses once a newer datagram was accepted, so they never come. A tick lower
 * than the newest rebuilt, or than the one being received, is a game that
 * started over: everything held is forgotten first.
 *
 * The client acknowledges a datagram when it accepts it, so the server takes a
 * state as held once every one of its parts was accepted: this class rebuilds
 * a state the moment its last part arrives, and keeps every state it rebuilt
 * for the longest reach of a delta, so a reference the server uses is held.
 * A state whose reference is not held is dropped, with a warning.
 *
 * Not thread-safe: use it from the thread that handles the messages.
 */
class WorldStateAssembler {
 public:
  /**
   * @brief Takes one part of a state.
   * @returns The full state when @p part was the last missing of its tick and
   * the state it is written against is held; nothing otherwise.
   */
  [[nodiscard]] std::optional<network::WorldState> add(
      const network::DeltaWorldState& part);

  /** @brief Forgets the parts and the states held. */
  void clear();

 private:
  [[nodiscard]] bool goesBack(const network::DeltaWorldState& part) const;
  [[nodiscard]] bool fillsASlot(const network::DeltaWorldState& part) const;
  [[nodiscard]] std::optional<network::WorldState> rebuild();
  [[nodiscard]] const network::WorldState* referenceOf(
      const PendingDelta& pending) const;
  void remember(const network::WorldState& state);

  std::optional<PendingDelta> pending_;
  std::deque<network::WorldState> history_;
  logging::Logger logger_{std::string{ASSEMBLER_LOGGER_NAME}};
};

}  // namespace rtype::client
