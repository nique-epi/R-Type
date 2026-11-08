#include "BanBookFixture.hpp"
#include <SQLiteCpp/Database.h>
#include <filesystem>
#include <string>
#include "BanBookTestConstants.hpp"
#include "StorageConstants.hpp"

namespace rtype::server::testing {

std::filesystem::path BanBookFixture::file() const {
  return folder_.path() / BAN_BOOK_FILE_NAME;
}

void BanBookFixture::dropTheBans() const {
  SQLite::Database other{file().string(), SQLite::OPEN_READWRITE,
                         rtype::storage::DATABASE_BUSY_TIMEOUT_MILLISECONDS};
  other.exec(std::string{DROP_BANS_STATEMENT});
}

}  // namespace rtype::server::testing
