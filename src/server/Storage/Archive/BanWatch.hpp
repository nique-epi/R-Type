#pragma once

#include <filesystem>
#include <stop_token>
#include <thread>
#include "BansDatabase.hpp"
#include "LatestBans.hpp"
#include "Logger.hpp"

namespace rtype::server {

/**
 * @brief Reads the bans in force from the file of the server again every
 * BAN_REREAD_INTERVAL, on a thread and a connection of its own, and publishes
 * them in LatestBans. A failed read keeps the bans read before, and is logged.
 * open() reads once before it returns, so the lobby has the bans from the
 * first tick on.
 */
class BanWatch {
 public:
  BanWatch();

  /** @brief Opens @p file, reads its bans, then starts the thread.
   * @throws storage::StorageException when @p file cannot be opened. */
  void open(const std::filesystem::path& file);

  /** @brief Stops the thread and closes the connection. */
  void close();

  [[nodiscard]] LatestBans& bans();

 private:
  void run(const std::stop_token& stop, const BansDatabase& database);
  void readAgain(const BansDatabase& database);

  LatestBans bans_;
  logging::Logger logger_;
  std::jthread worker_;
};

}  // namespace rtype::server
