#pragma once

#include <SQLiteCpp/Database.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include "DrawnRound.hpp"
#include "RoomRound.hpp"

namespace rtype::master {

/** @brief The table `casino_rounds`: the only code that writes its SQL. It
 * works on a connection the caller holds, inside the caller's transaction. */
class CasinoRoundRepository {
 public:
  /** @param connection It must outlive the repository. */
  explicit CasinoRoundRepository(SQLite::Database& connection);

  /** @returns The number @p place drew, nothing before it was drawn. */
  [[nodiscard]] std::optional<std::uint8_t> numberOf(network::RoomRound place);

  /** @returns The rooms whose round @p round was drawn. */
  [[nodiscard]] std::vector<std::uint8_t> roomsDrawnIn(std::uint64_t round);

  /** @brief Writes @p round as drawn.
   * @returns Whether it was written: false when its round was drawn
   * already, which keeps its first number. */
  bool insert(const DrawnRound& round);

  /** @returns The numbers of the rounds of the room of @p place before its
   * round, newest first, @p count at most. */
  [[nodiscard]] std::vector<std::uint8_t> numbersBefore(
      network::RoomRound place, std::size_t count);

  /** @brief Removes the rounds of each room older than its last @p kept,
   * those whose bets are kept excepted. */
  void pruneKeeping(std::size_t kept);

 private:
  SQLite::Database& connection_;
};

}  // namespace rtype::master
