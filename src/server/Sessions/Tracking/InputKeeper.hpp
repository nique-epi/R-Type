#pragma once

#include <functional>
#include "ConnectedPlayer.hpp"
#include "MessageOrigin.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

/**
 * @brief Tells the newest input of each player from a late one.
 *
 * A PlayerInput is newer when its inputNumber is higher than
 * ConnectedPlayer::newestInputNumber, which then takes that number, so an input
 * that arrives late or twice changes nothing. inputNumber is compared as a
 * plain number: a session sending 60 inputs per second would take more than
 * 800 days to wrap it. The number kept outlives a new world, since the client
 * numbers its inputs for the whole session. Each newer input is shown to
 * onNewerInput before its number is kept, so whoever reads presses sees every
 * input in order, even several arriving in one tick; what the input holds is
 * theirs to keep.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class InputKeeper {
 public:
  /**
   * @param roster The players; it must outlive the keeper.
   * @param onNewerInput Shown each input newer than the one kept, before it
   * replaces it; it must not be empty.
   */
  InputKeeper(
      PlayerRoster& roster,
      std::function<void(const ConnectedPlayer&, const network::PlayerInput&)>
          onNewerInput);

  /**
   * @brief Shows @p input to onNewerInput, then keeps its number for the player
   * who sent it, when it is newer than the one kept. Does nothing when the
   * sender is not a player.
   */
  void keepIfNewer(const network::MessageOrigin& origin,
                   const network::PlayerInput& input);

 private:
  PlayerRoster& roster_;
  std::function<void(const ConnectedPlayer&, const network::PlayerInput&)>
      onNewerInput_;
};

}  // namespace rtype::server
