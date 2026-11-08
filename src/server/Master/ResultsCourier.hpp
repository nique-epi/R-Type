#pragma once

#include <filesystem>
#include <optional>
#include <stop_token>
#include <string>
#include <vector>
#include "LastProblem.hpp"
#include "Logger.hpp"
#include "MasterClient.hpp"
#include "ResultDelivery.hpp"
#include "ResultsDatabase.hpp"
#include "UnsentGame.hpp"

namespace rtype::server {

/**
 * @brief Sends the master the vanilla games it has not answered yet, from the
 * file the games of this supervisor write their results to.
 *
 * It runs on MasterLink only, which owns it and the connection it opens to
 * the file: the games open the same file from their own processes, and SQLite
 * keeps them apart (see storage::openDatabaseFile()). The file is opened at
 * the first sending, never before: a supervisor that runs alone opens nothing.
 *
 * A result the master accepts is marked sent; one it refuses for good is
 * marked refused and written in the journal once; every other result waits,
 * in the file, for a later sending, so a master that was down gets every
 * result once it is back, a server restart in between included.
 */
class ResultsCourier {
 public:
  /** @param file The file of the results of the games (IResultsFile).
   * @param client The client of the master, used from MasterLink only.
   * @param problems The last problem of the link, which the heartbeats write
   * too. Both must outlive the courier. */
  ResultsCourier(std::filesystem::path file, network::MasterClient& client,
                 LastProblem& problems);

  /**
   * @brief Sends at most MAXIMUM_RESULTS_PER_ROUND waiting games, the oldest
   * first, one request each; stops at the first one that must wait, or once
   * @p stop is requested. A file that cannot be opened or read is written in
   * the journal once, and opened again at the next call.
   */
  void sendWaitingResults(const std::stop_token& stop);

 private:
  /** @returns The file, opened at the first call; nullptr when it cannot be
   * opened. */
  [[nodiscard]] ResultsDatabase* openedDatabase();
  /** @returns The waiting games of @p database; none when it cannot be read.
   */
  [[nodiscard]] std::vector<UnsentGame> waitingGames(
      const ResultsDatabase& database);
  /** @returns What became of @p game, sent to the master. */
  [[nodiscard]] ResultDelivery deliver(const UnsentGame& game);
  /** @brief Writes in @p database that the master answered @p gameId with
   * @p delivery, Accepted or Refused. */
  void keep(ResultsDatabase& database, const std::string& gameId,
            ResultDelivery delivery);

  std::filesystem::path file_;
  network::MasterClient& client_;
  std::optional<ResultsDatabase> database_;
  logging::Logger logger_;
  LastProblem& problems_;
};

}  // namespace rtype::server
