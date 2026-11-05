#pragma once

#include <cstdint>
#include <optional>
#include "ActionSet.hpp"
#include "IPlayerInputLink.hpp"

/**
 * @brief Player input link double that sends nothing, as a ServerSession does
 * while the server has not accepted the player.
 */
class RefusingPlayerInputLink : public rtype::client::IPlayerInputLink {
 public:
  /** @returns Nothing: no input is sent. */
  std::optional<std::uint32_t> sendPlayerInput(
      rtype::client::ActionSet held) override;
};
