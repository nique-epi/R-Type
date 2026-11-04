#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include "MasterDatabase.hpp"
#include "ServerKeyRecord.hpp"

namespace rtype::master {

/** @brief The table `server_keys`: the only code that writes its SQL. */
class ServerKeyRepository {
 public:
  explicit ServerKeyRepository(MasterDatabase& database);

  /** @brief Adds @p key to the table. */
  void insert(const ServerKeyRecord& key);

  /** @returns The id of the key of @p fingerprint, or nothing. */
  [[nodiscard]] std::optional<std::int64_t> idOfFingerprint(
      const std::string& fingerprint);

 private:
  MasterDatabase& database_;
};

}  // namespace rtype::master
