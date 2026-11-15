#include "AccountsService.hpp"
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include "Account.hpp"
#include "AccountCredentials.hpp"
#include "AccountRefusal.hpp"
#include "AccountRole.hpp"
#include "AccountsServiceConstants.hpp"
#include "CreationQuota.hpp"
#include "CredentialsRequest.hpp"
#include "Hashing.hpp"
#include "MasterConstants.hpp"
#include "MasterDatabase.hpp"
#include "MasterMessageText.hpp"
#include "NewAccount.hpp"
#include "NewSession.hpp"
#include "OpenedSession.hpp"
#include "PasswordKey.hpp"
#include "SessionOwner.hpp"
#include "SessionToken.hpp"
#include "SignInFailure.hpp"
#include "StoredAccount.hpp"

namespace rtype::master {

AccountsService::AccountsService(MasterDatabase& database,
                                 const network::PasswordKey& key)
    : key_(key),
      accounts_(database),
      sessions_(database),
      failures_(database),
      logger_(std::string{MASTER_LOGGER_NAME}) {}

std::variant<network::Account, AccountRefusal> AccountsService::createAccount(
    const CredentialsRequest& request) {
  const CreationQuota quota{
      .maximum = MAXIMUM_ACCOUNTS_PER_ADDRESS,
      .since = request.requestedAt - ACCOUNT_CREATION_WINDOW};
  if (const std::optional<AccountRefusal> refusal =
          accounts_.refusalBeforeHashing(request, quota)) {
    return *refusal;
  }
  const std::variant<std::int64_t, AccountRefusal> inserted =
      accounts_.insertWithin(newAccountOf(request), quota);
  if (const auto* refusal = std::get_if<AccountRefusal>(&inserted)) {
    return *refusal;
  }
  return network::Account{.id = std::get<std::int64_t>(inserted),
                          .nickname = request.credentials.nickname,
                          .role = network::AccountRole::Player};
}

std::variant<network::OpenedSession, AccountRefusal>
AccountsService::openSession(const CredentialsRequest& request) {
  if (failedTooOften(request)) {
    return AccountRefusal::TooManyFailures;
  }
  const std::optional<network::Account> account =
      verifiedAccountOf(request.credentials);
  if (!account.has_value()) {
    recordFailure(request);
    return AccountRefusal::WrongCredentials;
  }
  logger_.info(account->nickname, SIGNED_IN_TEXT, request.address);
  return openedSessionFor(*account, request);
}

std::optional<network::Account> AccountsService::accountOfToken(
    std::string_view token, std::chrono::sys_seconds now) {
  if (!network::isWellFormedSessionToken(token)) {
    return std::nullopt;
  }
  const std::string fingerprint = sessionFingerprintOf(token);
  const std::optional<SessionOwner> owner = sessions_.ownerOf(fingerprint, now);
  if (!owner.has_value()) {
    return std::nullopt;
  }
  if (owner->expiresAt - now < SESSION_LIFETIME - SESSION_REFRESH_AGE) {
    sessions_.extend(fingerprint, now + SESSION_LIFETIME);
  }
  return owner->account;
}

void AccountsService::closeSession(std::string_view token) {
  if (network::isWellFormedSessionToken(token)) {
    sessions_.remove(sessionFingerprintOf(token));
  }
}

bool AccountsService::makeAdmin(const std::string& nickname) {
  return accounts_.giveRole(nickname, network::AccountRole::Admin);
}

NewAccount AccountsService::newAccountOf(
    const CredentialsRequest& request) const {
  return NewAccount{.nickname = request.credentials.nickname,
                    .passwordHash = network::passwordHashOf(
                        request.credentials.password, key_),
                    .createdByAddress = request.address,
                    .createdAt = request.requestedAt};
}

bool AccountsService::failedTooOften(const CredentialsRequest& request) {
  const bool tooOften =
      failures_.failuresSince(request.address,
                              request.requestedAt - SIGN_IN_FAILURE_WINDOW) >=
      MAXIMUM_SIGN_IN_FAILURES;
  if (tooOften) {
    logger_.warn(SIGN_IN_AS_TEXT, request.credentials.nickname,
                 SIGN_IN_FROM_TEXT, request.address, TOO_MANY_FAILURES_TEXT);
  }
  return tooOften;
}

std::optional<network::Account> AccountsService::verifiedAccountOf(
    const network::AccountCredentials& credentials) {
  const std::optional<StoredAccount> stored =
      accounts_.accountNamed(credentials.nickname);
  if (!stored.has_value() ||
      !network::passwordMatches(credentials.password, stored->passwordHash,
                                key_)) {
    return std::nullopt;
  }
  return stored->account;
}

void AccountsService::recordFailure(const CredentialsRequest& request) {
  failures_.add(SignInFailure{.address = request.address,
                              .failedAt = request.requestedAt},
                request.requestedAt - SIGN_IN_FAILURE_WINDOW);
  logger_.warn(SIGN_IN_AS_TEXT, request.credentials.nickname, SIGN_IN_FROM_TEXT,
               request.address, WRONG_CREDENTIALS_TEXT);
}

network::OpenedSession AccountsService::openedSessionFor(
    const network::Account& account, const CredentialsRequest& request) {
  std::string token = newSessionToken();
  const std::chrono::sys_seconds expiresAt =
      request.requestedAt + SESSION_LIFETIME;
  sessions_.addRemovingExpired(
      NewSession{.tokenFingerprint = sessionFingerprintOf(token),
                 .accountId = account.id,
                 .address = request.address,
                 .createdAt = request.requestedAt,
                 .expiresAt = expiresAt});
  return network::OpenedSession{
      .token = std::move(token), .expiresAt = expiresAt, .account = account};
}

}  // namespace rtype::master
