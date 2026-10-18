#pragma once

#include "ActionSet.hpp"

namespace rtype::client {

/**
 * @brief Where the game sends the actions the player holds: ServerConnection in
 * the client, a double in the tests.
 */
class IPlayerInputLink {
 public:
  virtual ~IPlayerInputLink() = default;

  /**
   * @brief Sends the server a PlayerInput holding @p held, numbered after the
   * previous one, or nothing while the server has not accepted the player.
   */
  virtual void sendPlayerInput(ActionSet held) = 0;
};

}  // namespace rtype::client
