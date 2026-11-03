#pragma once

#include <cstdint>
#include <optional>

namespace rtype::client {

/**
 * @brief Decides whether a received world state is still worth applying: one
 * older than the newest applied is dropped, because UDP may deliver an earlier
 * datagram after a later one.
 *
 * Ticks are compared as plain integers: they do not wrap within a game, a
 * 32-bit tick lasting 2.27 years at 60 ticks per second. Call accept() only for
 * datagrams that passed the sender checks.
 */
class WorldStateFilter {
 public:
  /**
   * @returns false when @p tick is lower than the newest tick accepted before:
   * the state is old. Otherwise true, and @p tick becomes the newest accepted;
   * an equal tick is accepted, as another part of the same state.
   */
  [[nodiscard]] bool accept(std::uint32_t tick);

 private:
  std::optional<std::uint32_t> lastAppliedTick_;
};

}  // namespace rtype::client
