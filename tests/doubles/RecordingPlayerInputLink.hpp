#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "ActionSet.hpp"
#include "IPlayerInputLink.hpp"

/**
 * @brief Player input link double: it writes down the actions of every input
 * the game sends, in order, and sends nothing.
 */
class RecordingPlayerInputLink : public rtype::client::IPlayerInputLink {
 public:
  /** @returns The number of an input sent, counted from FIRST_INPUT_NUMBER. */
  std::optional<std::uint32_t> sendPlayerInput(
      rtype::client::ActionSet held) override;

  /** @returns The actions byte of every input sent, oldest first. */
  [[nodiscard]] const std::vector<std::uint8_t>& sentActions() const;

 private:
  std::vector<std::uint8_t> sentActions_;
};
