#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include "GameLifecycle.hpp"
#include "IChildProcess.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/** @brief What the supervisor knows of one game process: one line of its
 * registry. */
struct SupervisedGame {
  std::uint32_t number{};
  GameLifecycle lifecycle{GameLifecycle::Starting};
  std::uint8_t playerCount{};
  /** @brief The UDP port the game listens on, chosen by the supervisor. */
  std::uint16_t port{};
  /** @brief The token the game and the supervisor put in their datagrams. */
  std::uint64_t linkToken{};
  /** @brief The sequence of the newest status accepted; nothing before the
   * first one. */
  std::optional<std::uint16_t> newestSequence;
  /** @brief The time since the launch, counted while the game is Starting. */
  engine::Duration sinceLaunch{};
  std::unique_ptr<IChildProcess> process;
};

}  // namespace rtype::server
