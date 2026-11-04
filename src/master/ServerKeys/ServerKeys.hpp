#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "MasterDatabase.hpp"
#include "ServerKeyRepository.hpp"

namespace rtype::master {

/**
 * @brief The keys of the game servers: issues one for an admin, and tells
 * which key a request presents. Safe to call from every thread of the HTTP
 * server, through the lock of the MasterDatabase.
 */
class ServerKeys {
 public:
  explicit ServerKeys(MasterDatabase& database);

  /**
   * @returns A new key named @p name, issued at @p now: the only time it is
   * ever seen, since the table keeps its fingerprint alone.
   */
  [[nodiscard]] std::string issue(const std::string& name,
                                  std::chrono::sys_seconds now);

  /**
   * @returns The id of the key @p keyText, or nothing when it is no key the
   * master issued. A text that is not well formed (isWellFormedServerKey()) is
   * refused before any hash or read.
   */
  [[nodiscard]] std::optional<std::int64_t> keyIdOf(std::string_view keyText);

 private:
  ServerKeyRepository repository_;
};

}  // namespace rtype::master
