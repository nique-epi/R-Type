#pragma once

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
 * session.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class InputKeeper {
 public:
  /**
   * @param roster The players; it must outlive the keeper.
   */
  explicit InputKeeper(PlayerRoster& roster);

  /**
   * @brief Keeps @p input for the player who sent it when it is newer than the
   * one kept; logs at the debug level when it holds other actions. Does nothing
   * when the sender is not a player.
   */
  void keepIfNewer(const network::MessageOrigin& origin,
                   const network::PlayerInput& input);

 private:
  PlayerRoster& roster_;
  logging::Logger logger_;
};

}  // namespace rtype::server
