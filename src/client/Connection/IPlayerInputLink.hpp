#pragma once

#include <cstdint>
#include <optional>
#include "ActionSet.hpp"

namespace rtype::client {

/**
 * @brief Where the game sends the actions the player holds: ServerSession in
 * the client, a double in the tests. Each input repeats the ones the server has
 * not confirmed yet, see confirmInputsUpTo().
 */
class IPlayerInputLink {
 public:
  virtual ~IPlayerInputLink() = default;

  /**
   * @brief Sends the server a PlayerInput holding @p held, numbered after the
   * previous one and repeating the unconfirmed ones before it, with
   * @p shownTick, the tick of the server the client draws the other entities
   * at (NO_SHOWN_TICK before the first state), or nothing while the server has
   * not accepted the player.
   * @returns The number the input was sent under, or nothing when none was
   * sent.
   */
  virtual std::optional<std::uint32_t> sendPlayerInput(
      ActionSet held, std::uint32_t shownTick) = 0;

  /**
   * @brief Tells the link the server applied the inputs numbered
   * @p acknowledged or lower: the next PlayerInput no longer repeats them.
   */
  virtual void confirmInputsUpTo(std::uint32_t acknowledged) = 0;
};

}  // namespace rtype::client
