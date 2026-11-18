#include "GameTicketServiceFixture.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <cstdint>
#include <string>
#include "GameTicketService.hpp"
#include "GameTicketServiceTestConstants.hpp"
#include "MasterDatabase.hpp"
#include "WalletServiceTestConstants.hpp"

namespace {

std::int64_t insertedAce(rtype::master::MasterDatabase& database) {
  return database.run([](SQLite::Database& connection) {
    SQLite::Statement insert{connection,
                             std::string{INSERT_TEST_USER_STATEMENT}};
    insert.bind(std::string{TEST_USER_NICKNAME_PARAMETER},
                std::string{WALLET_TEST_NICKNAME});
    insert.executeStep();
    return insert.getColumn(FIRST_RESULT_COLUMN).getInt64();
  });
}

}  // namespace

GameTicketServiceFixture::GameTicketServiceFixture()
    : ace_(insertedAce(file_.database())), tickets_(file_.database()) {}

rtype::master::GameTicketService& GameTicketServiceFixture::tickets() {
  return tickets_;
}

std::int64_t GameTicketServiceFixture::ace() const { return ace_; }

std::int64_t GameTicketServiceFixture::rowsKeyedBy(
    const std::string& fingerprint) {
  return file_.database().run([&fingerprint](SQLite::Database& connection) {
    SQLite::Statement query{connection, std::string{TICKET_ROWS_QUERY}};
    query.bind(std::string{TICKET_FINGERPRINT_TEST_PARAMETER}, fingerprint);
    query.executeStep();
    return query.getColumn(FIRST_RESULT_COLUMN).getInt64();
  });
}
