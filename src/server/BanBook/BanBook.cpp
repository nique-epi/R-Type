#include "BanBook.hpp"
#include <chrono>
#include <filesystem>
#include <system_error>
#include "Ban.hpp"
#include "BanList.hpp"
#include "BanTarget.hpp"
#include "BansDatabase.hpp"

namespace rtype::server {

void BanBook::open(const std::filesystem::path& file,
                   std::chrono::sys_seconds now) {
  file_ = file;
  std::error_code error;
  if (std::filesystem::exists(file_, error)) {
    list_ = BanList{database().inForce(now)};
  }
}

void BanBook::close() { database_.reset(); }

void BanBook::put(const Ban& ban) {
  database().save(ban);
  list_.put(ban);
}

bool BanBook::remove(const BanTarget& target) {
  if (database_.has_value()) {
    static_cast<void>(database_->remove(target));
  }
  return list_.remove(target);
}

const BanList& BanBook::list() const { return list_; }

BansDatabase& BanBook::database() {
  if (!database_.has_value()) {
    database_.emplace(file_);
  }
  return *database_;
}

}  // namespace rtype::server
