#include "AccountsService.hpp"
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include "AccountsServiceConstants.hpp"
#include "ActiveSession.hpp"
#include "CreatedSession.hpp"
#include "CreationQuota.hpp"
#include "CredentialsAttempt.hpp"
#include "CredentialsRefusal.hpp"
#include "Hashing.hpp"
#include "MasterConstants.hpp"
#include "MasterDatabase.hpp"
#include "MasterMessageText.hpp"
#include "NewSession.hpp"
#include "NewUser.hpp"
#include "PasswordKey.hpp"
#include "SessionToken.hpp"
#include "SignInFailure.hpp"
#include "StoredUser.hpp"
#include "User.hpp"
#include "UserCredentials.hpp"
#include "UserRole.hpp"

namespace rtype::master {

AccountsService::AccountsService(MasterDatabase& database,
                                 const network::PasswordKey& key)
    : key_(key),
      users_(database),
      sessions_(database),
      failures_(database),
      logger_(std::string{MASTER_LOGGER_NAME}) {}

std::variant<network::User, CredentialsRefusal> AccountsService::createUser(
    const CredentialsAttempt& request) {
  const CreationQuota quota{
      .maximum = MAXIMUM_USERS_PER_ADDRESS,
      .since = request.requestedAt - USER_CREATION_WINDOW};
  if (const std::optional<CredentialsRefusal> refusal =
          users_.refusalBeforeHashing(request, quota)) {
    return *refusal;
  }
  const std::variant<std::int64_t, CredentialsRefusal> inserted =
      users_.insertWithin(newUserOf(request), quota);
  if (const auto* refusal = std::get_if<CredentialsRefusal>(&inserted)) {
    return *refusal;
  }
  return network::User{.id = std::get<std::int64_t>(inserted),
                       .nickname = request.credentials.nickname,
                       .role = network::UserRole::Player};
}

std::variant<network::CreatedSession, CredentialsRefusal>
AccountsService::createSession(const CredentialsAttempt& request) {
  if (failedTooOften(request)) {
    return CredentialsRefusal::TooManyFailures;
  }
  const std::optional<network::User> user = verifiedUserOf(request.credentials);
  if (!user.has_value()) {
    recordFailure(request);
    return CredentialsRefusal::WrongCredentials;
  }
  logger_.info(user->nickname, SIGNED_IN_TEXT, request.address);
  return createdSessionFor(*user, request);
}

std::optional<network::User> AccountsService::userOfToken(
    std::string_view token, std::chrono::sys_seconds now) {
  if (!network::isWellFormedSessionToken(token)) {
    return std::nullopt;
  }
  const std::string fingerprint = sessionFingerprintOf(token);
  const std::optional<ActiveSession> owner =
      sessions_.ownerOf(fingerprint, now);
  if (!owner.has_value()) {
    return std::nullopt;
  }
  if (owner->expiresAt - now < SESSION_LIFETIME - SESSION_REFRESH_AGE) {
    sessions_.extend(fingerprint, now + SESSION_LIFETIME);
  }
  return owner->user;
}

void AccountsService::deleteSession(std::string_view token) {
  if (network::isWellFormedSessionToken(token)) {
    sessions_.remove(sessionFingerprintOf(token));
  }
}

bool AccountsService::makeAdmin(const std::string& nickname) {
  return users_.giveRole(nickname, network::UserRole::Admin);
}

NewUser AccountsService::newUserOf(const CredentialsAttempt& request) const {
  return NewUser{.nickname = request.credentials.nickname,
                 .passwordHash = network::passwordHashOf(
                     request.credentials.password, key_),
                 .createdByAddress = request.address,
                 .createdAt = request.requestedAt};
}

bool AccountsService::failedTooOften(const CredentialsAttempt& request) {
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

std::optional<network::User> AccountsService::verifiedUserOf(
    const network::UserCredentials& credentials) {
  const std::optional<StoredUser> stored =
      users_.userNamed(credentials.nickname);
  if (!stored.has_value() ||
      !network::passwordMatches(credentials.password, stored->passwordHash,
                                key_)) {
    return std::nullopt;
  }
  return stored->user;
}

void AccountsService::recordFailure(const CredentialsAttempt& request) {
  failures_.add(SignInFailure{.address = request.address,
                              .failedAt = request.requestedAt},
                request.requestedAt - SIGN_IN_FAILURE_WINDOW);
  logger_.warn(SIGN_IN_AS_TEXT, request.credentials.nickname, SIGN_IN_FROM_TEXT,
               request.address, WRONG_CREDENTIALS_TEXT);
}

network::CreatedSession AccountsService::createdSessionFor(
    const network::User& user, const CredentialsAttempt& request) {
  std::string token = newSessionToken();
  const std::chrono::sys_seconds expiresAt =
      request.requestedAt + SESSION_LIFETIME;
  sessions_.addRemovingExpired(
      NewSession{.tokenFingerprint = sessionFingerprintOf(token),
                 .userId = user.id,
                 .address = request.address,
                 .createdAt = request.requestedAt,
                 .expiresAt = expiresAt});
  return network::CreatedSession{
      .token = std::move(token), .expiresAt = expiresAt, .user = user};
}

}  // namespace rtype::master
