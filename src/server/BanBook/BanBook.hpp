#pragma once

#include <chrono>
#include <filesystem>
#include <optional>
#include "Ban.hpp"
#include "BanList.hpp"
#include "BanTarget.hpp"
#include "BansDatabase.hpp"

namespace rtype::server {

/**
 * @brief The bans of the supervisor: the table of the file of the server and
 * a copy in memory, which the lobby of the supervisor and the console read
 * without touching the disk. Every change is written to the file first, then
 * to the copy, so a write the file refuses leaves both as they were. A ban
 * that ends stays in the copy until the next start; BanList::bars() no
 * longer counts it.
 *
 * The file is opened only when it exists or when the first ban is written,
 * so a supervisor that bans nobody, and runs without a master, creates no
 * file.
 *
 * Use it on the simulation thread only.
 */
class BanBook {
 public:
  /**
   * @brief Keeps @p file, and, when it exists, opens it and reads the bans in
   * force at @p now.
   * @throws storage::StorageException when it exists and cannot be opened.
   */
  void open(const std::filesystem::path& file, std::chrono::sys_seconds now);

  /** @brief Closes the file; the copy stays as it was. */
  void close();

  /** @brief Writes @p ban, in place of the ban of its target, opening the
   * file first when it is not open yet.
   * @throws storage::StorageException when the file cannot be opened, and
   * SQLite::Exception when it refuses the ban; nothing changes then. */
  void put(const Ban& ban);

  /** @returns Whether a ban of @p target was there; it is gone.
   * @throws SQLite::Exception when the file refuses it; nothing changes then.
   */
  bool remove(const BanTarget& target);

  [[nodiscard]] const BanList& list() const;

 private:
  /** @returns The file, opened, and created when it did not exist. */
  BansDatabase& database();

  std::filesystem::path file_;
  std::optional<BansDatabase> database_;
  BanList list_;
};

}  // namespace rtype::server
