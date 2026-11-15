#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include "Account.hpp"
#include "AccountCredentials.hpp"
#include "AccountRefusal.hpp"
#include "AccountRepository.hpp"
#include "CredentialsRequest.hpp"
#include "Logger.hpp"
#include "MasterDatabase.hpp"
#include "NewAccount.hpp"
#include "OpenedSession.hpp"
#include "PasswordKey.hpp"
#include "SessionRepository.hpp"
#include "SignInFailureRepository.hpp"

namespace rtype::master {

/**
 * @brief The rules of the accounts and of their sessions. A password is hashed
 * on the thread of the request, outside the lock of the database, and checked
 * only when its account exists: a nickname is public anyway, on every
 * leaderboard, and a creation says whether it is taken.
 */
class AccountsService {
 public:
  /** @param key The key every password hash is encrypted with. */
  AccountsService(MasterDatabase& database, const network::PasswordKey& key);

  /**
   * @brief Creates a player named by the credentials of @p request, whose
   * password only the hash is kept of.
   * @returns The account; or AccountRefusal::TooManyCreations when its address
   * created MAXIMUM_ACCOUNTS_PER_ADDRESS accounts in the last
   * ACCOUNT_CREATION_WINDOW; AccountRefusal::NicknameTaken when an account
   * has its nickname, its ASCII letters in any case. Both are checked before
   * the password is hashed, then again with the insert.
   */
  std::variant<network::Account, AccountRefusal> createAccount(
      const CredentialsRequest& request);

  /**
   * @brief Opens a session of SESSION_LIFETIME for the account the credentials
   * of @p request name, and writes in the journal who signed in, or who
   * failed, and from where; never the password.
   * @returns The session and its token; or AccountRefusal::TooManyFailures
   * when its address failed MAXIMUM_SIGN_IN_FAILURES sign-ins in the last
   * SIGN_IN_FAILURE_WINDOW, checked first; AccountRefusal::WrongCredentials,
   * counted as a failure, when no account has the nickname or its password is
   * another one.
   */
  std::variant<network::OpenedSession, AccountRefusal> openSession(
      const CredentialsRequest& request);

  /**
   * @returns The account of the session of @p token at @p now, or nothing for
   * a token not well formed, unknown or expired. A session last moved more
   * than SESSION_REFRESH_AGE ago expires SESSION_LIFETIME after @p now.
   */
  std::optional<network::Account> accountOfToken(std::string_view token,
                                                 std::chrono::sys_seconds now);

  /** @brief Closes the session of @p token; nothing for a token no session
   * has. */
  void closeSession(std::string_view token);

  /** @brief Makes the account named @p nickname an admin.
   * @returns Whether an account has that nickname. */
  bool makeAdmin(const std::string& nickname);

 private:
  [[nodiscard]] NewAccount newAccountOf(
      const CredentialsRequest& request) const;

  /** @returns Whether the address of @p request failed too many sign-ins;
   * writes it in the journal when it did. */
  [[nodiscard]] bool failedTooOften(const CredentialsRequest& request);

  /** @returns The account @p credentials name when its password is theirs. */
  [[nodiscard]] std::optional<network::Account> verifiedAccountOf(
      const network::AccountCredentials& credentials);

  /** @brief Counts the failed sign-in of @p request, and writes it in the
   * journal. */
  void recordFailure(const CredentialsRequest& request);

  /** @returns A new session of @p account, opened as @p request asks. */
  network::OpenedSession openedSessionFor(const network::Account& account,
                                          const CredentialsRequest& request);

  network::PasswordKey key_;
  AccountRepository accounts_;
  SessionRepository sessions_;
  SignInFailureRepository failures_;
  logging::Logger logger_;
};

}  // namespace rtype::master
