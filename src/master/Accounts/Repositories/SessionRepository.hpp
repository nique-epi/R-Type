#pragma once

#include <chrono>
#include <optional>
#include <string>
#include "ActiveSession.hpp"
#include "MasterDatabase.hpp"
#include "NewSession.hpp"

namespace rtype::master {

/** @brief The table `sessions`: the only code that writes its SQL. Each call
 * runs behind the lock of the MasterDatabase. */
class SessionRepository {
 public:
  explicit SessionRepository(MasterDatabase& database);

  /** @brief Adds @p session, and removes in the same transaction every
   * session expired when it opens, so the table holds only live ones. */
  void addRemovingExpired(const NewSession& session);

  /** @returns The account of the session whose token has @p fingerprint and
   * that has not expired at @p now, with its expiry; nothing otherwise. */
  [[nodiscard]] std::optional<ActiveSession> ownerOf(
      const std::string& fingerprint, std::chrono::sys_seconds now);

  /** @brief Moves the expiry of the session whose token has @p fingerprint to
   * @p expiresAt. */
  void extend(const std::string& fingerprint,
              std::chrono::sys_seconds expiresAt);

  /** @brief Removes the session whose token has @p fingerprint; nothing when
   * none has it. */
  void remove(const std::string& fingerprint);

 private:
  MasterDatabase& database_;
};

}  // namespace rtype::master
