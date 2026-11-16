#include "CasinoSeatRepository.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <chrono>
#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>
#include "CasinoBindings.hpp"
#include "CasinoRepositoryConstants.hpp"
#include "EpochSeconds.hpp"
#include "SeatKey.hpp"
#include "SeatedPlayer.hpp"

namespace rtype::master {

CasinoSeatRepository::CasinoSeatRepository(SQLite::Database& connection)
    : connection_(connection) {}

std::optional<std::chrono::sys_seconds> CasinoSeatRepository::seenAt(
    SeatKey seat) {
  SQLite::Statement query{connection_, std::string{SEAT_SEEN_QUERY}};
  bindSeat(query, seat);
  if (!query.executeStep()) {
    return std::nullopt;
  }
  return dateOfEpochSeconds(query.getColumn(CASINO_FIRST_COLUMN).getInt64());
}

std::int64_t CasinoSeatRepository::othersSeatedSince(
    SeatKey seat, std::chrono::sys_seconds since) {
  SQLite::Statement query{connection_, std::string{OTHER_SEATS_COUNT_QUERY}};
  bindSeat(query, seat);
  query.bind(std::string{SEEN_SINCE_PARAMETER}, epochSecondsOf(since));
  query.executeStep();
  return query.getColumn(CASINO_FIRST_COLUMN).getInt64();
}

void CasinoSeatRepository::markSeen(SeatKey seat,
                                    std::chrono::sys_seconds seenAt) {
  SQLite::Statement replace{connection_, std::string{REPLACE_SEAT_STATEMENT}};
  bindSeat(replace, seat);
  replace.bind(std::string{SEEN_AT_PARAMETER}, epochSecondsOf(seenAt));
  replace.exec();
}

void CasinoSeatRepository::remove(SeatKey seat) {
  SQLite::Statement remove{connection_, std::string{DELETE_SEAT_STATEMENT}};
  bindSeat(remove, seat);
  remove.exec();
}

std::vector<SeatedPlayer> CasinoSeatRepository::seatedSince(
    std::uint8_t room, std::chrono::sys_seconds since) {
  SQLite::Statement query{connection_, std::string{SEATED_PLAYERS_QUERY}};
  query.bind(std::string{ROOM_PARAMETER}, int{room});
  query.bind(std::string{SEEN_SINCE_PARAMETER}, epochSecondsOf(since));
  std::vector<SeatedPlayer> players;
  while (query.executeStep()) {
    players.push_back(SeatedPlayer{
        .userId = query.getColumn(CASINO_FIRST_COLUMN).getInt64(),
        .nickname = query.getColumn(CASINO_SECOND_COLUMN).getString()});
  }
  return players;
}

std::map<std::uint8_t, std::int64_t> CasinoSeatRepository::countsSince(
    std::chrono::sys_seconds since) {
  SQLite::Statement query{connection_, std::string{SEATED_COUNTS_QUERY}};
  query.bind(std::string{SEEN_SINCE_PARAMETER}, epochSecondsOf(since));
  std::map<std::uint8_t, std::int64_t> counts;
  while (query.executeStep()) {
    counts.emplace(static_cast<std::uint8_t>(
                       query.getColumn(CASINO_FIRST_COLUMN).getInt()),
                   query.getColumn(CASINO_SECOND_COLUMN).getInt64());
  }
  return counts;
}

void CasinoSeatRepository::removeSeenBefore(std::chrono::sys_seconds since) {
  SQLite::Statement remove{connection_,
                           std::string{DELETE_OLD_SEATS_STATEMENT}};
  remove.bind(std::string{SEEN_SINCE_PARAMETER}, epochSecondsOf(since));
  remove.exec();
}

}  // namespace rtype::master
