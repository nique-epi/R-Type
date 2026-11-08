#pragma once

#include <SQLiteCpp/Database.h>
#include <chrono>
#include <filesystem>
#include <vector>
#include "Ban.hpp"
#include "BanTarget.hpp"

namespace rtype::server {

/**
 * @brief The bans of the file of the server. Every value reaches SQLite as a
 * bound parameter, so a nickname is never read as SQL.
 *
 * Blocks on the disk, briefly: the supervisor writes from its simulation
 * thread, a console line at a time; a game reads from its BanWatch thread.
 */
class BansDatabase {
 public:
  /**
   * @param file The database of this server, created when it does not exist.
   * @throws storage::StorageException when it cannot be opened (see
   * storage::openDatabaseFile()).
   */
  explicit BansDatabase(const std::filesystem::path& file);

  /** @brief Writes @p ban, in place of the ban of its target.
   * @throws SQLite::Exception when SQLite refuses it; nothing changes then. */
  void save(const Ban& ban);

  /** @returns Whether a ban of @p target was there; it is gone.
   * @throws SQLite::Exception when SQLite refuses it. */
  bool remove(const BanTarget& target);

  /** @returns The bans that have not ended at @p now, oldest first.
   * @throws SQLite::Exception when SQLite refuses it. */
  [[nodiscard]] std::vector<Ban> inForce(std::chrono::sys_seconds now) const;

 private:
  SQLite::Database database_;
};

}  // namespace rtype::server
