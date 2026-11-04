#include "ServerKeyRepository.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <cstdint>
#include <optional>
#include <string>
#include "MasterDatabase.hpp"
#include "ServerKeyRecord.hpp"
#include "ServerKeysConstants.hpp"

namespace rtype::master {

ServerKeyRepository::ServerKeyRepository(MasterDatabase& database)
    : database_(database) {}

void ServerKeyRepository::insert(const ServerKeyRecord& key) {
  database_.run([&key](SQLite::Database& connection) {
    SQLite::Statement insert{connection,
                             std::string{INSERT_SERVER_KEY_STATEMENT}};
    insert.bind(std::string{KEY_NAME_PARAMETER}, key.name);
    insert.bind(std::string{FINGERPRINT_PARAMETER}, key.fingerprint);
    insert.bind(
        std::string{ISSUED_AT_PARAMETER},
        static_cast<std::int64_t>(key.issuedAt.time_since_epoch().count()));
    insert.exec();
  });
}

std::optional<std::int64_t> ServerKeyRepository::idOfFingerprint(
    const std::string& fingerprint) {
  return database_.run([&fingerprint](SQLite::Database& connection)
                           -> std::optional<std::int64_t> {
    SQLite::Statement query{connection,
                            std::string{SERVER_KEY_OF_FINGERPRINT_QUERY}};
    query.bind(std::string{FINGERPRINT_PARAMETER}, fingerprint);
    if (!query.executeStep()) {
      return std::nullopt;
    }
    return query.getColumn(KEY_ID_COLUMN).getInt64();
  });
}

}  // namespace rtype::master
