#pragma once

#include <cstdint>
#include <optional>
#include "GameListReading.hpp"
#include "ServerListReading.hpp"

namespace rtype::client {

/**
 * @brief The reads of the master the list of servers needs, without the frame
 * ever waiting on HTTP: an ask returns at once, and its reading shows up on a
 * later frame. A newer ask of a kind replaces the one before it.
 */
class IMasterReads {
 public:
  virtual ~IMasterReads() = default;

  /** @returns The number of this ask for the list of servers. */
  virtual std::uint64_t askServers() = 0;

  /** @returns The number of this ask for the games of @p serverId. */
  virtual std::uint64_t askGames(std::int64_t serverId) = 0;

  /** @returns The newest list of servers read, nothing before the first. */
  [[nodiscard]] virtual std::optional<ServerListReading> servers() const = 0;

  /** @returns The newest list of games read, nothing before the first. */
  [[nodiscard]] virtual std::optional<GameListReading> games() const = 0;
};

}  // namespace rtype::client
