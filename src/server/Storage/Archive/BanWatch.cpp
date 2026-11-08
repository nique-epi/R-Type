#include "BanWatch.hpp"
#include <SQLiteCpp/Exception.h>
#include <chrono>
#include <filesystem>
#include <stop_token>
#include <string>
#include <thread>
#include <utility>
#include "ArchiveConstants.hpp"
#include "BanList.hpp"
#include "BansDatabase.hpp"
#include "LatestBans.hpp"

namespace rtype::server {

BanWatch::BanWatch() : logger_(std::string{STORAGE_LOGGER_NAME}) {}

void BanWatch::open(const std::filesystem::path& file) {
  BansDatabase database{file};
  readAgain(database);
  worker_ =
      std::jthread{[this, database = std::move(database)](
                       const std::stop_token& stop) { run(stop, database); }};
}

void BanWatch::close() { worker_ = std::jthread{}; }

LatestBans& BanWatch::bans() { return bans_; }

void BanWatch::run(const std::stop_token& stop, const BansDatabase& database) {
  auto lastRead = std::chrono::steady_clock::now();
  while (!stop.stop_requested()) {
    std::this_thread::sleep_for(STORAGE_POLL_INTERVAL);
    if (std::chrono::steady_clock::now() - lastRead >= BAN_REREAD_INTERVAL) {
      lastRead = std::chrono::steady_clock::now();
      readAgain(database);
    }
  }
}

void BanWatch::readAgain(const BansDatabase& database) {
  try {
    bans_.publish(
        BanList{database.inForce(std::chrono::floor<std::chrono::seconds>(
            std::chrono::system_clock::now()))});
  } catch (const SQLite::Exception& error) {
    logger_.warn("bans not read again, the ones read before stay: ",
                 error.what());
  }
}

}  // namespace rtype::server
