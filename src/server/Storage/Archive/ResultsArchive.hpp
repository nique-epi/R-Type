#pragma once

#include <cstddef>
#include <filesystem>
#include <stop_token>
#include <thread>
#include <vector>
#include "BoundedQueue.hpp"
#include "GameResult.hpp"
#include "IResultsArchive.hpp"
#include "LeaderboardAsk.hpp"
#include "Logger.hpp"
#include "ResultsDatabase.hpp"

namespace rtype::server {

/**
 * @brief The storage thread and the queues that reach it. Only this thread
 * talks to the database, which it owns, so a game never waits for the disk.
 *
 * Every 20 ms it saves the waiting results, then answers the waiting requests.
 * It takes the requests off their queue before the results, so a request made
 * after a result always reads that result. Nothing a game hands over is
 * lost by a stop: the thread empties the queues once more on its way out. A
 * process killed loses what still waits in a queue, at most one interval of
 * work, and nothing already committed.
 *
 * An error on one result or one request is logged and the next is handled: a
 * full disk costs results, not the games.
 */
class ResultsArchive final : public IResultsArchive {
 public:
  ResultsArchive();

  /**
   * @brief Opens @p file on the calling thread, so an error reaches the caller,
   * then starts the thread that owns it.
   * @throws storage::StorageException when the file cannot be opened; nothing
   * is left open then.
   */
  void open(const std::filesystem::path& file);

  /** @brief Asks the thread to finish what waits, and waits for it. Does
   * nothing when it does not run. */
  void close();

  void record(GameResult result) override;
  void ask(LeaderboardAsk ask) override;

 private:
  void run(const std::stop_token& stop, ResultsDatabase& database);
  void workThrough(ResultsDatabase& database);
  void save(ResultsDatabase& database, const GameResult& result);
  void answer(const ResultsDatabase& database, const LeaderboardAsk& ask);
  void reportDiscardedResults();

  concurrency::BoundedQueue<GameResult> results_;
  concurrency::BoundedQueue<LeaderboardAsk> asks_;
  std::vector<GameResult> resultsToSave_;
  std::vector<LeaderboardAsk> asksToAnswer_;
  std::size_t reportedDiscards_{0};
  logging::Logger logger_;
  std::jthread worker_;
};

}  // namespace rtype::server
