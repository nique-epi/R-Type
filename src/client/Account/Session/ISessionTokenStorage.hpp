#pragma once

#include <optional>
#include <string>

namespace rtype::client {

/**
 * @brief Where the token of the account session is kept between two
 * launches, so the player stays signed in without signing in again.
 */
class ISessionTokenStorage {
 public:
  virtual ~ISessionTokenStorage() = default;

  /** @returns The token the previous launch kept; nothing when none was
   * kept, or it cannot be read. */
  [[nodiscard]] virtual std::optional<std::string> load() const = 0;

  /** @brief Keeps @p token for the next launch, in place of the one kept
   * before. */
  virtual void keep(const std::string& token) = 0;

  /** @brief Forgets the token kept, if any: the next launch is a guest. */
  virtual void forget() = 0;
};

}  // namespace rtype::client
