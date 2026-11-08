#include "ResultsArchive.hpp"
#include <cstddef>
#include <exception>
#include <filesystem>
#include <stop_token>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "ArchiveConstants.hpp"
#include "GameResult.hpp"
#include "LeaderboardAsk.hpp"
#include "LeaderboardReply.hpp"
#include "MessagePayload.hpp"
#include "ResultsDatabase.hpp"

namespace rtype::server {

ResultsArchive::ResultsArchive()
    : results_(WAITING_RESULTS_CAPACITY),
      asks_(WAITING_ASKS_CAPACITY),
      logger_(std::string{STORAGE_LOGGER_NAME}) {
  resultsToSave_.reserve(WAITING_RESULTS_CAPACITY);
  asksToAnswer_.reserve(WAITING_ASKS_CAPACITY);
}

void ResultsArchive::open(const std::filesystem::path& file) {
  worker_ = std::jthread{
      [this, database = ResultsDatabase{file}](
          const std::stop_token& stop) mutable { run(stop, database); }};
}

void ResultsArchive::close() { worker_ = std::jthread{}; }

void ResultsArchive::record(GameResult result) {
  results_.push(std::move(result));
}

void ResultsArchive::ask(LeaderboardAsk ask) { asks_.push(std::move(ask)); }

void ResultsArchive::run(const std::stop_token& stop,
                         ResultsDatabase& database) {
  while (!stop.stop_requested()) {
    workThrough(database);
    std::this_thread::sleep_for(STORAGE_POLL_INTERVAL);
  }
  workThrough(database);
}

void ResultsArchive::workThrough(ResultsDatabase& database) {
  asks_.drainInto(asksToAnswer_);
  results_.drainInto(resultsToSave_);
  for (const GameResult& result : resultsToSave_) {
    save(database, result);
  }
  resultsToSave_.clear();
  reportDiscardedResults();
  for (const LeaderboardAsk& ask : asksToAnswer_) {
    answer(database, ask);
  }
  asksToAnswer_.clear();
}

void ResultsArchive::save(ResultsDatabase& database, const GameResult& result) {
  try {
    database.save(result);
    logger_.info("saved the result of a game of ", result.players.size(),
                 " players");
  } catch (const std::exception& error) {
    logger_.error("the result of a game is lost: ", error.what());
  }
}

void ResultsArchive::answer(const ResultsDatabase& database,
                            const LeaderboardAsk& ask) {
  try {
    ask.replies->push(
        LeaderboardReply{.recipient = ask.asker,
                         .sessionToken = ask.sessionToken,
                         .payload = network::encodeMessage(
                             database.leaderboard(ask.request, ask.nickname))});
  } catch (const std::exception& error) {
    logger_.error("a leaderboard could not be read: ", error.what());
  }
}

void ResultsArchive::reportDiscardedResults() {
  const std::size_t discarded = results_.discardedCount();
  if (discarded != reportedDiscards_) {
    logger_.error("results of games were lost, the disk is too slow: ",
                  discarded - reportedDiscards_);
    reportedDiscards_ = discarded;
  }
}

}  // namespace rtype::server
