#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include "ISessionTokenStorage.hpp"
#include "Logger.hpp"

namespace rtype::client {

/**
 * @brief The token of the session kept in a file of its own, readable and
 * writable by the player only, never in the settings and never logged.
 *
 * A keep replaces the file through replacePlayerFile(), the file made private
 * before the token is written, so the file is either the previous token or
 * the new one; a failure leaves the previous one. A file with no path keeps
 * nothing.
 */
class SessionTokenFile final : public ISessionTokenStorage {
 public:
  /** @param path The file, or nothing to keep no token. */
  explicit SessionTokenFile(std::optional<std::filesystem::path> path);

  /** @returns The text of the file; nothing when there is no file, when it is
   * empty, larger than MAXIMUM_SESSION_TOKEN_FILE_SIZE or cannot be read. */
  [[nodiscard]] std::optional<std::string> load() const override;
  void keep(const std::string& token) override;
  void forget() override;

 private:
  std::optional<std::filesystem::path> path_;
  logging::Logger logger_;
};

}  // namespace rtype::client
