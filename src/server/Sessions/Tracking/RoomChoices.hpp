#pragma once

#include "MessageDispatcher.hpp"
#include "PlayerRoster.hpp"
#include "ReadyKeeper.hpp"
#include "RoomChoiceHandlers.hpp"

namespace rtype::server {

/**
 * @brief Hands the choices a player makes in the waiting room (its ship look,
 * its place at the roulette table, its bets) to their handler, with the player
 * who sent them, only while the game takes the choices of the waiting room.
 *
 * A datagram may hold a Disconnection before a choice, which frees the seat:
 * the choice of an address with no seat any more goes nowhere.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class RoomChoices {
 public:
  /**
   * @param roster The players; it must outlive the choices.
   * @param ready Tells whether the game takes the choices of the waiting room;
   * it must outlive the choices.
   * @param handlers What each choice goes to. None may be empty.
   */
  RoomChoices(PlayerRoster& roster, const ReadyKeeper& ready,
              RoomChoiceHandlers handlers);

  /** @brief Tells @p dispatcher to hand each choice to its handler. */
  void registerWith(network::MessageDispatcher& dispatcher);

 private:
  /** @returns The player who sent from @p origin when the game takes the
   * choices of the waiting room, nullptr otherwise. */
  [[nodiscard]] ConnectedPlayer* chooserOf(
      const network::MessageOrigin& origin);

  PlayerRoster& roster_;
  const ReadyKeeper& ready_;
  RoomChoiceHandlers handlers_;
};

}  // namespace rtype::server
