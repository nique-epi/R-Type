#pragma once

#include <cstdint>
#include <optional>
#include <random>
#include "Endpoint.hpp"
#include "GameRules.hpp"
#include "GameSpawnerParts.hpp"
#include "Logger.hpp"

namespace rtype::server {

/**
 * @brief Starts the game processes of the supervisor: picks a port and a link
 * token for the new game, starts its process, adds it to the registry and lets
 * the network admit it, so its first status is not dropped as a stranger's.
 */
class GameSpawner {
 public:
  explicit GameSpawner(GameSpawnerParts parts);

  /**
   * @brief Starts a game process played by @p rules, which the registry
   * remembers @p creator opened: the sender of a NewGame, nothing for a
   * player who took any game.
   * @returns Whether it started. When the system refuses (no free port, the
   * program cannot be started), nothing is added, the error is logged and the
   * supervisor goes on: a game that cannot start never stops the others.
   */
  [[nodiscard]] bool openGame(std::optional<network::Endpoint> creator,
                              network::GameRules rules);

 private:
  [[nodiscard]] std::uint64_t drawLinkToken();

  GameSpawnerParts parts_;
  std::uint32_t nextNumber_{0};
  std::random_device random_;
  logging::Logger logger_;
};

}  // namespace rtype::server
