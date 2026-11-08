#pragma once

#include <functional>
#include "ConnectedPlayer.hpp"
#include "Expulsion.hpp"
#include "ExpulsionRule.hpp"
#include "Logger.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

/**
 * @brief Expels the players a rule chooses: the pilots the supervisor names,
 * or the pilots a ban bars. Tells each one, in its
 * session, with FAREWELL_COPIES copies of an Expulsion in a row, since
 * nothing repeats it, and marks it expelled for the reason of the rule, so
 * Departures closes its session
 * at the next tick. The copies are queued before the mark, so they always go
 * out under the session of the player. A player already marked is not told
 * again, so the copies of one order, or the bans read again, expel once.
 *
 * Use it on the simulation thread only.
 */
class PlayerExpeller {
 public:
  PlayerExpeller();

  /** @brief Expels every player of @p roster that @p rule chooses and that is
   * not expelled yet, each copy of its Expulsion handed to @p send. */
  void expel(const std::function<void(network::OutgoingDatagram)>& send,
             PlayerRoster& roster, const ExpulsionRule& rule) const;

 private:
  /** @brief Hands the Expulsion of @p player for @p reason to @p send, then
   * marks it expelled for that reason. */
  void tellExpelled(const std::function<void(network::OutgoingDatagram)>& send,
                    ConnectedPlayer& player,
                    network::ExpulsionReason reason) const;

  logging::Logger logger_;
};

}  // namespace rtype::server
