#include "CasinoRoundRepository.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "CasinoBindings.hpp"
#include "CasinoRepositoryConstants.hpp"
#include "DrawnRound.hpp"
#include "EpochSeconds.hpp"
#include "ResultColumnConstants.hpp"
#include "RoomRound.hpp"
#include "StatementParts.hpp"

namespace rtype::master {

CasinoRoundRepository::CasinoRoundRepository(SQLite::Database& connection)
    : connection_(connection) {}

std::optional<std::uint8_t> CasinoRoundRepository::numberOf(
    network::RoomRound place) {
  SQLite::Statement query{connection_, std::string{WINNING_NUMBER_QUERY}};
  bindPlace(query, place);
  if (!query.executeStep()) {
    return std::nullopt;
  }
  return static_cast<std::uint8_t>(query.getColumn(FIRST_COLUMN).getInt());
}

std::vector<std::uint8_t> CasinoRoundRepository::roomsDrawnIn(
    std::uint64_t round) {
  SQLite::Statement query{connection_, std::string{DRAWN_ROOMS_QUERY}};
  query.bind(std::string{ROUND_PARAMETER}, static_cast<std::int64_t>(round));
  return rowsOf(query, [](const SQLite::Statement& row) {
    return static_cast<std::uint8_t>(row.getColumn(FIRST_COLUMN).getInt());
  });
}

bool CasinoRoundRepository::insert(const DrawnRound& round) {
  SQLite::Statement insert{connection_, std::string{INSERT_ROUND_STATEMENT}};
  bindPlace(insert, round.place);
  insert.bind(std::string{WINNING_NUMBER_PARAMETER}, int{round.winningNumber});
  insert.bind(std::string{DRAWN_AT_PARAMETER}, epochSecondsOf(round.drawnAt));
  return insert.exec() > 0;
}

std::vector<std::uint8_t> CasinoRoundRepository::numbersBefore(
    network::RoomRound place, std::size_t count) {
  SQLite::Statement query{connection_, std::string{HISTORY_QUERY}};
  bindPlace(query, place);
  query.bind(std::string{KEPT_PARAMETER}, static_cast<std::int64_t>(count));
  return rowsOf(query, [](const SQLite::Statement& row) {
    return static_cast<std::uint8_t>(row.getColumn(FIRST_COLUMN).getInt());
  });
}

void CasinoRoundRepository::pruneKeeping(std::size_t kept) {
  SQLite::Statement prune{connection_, std::string{PRUNE_ROUNDS_STATEMENT}};
  prune.bind(std::string{KEPT_PARAMETER}, static_cast<std::int64_t>(kept));
  prune.exec();
}

}  // namespace rtype::master
