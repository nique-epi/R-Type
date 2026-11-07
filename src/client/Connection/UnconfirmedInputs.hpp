#pragma once

#include <cstdint>
#include <deque>
#include "PlayerInput.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::client {

/**
 * @brief Numbers the inputs the player sends and keeps those the server has not
 * confirmed, so each PlayerInput repeats them: a datagram lost on the way costs
 * no input, the next one carries it again. At most MAXIMUM_REPEATED_INPUTS
 * inputs before the newest are kept, the oldest forgotten first.
 */
class UnconfirmedInputs {
 public:
  /**
   * @brief Numbers @p actions after the previous input and keeps it.
   * @returns The PlayerInput to send: the new input, and the unconfirmed ones
   * before it, newest first.
   */
  [[nodiscard]] network::PlayerInput add(std::uint8_t actions,
                                         std::uint32_t shownTick);

  /** @brief Forgets the inputs numbered @p acknowledged or lower. */
  void confirmUpTo(std::uint32_t acknowledged);

  /** @brief Forgets every input; the next one is numbered
   * FIRST_INPUT_NUMBER. */
  void clear();

 private:
  std::uint32_t nextNumber_{network::FIRST_INPUT_NUMBER};
  std::deque<std::uint8_t> kept_;
};

}  // namespace rtype::client
