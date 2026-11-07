#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include "Application.hpp"
#include "ConfigurationReport.hpp"
#include "CreatedServer.hpp"
#include "JournalFile.hpp"
#include "MasterDatabaseFile.hpp"

/**
 * @brief The modules of the master, started on a free port and a database of
 * their own, writing to a journal of their own and nothing on stderr; it stops
 * when it is destroyed.
 */
class RunningMaster {
 public:
  RunningMaster();

  /** @returns The TCP port its HTTP API listens on. */
  [[nodiscard]] std::uint16_t port() const;

  /** @returns Where a MasterClient reaches it: `http://127.0.0.1:<port>`. */
  [[nodiscard]] std::string url() const;

  /** @returns Where a master on @p port of the loopback would answer. */
  [[nodiscard]] static std::string urlOf(std::uint16_t port);

  /** @brief Stops the master, so its port answers nothing. */
  void stop();

  /** @brief Starts the master again, on the same port and database. */
  void restart();

  /** @returns The last heartbeat the server @p serverId sent, as the master
   * keeps it, or nothing before the first one. */
  [[nodiscard]] std::optional<std::string> lastHeartbeatOf(
      std::int64_t serverId) const;

  /** @returns The server named @p name, created on it as a host creates one,
   * with its key. */
  [[nodiscard]] rtype::network::CreatedServer createServer(
      std::string_view name) const;

 private:
  JournalFile journal_;
  MasterDatabaseFile databaseFile_;
  std::uint16_t port_;
  rtype::engine::ConfigurationReport report_;
  std::unique_ptr<rtype::engine::Application> application_;

  /** @returns The modules of the master on the port and the database. */
  [[nodiscard]] std::unique_ptr<rtype::engine::Application> application();
};
