#pragma once

#include <SQLiteCpp/Database.h>
#include <chrono>
#include <cstdint>
#include <map>
#include <optional>
#include <vector>
#include "SeatKey.hpp"
#include "SeatedPlayer.hpp"

namespace rtype::master {

/** @brief The table `casino_seats`: the only code that writes its SQL. It
 * works on a connection the caller holds, inside the caller's transaction. */
class CasinoSeatRepository {
 public:
  /** @param connection It must outlive the repository. */
  explicit CasinoSeatRepository(SQLite::Database& connection);

  /** @returns When @p seat was last seen, nothing when it was never taken or
   * was left. */
  [[nodiscard]] std::optional<std::chrono::sys_seconds> seenAt(SeatKey seat);

  /** @returns The seats of the room of @p seat taken by other accounts and
   * seen at or after @p since. */
  [[nodiscard]] std::int64_t othersSeatedSince(SeatKey seat,
                                               std::chrono::sys_seconds since);

  /** @brief Writes @p seat as seen at @p seenAt, taken when it was not. */
  void markSeen(SeatKey seat, std::chrono::sys_seconds seenAt);

  /** @brief Removes @p seat; removing a seat no one holds does nothing. */
  void remove(SeatKey seat);

  /** @returns The accounts seated in @p room and seen at or after @p since,
   * first seated first. */
  [[nodiscard]] std::vector<SeatedPlayer> seatedSince(
      std::uint8_t room, std::chrono::sys_seconds since);

  /** @returns The seats seen at or after @p since, by room; a room with none
   * is left out. */
  [[nodiscard]] std::map<std::uint8_t, std::int64_t> countsSince(
      std::chrono::sys_seconds since);

  /** @brief Removes every seat last seen before @p since. */
  void removeSeenBefore(std::chrono::sys_seconds since);

 private:
  SQLite::Database& connection_;
};

}  // namespace rtype::master
