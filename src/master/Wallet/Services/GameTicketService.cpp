#include "GameTicketService.hpp"
#include <SQLiteCpp/Database.h>
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include "GameTicketGrant.hpp"
#include "GameTicketRepository.hpp"
#include "GameTicketServiceConstants.hpp"
#include "GameTicketState.hpp"
#include "GameTicketText.hpp"
#include "IssuedGameTicket.hpp"
#include "MasterDatabase.hpp"

namespace rtype::master {

GameTicketService::GameTicketService(MasterDatabase& database)
    : database_(database) {}

std::optional<network::GameTicketGrant> GameTicketService::issue(
    std::int64_t userId, std::chrono::sys_seconds now) {
  return database_.transact([userId, now](SQLite::Database& connection)
                                -> std::optional<network::GameTicketGrant> {
    GameTicketRepository tickets{connection};
    tickets.forgetExpired(now);
    if (tickets.issuedSince(userId, now - GAME_TICKET_WINDOW) >=
        MAXIMUM_GAME_TICKETS_PER_WINDOW) {
      return std::nullopt;
    }
    network::GameTicketGrant grant{.ticket = newGameTicket(),
                                   .expiresAt = now + GAME_TICKET_LIFETIME};
    tickets.insert(
        IssuedGameTicket{.fingerprint = gameTicketFingerprintOf(grant.ticket),
                         .userId = userId,
                         .issuedAt = now,
                         .expiresAt = grant.expiresAt});
    return grant;
  });
}

std::optional<network::GameTicketState> GameTicketService::stateOf(
    std::int64_t userId, const std::string& fingerprint) {
  return database_.run([userId, &fingerprint](SQLite::Database& connection) {
    return GameTicketRepository{connection}.stateOf(userId, fingerprint);
  });
}

}  // namespace rtype::master
