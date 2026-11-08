#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include "Endpoint.hpp"
#include "GameRules.hpp"
#include "IChildProcess.hpp"

namespace rtype::server {

/** @brief What the registry is given when a game process was started. */
struct GameOpening {
  std::uint32_t number{};
  std::uint16_t port{};
  std::uint64_t linkToken{};
  /** @brief Who asked for a new game, when the game opened for it. */
  std::optional<network::Endpoint> creator;
  /** @brief The rules the game is played by. */
  network::GameRules rules{network::defaultGameRules()};
  std::unique_ptr<IChildProcess> process;
};

}  // namespace rtype::server
