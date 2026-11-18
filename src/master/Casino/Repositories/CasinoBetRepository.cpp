#include "CasinoBetRepository.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "CasinoBindings.hpp"
#include "CasinoRepositoryConstants.hpp"
#include "EpochSeconds.hpp"
#include "MasterMessageText.hpp"
#include "PlayerBet.hpp"
#include "ResultColumnConstants.hpp"
#include "RoomRound.hpp"
#include "RouletteBet.hpp"
#include "RouletteBetKind.hpp"
#include "StatementParts.hpp"

namespace rtype::master {

namespace {

/** @returns The bet of the row @p query is on, its kind in the column
 * @p kindColumn and its number and amount in the two after it. */
network::RouletteBet betIn(const SQLite::Statement& query, int kindColumn) {
  return network::RouletteBet{
      .kind =
          network::rouletteBetKindOf(query.getColumn(kindColumn).getString())
              .value_or(network::RouletteBetKind::Straight),
      .firstNumber =
          static_cast<std::uint8_t>(query.getColumn(kindColumn + 1).getInt()),
      .amount = static_cast<std::uint32_t>(
          query.getColumn(kindColumn + 2).getInt64())};
}

}  // namespace

CasinoBetRepository::CasinoBetRepository(SQLite::Database& connection)
    : connection_(connection) {}

std::vector<network::RouletteBet> CasinoBetRepository::betsOf(
    network::RoomRound place, std::int64_t userId) {
  SQLite::Statement query{connection_, std::string{PLAYER_BETS_QUERY}};
  bindPlace(query, place);
  query.bind(std::string{CASINO_USER_ID_PARAMETER}, userId);
  return rowsOf(query, [](const SQLite::Statement& row) {
    return betIn(row, PLAYER_BET_KIND_COLUMN);
  });
}

std::vector<PlayerBet> CasinoBetRepository::roundBetsOf(
    network::RoomRound place) {
  SQLite::Statement query{connection_, std::string{ROUND_BETS_QUERY}};
  bindPlace(query, place);
  return rowsOf(query, [](const SQLite::Statement& row) {
    return PlayerBet{.userId = row.getColumn(ROUND_BET_USER_COLUMN).getInt64(),
                     .bet = betIn(row, ROUND_BET_KIND_COLUMN)};
  });
}

void CasinoBetRepository::removeOf(network::RoomRound place,
                                   std::int64_t userId) {
  SQLite::Statement remove{connection_,
                           std::string{DELETE_PLAYER_BETS_STATEMENT}};
  bindPlace(remove, place);
  remove.bind(std::string{CASINO_USER_ID_PARAMETER}, userId);
  remove.exec();
}

void CasinoBetRepository::insert(
    network::RoomRound place, std::int64_t userId,
    const std::vector<network::RouletteBet>& bets) {
  SQLite::Statement insert{connection_, std::string{INSERT_BET_STATEMENT}};
  bindPlace(insert, place);
  insert.bind(std::string{CASINO_USER_ID_PARAMETER}, userId);
  for (std::size_t position = 0; position < bets.size(); ++position) {
    const network::RouletteBet& bet = bets.at(position);
    insert.reset();
    insert.bind(std::string{POSITION_PARAMETER},
                static_cast<std::int64_t>(position));
    insert.bind(std::string{KIND_PARAMETER},
                std::string{network::rouletteBetKindText(bet.kind)});
    insert.bind(std::string{FIRST_NUMBER_PARAMETER}, int{bet.firstNumber});
    insert.bind(std::string{BET_AMOUNT_PARAMETER},
                static_cast<std::int64_t>(bet.amount));
    insert.exec();
  }
}

std::vector<network::RoomRound> CasinoBetRepository::undrawnBefore(
    std::uint64_t round) {
  SQLite::Statement query{connection_, std::string{UNDRAWN_ROUNDS_QUERY}};
  query.bind(std::string{ROUND_PARAMETER}, static_cast<std::int64_t>(round));
  return rowsOf(query, [](const SQLite::Statement& row) {
    return network::RoomRound{
        .room = static_cast<std::uint8_t>(row.getColumn(FIRST_COLUMN).getInt()),
        .round = static_cast<std::uint64_t>(
            row.getColumn(SECOND_COLUMN).getInt64())};
  });
}

void CasinoBetRepository::removeDrawnBefore(std::chrono::sys_seconds before) {
  SQLite::Statement remove{connection_,
                           std::string{DELETE_SETTLED_BETS_STATEMENT}};
  remove.bind(std::string{BEFORE_PARAMETER}, epochSecondsOf(before));
  remove.exec();
}

}  // namespace rtype::master
