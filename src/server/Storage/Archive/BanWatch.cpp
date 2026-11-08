#include "BanWatch.hpp"
#include <SQLiteCpp/Exception.h>
#include <filesystem>
#include <stop_token>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "ArchiveConstants.hpp"
#include "Ban.hpp"
#include "BanList.hpp"
#include "BansDatabase.hpp"
#include "CurrentSecond.hpp"
#include "InterruptibleSleep.hpp"
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
  concurrency::sleepUnlessStopped(stop, BAN_REREAD_INTERVAL);
  while (!stop.stop_requested()) {
    readAgain(database);
    concurrency::sleepUnlessStopped(stop, BAN_REREAD_INTERVAL);
  }
}

void BanWatch::readAgain(const BansDatabase& database) {
  try {
    std::vector<Ban> standing = database.inForce(currentSecond());
    if (published_ != standing) {
      published_ = standing;
      bans_.publish(BanList{std::move(standing)});
    }
  } catch (const SQLite::Exception& error) {
    logger_.warn("bans not read again, the ones read before stay: ",
                 error.what());
  }
}

}  // namespace rtype::server
