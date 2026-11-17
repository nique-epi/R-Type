#include "SignInFailureRepository.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <chrono>
#include <cstdint>
#include <string>
#include "AccountsRepositoryConstants.hpp"
#include "EpochSeconds.hpp"
#include "MasterDatabase.hpp"
#include "ServersRepositoryConstants.hpp"
#include "SignInFailure.hpp"
#include "StatementParts.hpp"

namespace rtype::master {

SignInFailureRepository::SignInFailureRepository(MasterDatabase& database)
    : database_(database) {}

std::int64_t SignInFailureRepository::failuresSince(
    const std::string& address, std::chrono::sys_seconds since) {
  return database_.run([&address, since](SQLite::Database& connection) {
    SQLite::Statement query{connection,
                            std::string{SIGN_IN_FAILURES_SINCE_QUERY}};
    query.bind(std::string{ADDRESS_PARAMETER}, address);
    query.bind(std::string{SINCE_PARAMETER}, epochSecondsOf(since));
    query.executeStep();
    return query.getColumn(COUNT_COLUMN).getInt64();
  });
}

void SignInFailureRepository::add(const SignInFailure& failure,
                                  std::chrono::sys_seconds forgetBefore) {
  database_.transact([&failure, forgetBefore](SQLite::Database& connection) {
    eraseOlderThan(connection, FORGET_SIGN_IN_FAILURES_STATEMENT, forgetBefore);
    SQLite::Statement insert{connection,
                             std::string{INSERT_SIGN_IN_FAILURE_STATEMENT}};
    insert.bind(std::string{ADDRESS_PARAMETER}, failure.address);
    insert.bind(std::string{CREATED_AT_PARAMETER},
                epochSecondsOf(failure.failedAt));
    insert.exec();
  });
}

}  // namespace rtype::master
