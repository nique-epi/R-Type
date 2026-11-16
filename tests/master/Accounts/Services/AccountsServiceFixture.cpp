#include "AccountsServiceFixture.hpp"
#include <SQLiteCpp/Column.h>
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <chrono>
#include <cstddef>
#include <optional>
#include <string>
#include <variant>
#include "AccountsServiceTestConstants.hpp"
#include "AccountsTestConstants.hpp"
#include "CreatedSession.hpp"
#include "CredentialsAttempt.hpp"
#include "Hashing.hpp"
#include "MasterTestConstants.hpp"
#include "PasswordKey.hpp"
#include "StorageConstants.hpp"
#include "User.hpp"
#include "UserCredentials.hpp"

namespace {

rtype::master::CredentialsAttempt aceRequest() {
  return rtype::master::CredentialsAttempt{
      .credentials =
          rtype::network::UserCredentials{
              .nickname = std::string{ACE_NICKNAME},
              .password = std::string{ACE_PASSWORD}},
      .address = std::string{SERVICE_CLIENT_ADDRESS},
      .requestedAt = SESSION_OPENED_AT};
}

}  // namespace

AccountsServiceFixture::AccountsServiceFixture()
    : service_(file_.database(),
               rtype::network::passwordKeyOfHexadecimal(TEST_PASSWORD_KEY)
                   .value_or(rtype::network::PasswordKey{})) {
  static_cast<void>(service_.createUser(aceRequest()));
}

std::string AccountsServiceFixture::aceToken() {
  const auto opened = service_.createSession(aceRequest());
  const auto* session = std::get_if<rtype::network::CreatedSession>(&opened);
  return session == nullptr ? std::string{} : session->token;
}

std::optional<rtype::network::User> AccountsServiceFixture::accountAt(
    const std::string& token, std::chrono::sys_seconds date) {
  return service_.userOfToken(token, date);
}

std::string AccountsServiceFixture::keptPassword() const {
  const SQLite::Database reader{
      file_.file().string(), SQLite::OPEN_READONLY,
      rtype::storage::DATABASE_BUSY_TIMEOUT_MILLISECONDS};
  SQLite::Statement query{reader, std::string{KEPT_PASSWORD_QUERY}};
  query.bind(std::string{KEPT_PASSWORD_NICKNAME_PARAMETER},
             std::string{ACE_NICKNAME});
  if (!query.executeStep()) {
    return {};
  }
  const SQLite::Column column = query.getColumn(KEPT_PASSWORD_COLUMN);
  return std::string{static_cast<const char*>(column.getBlob()),
                     static_cast<std::size_t>(column.getBytes())};
}
