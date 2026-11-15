#include "AccountsServiceFixture.hpp"
#include <SQLiteCpp/Column.h>
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <chrono>
#include <cstddef>
#include <optional>
#include <string>
#include <variant>
#include "Account.hpp"
#include "AccountCredentials.hpp"
#include "AccountsServiceTestConstants.hpp"
#include "AccountsTestConstants.hpp"
#include "CredentialsRequest.hpp"
#include "Hashing.hpp"
#include "MasterTestConstants.hpp"
#include "OpenedSession.hpp"
#include "PasswordKey.hpp"
#include "StorageConstants.hpp"

namespace {

rtype::master::CredentialsRequest aceRequest() {
  return rtype::master::CredentialsRequest{
      .credentials =
          rtype::network::AccountCredentials{
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
  static_cast<void>(service_.createAccount(aceRequest()));
}

std::string AccountsServiceFixture::aceToken() {
  const auto opened = service_.openSession(aceRequest());
  const auto* session = std::get_if<rtype::network::OpenedSession>(&opened);
  return session == nullptr ? std::string{} : session->token;
}

std::optional<rtype::network::Account> AccountsServiceFixture::accountAt(
    const std::string& token, std::chrono::sys_seconds date) {
  return service_.accountOfToken(token, date);
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
