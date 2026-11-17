#pragma once

#include <SQLiteCpp/Database.h>
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include "DeviceLinkVerdict.hpp"
#include "NewDeviceLink.hpp"
#include "StoredDeviceLink.hpp"

namespace rtype::master {

/** @brief The table `device_links`, the links a game asked for to sign in
 * through the web site: the only code that writes its SQL. It works on a
 * connection the caller holds, inside the caller's transaction. */
class DeviceLinkRepository {
 public:
  /** @param connection It must outlive the repository. */
  explicit DeviceLinkRepository(SQLite::Database& connection);

  /** @brief Adds @p link, unless a link holds its short code or its secret.
   * @returns Whether it was added. */
  [[nodiscard]] bool insert(const NewDeviceLink& link);

  /** @returns How many links @p address asked for after @p since. */
  [[nodiscard]] std::int64_t createdSince(const std::string& address,
                                          std::chrono::sys_seconds since);

  /** @brief Forgets the links expired before @p oldest. */
  void forgetExpiredBefore(std::chrono::sys_seconds oldest);

  /** @returns The link whose secret has @p fingerprint, nothing when none. */
  [[nodiscard]] std::optional<StoredDeviceLink> ofSecret(
      const std::string& fingerprint);

  /** @returns The link of the short code @p userCode, nothing when none. */
  [[nodiscard]] std::optional<StoredDeviceLink> ofUserCode(
      const std::string& userCode);

  /** @brief Notes that the game read the link of @p fingerprint at
   * @p readAt. */
  void markRead(const std::string& fingerprint,
                std::chrono::sys_seconds readAt);

  /** @brief Writes @p verdict on the link of @p userCode, when no one decided
   * on it yet.
   * @returns Whether it was written. */
  [[nodiscard]] bool decide(const std::string& userCode,
                            DeviceLinkVerdict verdict);

  /** @brief Removes the link whose secret has @p fingerprint; nothing when
   * none has it. */
  void remove(const std::string& fingerprint);

 private:
  SQLite::Database& connection_;
};

}  // namespace rtype::master
