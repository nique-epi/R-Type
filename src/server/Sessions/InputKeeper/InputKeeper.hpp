#pragma once

#include <functional>
#include "ConnectedPlayer.hpp"
#include "Logger.hpp"
#include "MessageOrigin.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

/**
 * @brief Keeps the newest input of each player.
 *
 * A PlayerInput is kept in ConnectedPlayer::newestInput when its inputNumber
 * is higher than the one kept, so an input that arrives late or twice changes
 * nothing. inputNumber is compared as a plain number: a session sending 60
 * inputs per second would take more than 800 days to wrap it. The input kept
 * outlives a new world, since the client numbers its inputs for the whole
 * session. Each input kept is first shown to onNewerInput, the input it
 * replaces still kept, so whoever reads presses sees every input in order,
 * even several arriving in one tick.
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
   * @brief Keeps @p input for the player who sent it when it is newer than the
   * one kept; logs at the debug level when it holds other actions. Does nothing
   * when the sender is not a player.
   */
  void keepIfNewer(const network::MessageOrigin& origin,
                   const network::PlayerInput& input);

 private:
  PlayerRoster& roster_;
  std::function<void(const ConnectedPlayer&, const network::PlayerInput&)>
      onNewerInput_;
  logging::Logger logger_;
};

}  // namespace rtype::server
