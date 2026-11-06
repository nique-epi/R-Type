#pragma once

#include <cstddef>
#include "FarewellPause.hpp"
#include "IDatagramSender.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

/**
 * @brief Tells every player of the game that the server shuts down, with the
 * Disconnection the protocol sends the clients in that case:
 * FAREWELL_COPIES copies, FAREWELL_COPY_INTERVAL apart, each under
 * the next session header of its player.
 *
 * It runs on the thread that stops the server, after the loop stopped
 * running its stages, so nothing else touches the game; the tick thread still
 * flushes what is queued to the sender at the end of every tick, which is why
 * it waits one last time, FAREWELL_FLUSH_WAIT, after the last copy.
 */
class FarewellSender {
 public:
  /** @param sender, pause Both must outlive the object. */
  FarewellSender(IDatagramSender& sender, FarewellPause pause);

  /** @brief Sends the copies to every player of @p roster; sends nothing and
   * waits for nothing when it holds no player. */
  void sayGoodbyeTo(PlayerRoster& roster);

 private:
  /** @returns How many players the copy went to. */
  [[nodiscard]] std::size_t queueOneCopy(PlayerRoster& roster);

  IDatagramSender* sender_;
  FarewellPause pause_;
};

}  // namespace rtype::server
