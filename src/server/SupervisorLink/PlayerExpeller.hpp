#pragma once

#include <string_view>
#include "ConnectedPlayer.hpp"
#include "IDatagramSender.hpp"
#include "Logger.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

/**
 * @brief Expels the players the supervisor names: tells each one, in its
 * session, with FAREWELL_COPIES copies of an Expulsion in a row, since
 * nothing repeats it, and marks it expelled, so Departures closes its session
 * at the next tick. The copies are queued before the mark, so they always go
 * out under the session of the player. A player already marked is not told
 * again, so the copies of one order expel once.
 *
 * Use it on the simulation thread only.
 */
class PlayerExpeller {
 public:
  PlayerExpeller();

  /** @brief Expels every player of @p roster named @p nickname (see
   * network::sameNickname()) and not expelled yet, its Expulsion queued on
   * @p sender. */
  void expel(IDatagramSender& sender, PlayerRoster& roster,
             std::string_view nickname) const;

 private:
  /** @brief Queues the Expulsion of @p player on @p sender, then marks it
   * expelled. */
  void tellExpelled(IDatagramSender& sender, ConnectedPlayer& player) const;

  logging::Logger logger_;
};

}  // namespace rtype::server
