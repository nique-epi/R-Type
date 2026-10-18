#pragma once

#include <cstdint>
#include <vector>
#include "ActionSet.hpp"
#include "IPlayerInputLink.hpp"

/**
 * @brief Player input link double: it writes down the actions of every input
 * the game sends, in order, and sends nothing.
 */
class RecordingPlayerInputLink : public rtype::client::IPlayerInputLink {
 public:
  void sendPlayerInput(rtype::client::ActionSet held) override;

  /** @returns The actions byte of every input sent, oldest first. */
  [[nodiscard]] const std::vector<std::uint8_t>& sentActions() const;

 private:
  std::vector<std::uint8_t> sentActions_;
};
