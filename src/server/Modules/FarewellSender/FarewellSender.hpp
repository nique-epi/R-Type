#pragma once

#include <cstddef>
#include <functional>
#include "Games.hpp"
#include "IDatagramSender.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief Tells every player of the games that the server shuts down, with the
 * Disconnection the protocol sends the clients in that case:
 * REPEATED_MESSAGE_COPIES copies, REPEATED_MESSAGE_INTERVAL apart, each under
 * the next session header of its player.
 *
 * It runs on the thread that stops the server, after the loop stopped
 * running its stages, so nothing else touches the games; the tick thread still
 * flushes what is queued to the sender at the end of every tick, which is why
 * it waits one last time, FAREWELL_FLUSH_WAIT, after the last copy.
 */
class FarewellSender {
 public:
  /** @brief Waits for the given time; the server sleeps, a test only writes the
   * time down. */
  using Pause = std::function<void(engine::Duration)>;

  /** @param sender, pause Both must outlive the object. */
  FarewellSender(IDatagramSender& sender, Pause pause);

  /** @brief Sends the copies to every player of @p games; sends nothing and
   * waits for nothing when no game holds a player. */
  void sayGoodbyeTo(const Games& games);

 private:
  /** @returns How many players the copy went to. */
  [[nodiscard]] std::size_t queueOneCopy(const Games& games);

  IDatagramSender* sender_;
  Pause pause_;
};

}  // namespace rtype::server
