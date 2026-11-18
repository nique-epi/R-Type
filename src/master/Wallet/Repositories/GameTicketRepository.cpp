#include "GameTicketRepository.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include "EpochSeconds.hpp"
#include "GameTicketRepositoryConstants.hpp"
#include "GameTicketState.hpp"
#include "IssuedGameTicket.hpp"

namespace rtype::master {

GameTicketRepository::GameTicketRepository(SQLite::Database& connection)
    : connection_(connection) {}

void GameTicketRepository::insert(const IssuedGameTicket& ticket) {
  SQLite::Statement insert{connection_,
                           std::string{INSERT_GAME_TICKET_STATEMENT}};
  insert.bind(std::string{TICKET_FINGERPRINT_PARAMETER}, ticket.fingerprint);
  insert.bind(std::string{TICKET_USER_ID_PARAMETER}, ticket.userId);
  insert.bind(std::string{TICKET_ISSUED_AT_PARAMETER},
              epochSecondsOf(ticket.issuedAt));
  insert.bind(std::string{TICKET_EXPIRES_AT_PARAMETER},
              epochSecondsOf(ticket.expiresAt));
  insert.exec();
}

std::int64_t GameTicketRepository::issuedSince(std::int64_t userId,
                                               std::chrono::sys_seconds since) {
  SQLite::Statement query{connection_, std::string{TICKETS_ISSUED_SINCE_QUERY}};
  query.bind(std::string{TICKET_USER_ID_PARAMETER}, userId);
  query.bind(std::string{TICKET_SINCE_PARAMETER}, epochSecondsOf(since));
  query.executeStep();
  return query.getColumn(TICKET_COUNT_COLUMN).getInt64();
}

void GameTicketRepository::forgetExpired(std::chrono::sys_seconds now) {
  SQLite::Statement forget{connection_,
                           std::string{FORGET_EXPIRED_TICKETS_STATEMENT}};
  forget.bind(std::string{TICKET_NOW_PARAMETER}, epochSecondsOf(now));
  forget.exec();
}

std::optional<network::GameTicketState> GameTicketRepository::stateOf(
    std::int64_t userId, const std::string& fingerprint) {
  SQLite::Statement query{connection_, std::string{OWN_TICKET_QUERY}};
  query.bind(std::string{TICKET_FINGERPRINT_PARAMETER}, fingerprint);
  query.bind(std::string{TICKET_USER_ID_PARAMETER}, userId);
  if (!query.executeStep()) {
    return std::nullopt;
  }
  return network::GameTicketState{
      .expiresAt = dateOfEpochSeconds(
          query.getColumn(TICKET_EXPIRES_AT_COLUMN).getInt64()),
      .used = query.getColumn(TICKET_USED_COLUMN).getInt() != 0};
}

std::optional<std::int64_t> GameTicketRepository::redeem(
    const std::string& fingerprint, std::chrono::sys_seconds now) {
  SQLite::Statement redeem{connection_, std::string{REDEEM_TICKET_STATEMENT}};
  redeem.bind(std::string{TICKET_FINGERPRINT_PARAMETER}, fingerprint);
  redeem.bind(std::string{TICKET_NOW_PARAMETER}, epochSecondsOf(now));
  if (!redeem.executeStep()) {
    return std::nullopt;
  }
  return redeem.getColumn(TICKET_OWNER_COLUMN).getInt64();
}

}  // namespace rtype::master
